#include "Solver.h"

#include "ChoiceColumn.h"
#include "ChoiceNode.h"
#include "Minefield.h"
#include "PathChooser.h"

Solver::Solver(Minefield *minefield)
    : minefield(minefield)
{

}

void Solver::computeSolution()
{
    decidePath();
    buildSolutionGraph();
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

        // we traverse each state in the current column and generate the successor states in the next column
        for(auto choiceNode : currentColumn->getChoiceNodes())
        {
            choiceNode->addSuccessorsToColumn(nextColumn);
        }
    }
}
