#include "Solver.h"

#include "ChoiceColumn.h"
#include "ChoiceNode.h"
#include "Minefield.h"
#include "ObviousCellFlagger.h"
#include "PathChooser.h"
#include "ProgressProxy.h"

#include <algorithm>
#include <QDebug>

#define CHECK_CANCELLED if(cancelled) return;

Q_DECLARE_METATYPE(QSharedPointer<Solver>)

Solver::Solver(QSharedPointer<Minefield const> gameMinefield, QHash<Coordinate, double> previousMineChances)
    : startingMinefield(gameMinefield->getRevealedMinefield(), gameMinefield->getWidth(), gameMinefield->getHeight()),
      mineCount(gameMinefield->getMineCount()), minefieldPopulated(gameMinefield->isPopulated()), previousMineChances(previousMineChances)
    // clone the passed in minefield so this is thread safe with multiple solves vs the same field
{
    static int registeredMetatype = qRegisterMetaType<QSharedPointer<Solver>>();

    cancelled = false;

    choiceColumns.clear();
    columnCounts.clear();
    chancesToBeMine.clear();
    legalFieldCount = 0;

    progress = progress.create();
}

Solver::~Solver()
{
    // don't fully destroy the solver if we're waiting on a future that's interacting with the columns we would destroy
    currentFuture.waitForFinished();
}

void Solver::computeSolution()
{
    flagObviousCells();
    decidePath();
    buildSolutionGraph();
    analyzeSolutionGraph();
}

const QHash<Coordinate, double> &Solver::getChancesToBeMine() const
{
    return chancesToBeMine;
}

const QHash<Coordinate, int> &Solver::getColumnCounts() const
{
    return columnCounts;
}

int Solver::getLogLegalFieldCount() const
{
    return boost::multiprecision::log2(legalFieldCount).convert_to<int>();
}

int Solver::getPathIndex(const Coordinate &coord) const
{
    return path.indexOf(coord);
}

QSharedPointer<ProgressProxy> Solver::getProgress() const
{
    return progress;
}

void Solver::flagObviousCells()
{
    CHECK_CANCELLED;

    progress->emitProgressStep("Flagging obvious cells");

    // the previously calculated chances are the most obvious
    prepareStartingMinefield(previousMineChances);

    CHECK_CANCELLED;

    ObviousCellFlagger flagger(startingMinefield);

    flagger.flagObviousCells();

    CHECK_CANCELLED;

    // and now we've identified some more obvious ones
    prepareStartingMinefield(flagger.getChancesToBeMine());

    SolverMinefield flaggerMinefield = startingMinefield;
}

void Solver::decidePath()
{
    CHECK_CANCELLED;

    progress->emitProgressStep("Deciding path.");

    PathChooser chooser(startingMinefield);

    chooser.decidePath();

    path = chooser.getPath();
    tailPath = chooser.getTailPath();

    // there are four computational loops that go over the path size and a cleanup step
    progress->emitProgressMaximum(5 * path.size());

    if(logProgress)
    {
        qDebug() << "path length" << path.size();
    }
}

void Solver::buildSolutionGraph()
{
    CHECK_CANCELLED;

    progress->emitProgressStep("Building solution graph.");

    for(Coordinate coord: path)
    {
        // build the choice columns
        choiceColumns.append(QSharedPointer<ChoiceColumn>::create(coord.first, coord.second));
    }

    // the final column doesn't have a choice anymore and is just the end state where all choices have been made and the board is done
    choiceColumns.append(QSharedPointer<ChoiceColumn>::create(-1, -1));

    auto initialChoiceColumn = choiceColumns.first();

    // the starting node is the current state of the revealed minefield, with a choice pending for the first cell that we will visit
    QSharedPointer<ChoiceNode> startingNode(new ChoiceNode(startingMinefield, initialChoiceColumn->getX(), initialChoiceColumn->getY()));

    // adding the initial node gives us a starting point for the graph
    initialChoiceColumn->addChoiceNode(startingNode);

    // we skip the last one because there's nothing for it to connect to
    for(int i = 0; i < choiceColumns.size() - 1; ++i)
    {
        CHECK_CANCELLED;

        auto currentColumn = choiceColumns[i];
        auto nextColumn = choiceColumns[i + 1];

        // we traverse each state in the current column and generate the successor states in the next column
        for(auto choiceNode : currentColumn->getChoiceNodes())
        {
            choiceNode->addSuccessorsToNextColumn(nextColumn);
        }

        progress->incrementProgress();
    }

    auto finalColumnChoiceNodes = choiceColumns.last()->getChoiceNodes();
    if(finalColumnChoiceNodes.size() > 0)
    {
        // the only cell of the final choice column needs to account for the tail path cells
        finalColumnChoiceNodes.first()->setTailPathCellCount(tailPath.size());
    }

    qsizetype maxColumnSize = 0;

    for(const auto &column : choiceColumns)
    {
        columnCounts.insert({column->getX(), column->getY()}, column->getChoiceNodes().size());
        maxColumnSize = std::max(maxColumnSize, column->getChoiceNodes().size());
    }

    if(logProgress)
    {
        qDebug() << "largest column has" << maxColumnSize << "nodes";
        qDebug() << "last column has" << choiceColumns.last()->getChoiceNodes().size() << "nodes";
    }
}

