#include "ChoiceNode.h"

#include "ChoiceColumn.h"

ChoiceNode::ChoiceNode(PotentialMinefield minefield, int x, int y)
    : minefield(minefield), x(x), y(y)
{

}

const PotentialMinefield &ChoiceNode::getMinefield() const
{
    return minefield;
}

const QList<ChoiceNode::Edge> &ChoiceNode::getEdgesForward() const
{
    return edgesForward;
}

const QList<ChoiceNode::Edge> &ChoiceNode::getEdgesReverse() const
{
    return edgesReverse;
}

void ChoiceNode::addSuccessorsToColumn(QSharedPointer<ChoiceColumn> column)
{
    // these are what the minefields look like if the column's x/y is clear or a mine
    PotentialMinefield fieldIfMine = minefield.chooseMine(column->getX(), column->getY());
    PotentialMinefield fieldIfClear = minefield.chooseClear(column->getX(), column->getY());

    // we try to add edges to the graph, it can fail because the state may be illegal
    tryAddEdge(column, fieldIfMine, 1);
    tryAddEdge(column, fieldIfClear, 0);
}

void ChoiceNode::tryAddEdge(QSharedPointer<ChoiceColumn> column, const PotentialMinefield &minefield, int cost)
{
    // we don't add edges to illegal states
    if(minefield.isLegal())
    {
        // there will often be an existing choice node in the column that has the same state, use it
        auto edgeTargetNode = column->getChoiceNode(minefield.getMinefieldBytes());

        if(edgeTargetNode.isNull())
        {// no existing target, so need to create one
            edgeTargetNode = edgeTargetNode.create(minefield, column->getX(), column->getY());
            // we add it to the column so it can be reused
            column->addChoiceNode(edgeTargetNode);
        }

        linkTarget(edgeTargetNode, cost);
    }
}

void ChoiceNode::linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost)
{
    // add the target to our forward edges
    edgesForward.append({edgeTarget, cost});
    // add ourself to the target's reverse edges
    edgeTarget->edgesReverse.append({sharedFromThis(), cost});
}
