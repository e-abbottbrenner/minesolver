#include "Solver.h"

#include "ChoiceColumn.h"
#include "ChoiceNode.h"
#include "Minefield.h"
#include "PathChooser.h"

#include <QDebug>

Solver::Solver(Minefield *minefield)
    : minefield(minefield)
{

}

void Solver::computeSolution()
{
    decidePath();
    buildSolutionGraph();
    analyzeSolutionGraph();
}

const QHash<Coordinate, double> &Solver::getChancesToBeMine() const
{
    return chancesToBeMine;
}

void Solver::decidePath()
{
    PathChooser chooser(minefield);

    chooser.decidePath();

    path = chooser.getPath();
}

void Solver::buildSolutionGraph()
{
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
        auto currentColumn = choiceColumns[i];
        auto nextColumn = choiceColumns[i + 1];

        qDebug() << "processing column with" << currentColumn->getChoiceNodes().size() << "nodes";

        // we traverse each state in the current column and generate the successor states in the next column
        for(auto choiceNode : currentColumn->getChoiceNodes())
        {
            choiceNode->addSuccessorsToNextColumn(nextColumn);
        }
    }

    qDebug() << "last column has" << choiceColumns.last()->getChoiceNodes().size() << "nodes";
}

void Solver::analyzeSolutionGraph()
{
    qDebug() << "Analyzing...";

    for(auto column : choiceColumns)
    {// we start from the beginning and move forward to precompute paths in reverse since each column depends on the previous
        column->precomputePathsReverse(minefield->getNumMines());
    }

    for(auto iter = choiceColumns.rbegin(); iter != choiceColumns.rend(); ++iter)
    {// we start from the end of the columns to precompute the paths forward since each column depends on the next
        (*iter)->precomputePathsForward(minefield->getNumMines());
    }

    qDebug() << "path count precompution complete";

    for(auto column : choiceColumns)
    {// calculate all the ways to be
        if(column->getX() >= 0 && column->getY() >= 0)
        {// the final column has -1, -1
            column->calculateWaysToBe(minefield->getNumMines());

            std::cout << column->getX() << " " << column->getY() << " " << column->getWaysToBeMine() << " " << column->getWaysToBeClear() << " " << column->getPercentChanceToBeMine() << std::endl;

            chancesToBeMine.insert({column->getX(), column->getY()}, column->getPercentChanceToBeMine());
        }
    }
}