void Solver::analyzeSolutionGraph()
{
    CHECK_CANCELLED;

    progress->emitProgressStep("Analyzing solution graph.");

    if(logProgress)
    {
        qDebug() << "Analyzing...";
    }

    for(const QSharedPointer<ChoiceColumn> &column : {choiceColumns.first(), choiceColumns.last()})
    {// the final two columns are endpoints
        assert(column->getChoiceNodes().size() == 1);

        column->getChoiceNodes().first()->setEndpoint(true);
    }

    // ultimately we want to calculate for each column, the ways it could be a mine and the ways it could be clear
    // this requires counting paths through the columns
    // in order to avoid recursion, we precalculate these path counts for each column

    for(auto column : choiceColumns)
    {// we start from the beginning and move forward to precompute the paths back since each column depends on the previous
        CHECK_CANCELLED;

        currentFuture = column->precomputePathsBack(mineCount);
        currentFuture.waitForFinished();

        progress->incrementProgress();
    }

    for(auto iter = choiceColumns.rbegin(); iter != choiceColumns.rend(); ++iter)
    {// we start from the end of the columns and move backward to precompute the paths forward since each column depends on the next
        CHECK_CANCELLED;

        currentFuture = (*iter)->precomputePathsForward(mineCount);
        currentFuture.waitForFinished();

        progress->incrementProgress();
    }

    if(logProgress)
    {
        qDebug() << "path count precompution complete";
    }
    
    // the total number of valid fields is the number of paths forward from the first node
    auto validMinefieldCount = choiceColumns.first()->getChoiceNodes().first()->findPathsForward(mineCount);

    for(auto column : choiceColumns)
    {// calculate all the ways to be
        CHECK_CANCELLED;

        // set it so we can compute percentages
        column->setValidMinefieldCount(validMinefieldCount);
        
        // we compute the ways to be for all columns, including the final column
        currentFuture = column->calculateWaysToBeMine(mineCount);
        currentFuture.waitForFinished();

        if(column->getX() >= 0 && column->getY() >= 0)
        {// the final column has -1, -1, we don't insert chances for it at its coordinate as it represents all tail path cells
            chancesToBeMine.insert({column->getX(), column->getY()}, column->getPercentChanceToBeMine());
        }

        progress->incrementProgress();
    }

    for(Coordinate coord : tailPath)
    {// for the tail path cells we use the chance to be a mine from the final column since it represents them
        // the logic for generating the chance to be a mine is specialized for this column to produce correct results using a formula
        // this can be computed with a formula because there's no information about how the mines are distributed among the unknown cells off the path
        chancesToBeMine.insert({coord.first, coord.second}, choiceColumns.last()->getPercentChanceToBeMine());
    }

    // if all mines are known and passed in as previous state, it's possible for there to only be one choice column at (-1, -1)
    legalFieldCount = std::max(static_cast<SolverFloat>(1), validMinefieldCount);

    if(!minefieldPopulated)
    {// if the minefield was unpopulated there's actually no chance to get a mine because the first click is guaranteed to be not a mine
        chancesToBeMine.clear();
    }

    progress->emitProgressStep("Cleaning up.");

    // no longer need the data structure now that we have the final results
    // this can take some time so we don't want this step before complete
    while(!choiceColumns.isEmpty())
    {
        choiceColumns.removeFirst();

        progress->incrementProgress();
    }

    progress->emitProgressStep("Complete.");

    if(logProgress)
    {
        qDebug() << "processed" << choiceColumns.size();
        qDebug() << "analysis complete";
    }
}

void Solver::prepareStartingMinefield(const QHash<Coordinate, double>& previousMineChances)
{
    auto coords = previousMineChances.keys();

    for(const auto& coord : coords)
    {
        // for clear cells, we only do this if they haven't already been eliminated by becoming count cells
        if(previousMineChances[coord] == 0 && startingMinefield.getCell(coord.first, coord.second) < 0)
        {
            chancesToBeMine[coord] = 0;
            startingMinefield = startingMinefield.chooseClear(coord.first, coord.second);
        }

        if(previousMineChances[coord] == 1)
        {
            chancesToBeMine[coord] = 1;
            startingMinefield = startingMinefield.chooseMine(coord.first, coord.second);
            mineCount--;
        }
    }
}

void Solver::setLogProgress(bool newLogProgress)
{
    logProgress = newLogProgress;
}

void Solver::cancel()
{
    cancelled = true;

    currentFuture.cancel();
}
