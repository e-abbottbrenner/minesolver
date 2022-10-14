#include "ChoiceNode.h"

#include "ChoiceColumn.h"
#include "qdebug.h"

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

void ChoiceNode::addSuccessorsToNextColumn(QSharedPointer<ChoiceColumn> nextColumn)
{
    // these are what the minefields look like if the column's x/y is clear or a mine
    PotentialMinefield fieldIfMine = minefield.chooseMine(x, y);
    PotentialMinefield fieldIfClear = minefield.chooseClear(x, y);

    // we try to add edges to the next column, it can fail because the state may be illegal
    tryAddEdge(nextColumn, fieldIfMine, 1);
    tryAddEdge(nextColumn, fieldIfClear, 0);
}

void ChoiceNode::precomputePathsForward(int mineCount)
{
    precomputePaths(mineCount, true);
}

void ChoiceNode::precomputePathsReverse(int mineCount)
{
    precomputePaths(mineCount, false);
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
    if(cost > 0)
    {
        mineForwardEdge = edgeTarget;
    }
    else
    {
        clearForwardEdge = edgeTarget;
    }

    // add the target to our forward edges
    edgesForward.append({edgeTarget, cost});
    // add ourself to the target's reverse edges
    edgeTarget->edgesReverse.append({sharedFromThis(), cost});
}

void ChoiceNode::calculateWaysToBe(int mineCount)
{
    // we count paths in the graph with a fixed total cost to see how many ways this choice could be a mine in valid configurations of the minefield
    QList<qint64> totalWaysToBeMineForForwardMineCount;
    QList<qint64> totalWaysToBeClearForForwardMineCount;

    // the mineCount input is how many total mines will distribute throughout the graph
    // since we have to distribute them before and after this choice, so we need to loop across all possible distributions
    for(int i = 0; i <= mineCount; ++i)
    {
        // first we find the paths using mines forward
        // if i == 0, this will be fine because the function will see there are no paths forward
        qint64 pathsForwardIfMine = mineForwardEdge? mineForwardEdge->findPathsForward(i - 1) : 0;
        qint64 pathsForwardIfClear = clearForwardEdge? clearForwardEdge->findPathsForward(i) : 0;

        // then we find the opposite count of paths using mines that go back to the start node
        qint64 pathsReverse = findPathsReverse(mineCount - i);

        // these paths combine multiplicatively
        qint64 waysToBeMine = pathsReverse * pathsForwardIfMine;
        qint64 waysToBeClear = pathsReverse * pathsForwardIfClear;

        totalWaysToBeMineForForwardMineCount.append(waysToBeMine);
        totalWaysToBeClearForForwardMineCount.append(waysToBeClear);
    }

    auto sum = [] (const QList<qint64>& list) { qint64 result = 0; for(qint64 i : list) result += i; return result; };

    waysToBeMine = sum(totalWaysToBeMineForForwardMineCount);
    waysToBeClear = sum(totalWaysToBeClearForForwardMineCount);
}

qint64 ChoiceNode::getWaysToBeMine() const
{
    return waysToBeMine;
}

qint64 ChoiceNode::getWaysToBeClear() const
{
    return waysToBeClear;
}

int ChoiceNode::findPathsForward(int mineCount) const
{
    return findPaths(mineCount, true);
}

int ChoiceNode::findPathsReverse(int mineCount) const
{
    return findPaths(mineCount, false);
}

int ChoiceNode::findPaths(int mineCount, bool forward) const
{
    if(mineCount < 0)
    {// no valid path, used too much cost
        return 0;
    }

    if((forward && edgesForward.isEmpty())
            || (!forward && edgesReverse.isEmpty()))
    {// no more edges to proceed through, only report a path if we're at exactly 0 mines
        return mineCount == 0? 1 : 0;
    }

    int sumOfEdges = 0;

    for(const Edge &edge : forward? edgesForward : edgesReverse)
    {
        // rely on the assumption that the next node has already built the data
        int countAtNext = mineCount - edge.cost;
        if(countAtNext >= 0)
        {
            sumOfEdges += forward? edge.nextNode->pathsForward[countAtNext] : edge.nextNode->pathsReverse[countAtNext];
        }
    }

    return sumOfEdges;
}

void ChoiceNode::precomputePaths(int mineCount, bool forward)
{
    for(int i = 0; i <= mineCount; ++i)
    {
        if(forward)
        {
            pathsForward.append(findPaths(i, forward));
        }
        else
        {
            pathsReverse.append(findPaths(i, forward));
        }
    }
}