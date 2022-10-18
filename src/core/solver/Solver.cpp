#include "Solver.h"

#include "ChoiceColumn.h"
#include "ChoiceNode.h"
#include "Minefield.h"
#include "PathChooser.h"
#include "ProgressProxy.h"

#include <algorithm>
#include <QDebug>

#define CHECK_CANCELLED if(cancelled) return;

Solver::Solver(QSharedPointer<Minefield const> minefield)
    : minefield(minefield->clone())
    // clone the passed in minefield so this is thread safe with multiple solves vs the same field
{
    progress = progress.create();
}

void Solver::computeSolution()
{
    progress->reset();
    cancelled = false;

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
    return std::log2(legalFieldCount);
}

int Solver::getPathIndex(const Coordinate &coord) const
{
    return path.indexOf(coord);
}

QSharedPointer<ProgressProxy> Solver::getProgress() const
{
    return progress;
}

void Solver::decidePath()
{
    CHECK_CANCELLED;

    progress->emitProgressStep("Deciding path.");

    PathChooser chooser(minefield);

    chooser.decidePath();

    path = chooser.getPath();

    // there are four computational loops that go over the path size
    progress->emitProgressMaximum(4 * path.size());

    if(logProgress)
    {
        qDebug() << "path length" << path.size();
    }
}

void Solver::buildSolutionGraph()
{
    CHECK_CANCELLED;

    progress->emitProgressStep("Building solution graph.");

    choiceColumns.clear();
    columnCounts.clear();
    chancesToBeMine.clear();
    legalFieldCount = 0;

    for(Coordinate coord: path)
    {
        // build the choice columns
        choiceColumns.append(QSharedPointer<ChoiceColumn>::create(coord.first, coord.second));
    }

    // the final column doesn't have a choice anymore and is just the end state where all choices have been made and the board is done
    choiceColumns.append(QSharedPointer<ChoiceColumn>::create(-1, -1));

    auto initialChoiceColumn = choiceColumns.first();

    // the starting node is the current state of the revealed minefield, with a choice pending for the first cell that we will visit
    QSharedPointer<ChoiceNode> startingNode(new ChoiceNode(PotentialMinefield(minefield->getRevealedMinefield(),
                                                                              minefield->getWidth(), minefield->getHeight()),
                                                           initialChoiceColumn->getX(), initialChoiceColumn->getY()));

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

        column->precomputePathsBack(minefield->getNumMines());

        progress->incrementProgress();
    }

    for(auto iter = choiceColumns.rbegin(); iter != choiceColumns.rend(); ++iter)
    {// we start from the end of the columns and move backward to precompute the paths forward since each column depends on the next
        CHECK_CANCELLED;

        (*iter)->precomputePathsForward(minefield->getNumMines());

        progress->incrementProgress();
    }

    if(logProgress)
    {
        qDebug() << "path count precompution complete";
    }

    for(auto column : choiceColumns)
    {// calculate all the ways to be
        CHECK_CANCELLED;

        if(column->getX() >= 0 && column->getY() >= 0)
        {// the final column has -1, -1
            column->calculateWaysToBe(minefield->getNumMines());

            chancesToBeMine.insert({column->getX(), column->getY()}, column->getPercentChanceToBeMine());
        }

        progress->incrementProgress();
    }

    legalFieldCount = choiceColumns.first()->getWaysToBeClear() + choiceColumns.first()->getWaysToBeMine();

    progress->emitProgressStep("Complete.");

    if(logProgress)
    {
        qDebug() << "processed" << choiceColumns.size();
        qDebug() << "analysis complete";
    }
}

void Solver::setLogProgress(bool newLogProgress)
{
    logProgress = newLogProgress;
}

void Solver::cancel()
{
    cancelled = true;
}
