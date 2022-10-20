#include "ChoiceNode.h"

#include "ChoiceColumn.h"
#include "SolverMath.h"

ChoiceNode::ChoiceNode(const SolverMinefield &minefield, int x, int y)
    : minefield(minefield), x(x), y(y)
{
}

const SolverMinefield &ChoiceNode::getMinefield() const
{
    return minefield;
}

const QList<ChoiceNode::Edge> &ChoiceNode::getEdgesForward() const
{
    return edgesForward;
}

const QList<ChoiceNode::Edge> &ChoiceNode::getEdgesBack() const
{
    return edgesBack;
}

void ChoiceNode::addSuccessorsToNextColumn(QSharedPointer<ChoiceColumn> nextColumn)
{
    // these are what the minefields look like if the column's x/y is clear or a mine
    SolverMinefield fieldIfMine = minefield.chooseMine(x, y);
    SolverMinefield fieldIfClear = minefield.chooseClear(x, y);

    // we try to add edges to the next column, it can fail because the state may be illegal
    tryAddEdge(nextColumn, fieldIfMine, 1);
    tryAddEdge(nextColumn, fieldIfClear, 0);
}

void ChoiceNode::precomputePathsForward(int mineCount)
{
    precomputePaths(mineCount, true);
}

void ChoiceNode::precomputePathsBack(int mineCount)
{
    precomputePaths(mineCount, false);
}

void ChoiceNode::tryAddEdge(QSharedPointer<ChoiceColumn> column, const SolverMinefield &minefield, int cost)
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
    // add ourself to the target's back edges
    edgeTarget->edgesBack.append({sharedFromThis(), cost});
}

void ChoiceNode::calculateWaysToBe(int mineCount)
{
    // we count paths in the graph with a fixed total cost to see how many ways this choice could be a mine in valid configurations of the minefield
    QList<SolverFloat> totalWaysToBeMineForForwardMineCount;
    QList<SolverFloat> totalWaysToBeClearForForwardMineCount;

    // the mineCount input is how many total mines will distribute throughout the graph
    // since we have to distribute them before and after this choice, so we need to loop across all possible distributions
    for(int i = 0; i <= mineCount; ++i)
    {
        auto mineForwardEdgeStrong = mineForwardEdge.toStrongRef();
        auto clearFowardEdgeStrong = clearForwardEdge.toStrongRef();

        // first we find the paths using mines forward
        // if i == 0, this will be fine because the function will see there are no paths forward      
        SolverFloat pathsForwardIfMine = mineForwardEdgeStrong? mineForwardEdgeStrong->findPathsForward(i - 1) : 0;
        SolverFloat pathsForwardIfClear = clearFowardEdgeStrong? clearFowardEdgeStrong->findPathsForward(i) : 0;

        if(offPathCellCount > 0)
        {// if there are no off path cells this logic is irrelevant
            // additionally, having a non-zero value for off path cells is only possible for the trailing endpoint
            // the paths forward for mine or clear are found with the choose function
            // because there's no information on how they're distributed
            // if we're a mine we choose i - 1 from the remaining off path cells (we are one of them)
            // if we're not a mine we choose i from the remaining off path cells (we are one of them)
            pathsForwardIfMine = SolverMath::choose(offPathCellCount - 1, i - 1);
            pathsForwardIfClear = SolverMath::choose(offPathCellCount - 1, i);
        }

        // then we find the opposite count of paths using mines that go back to the start node
        SolverFloat pathsBack = findPathsBack(mineCount - i);

        // these paths combine multiplicatively
        SolverFloat waysToBeMine = pathsBack * pathsForwardIfMine;
        SolverFloat waysToBeClear = pathsBack * pathsForwardIfClear;

        totalWaysToBeMineForForwardMineCount.append(waysToBeMine);
        totalWaysToBeClearForForwardMineCount.append(waysToBeClear);
    }

    auto sum = [] (const QList<SolverFloat>& list) { SolverFloat result = 0; for(const SolverFloat &i : list) result += i; return result; };

    waysToBeMine = sum(totalWaysToBeMineForForwardMineCount);
    waysToBeClear = sum(totalWaysToBeClearForForwardMineCount);
}

SolverFloat ChoiceNode::getWaysToBeMine() const
{
    return waysToBeMine;
}

SolverFloat ChoiceNode::getWaysToBeClear() const
{
    return waysToBeClear;
}

bool ChoiceNode::isEndpoint() const
{
    return endpoint;
}

void ChoiceNode::setEndpoint(bool newEndpoint)
{
    endpoint = newEndpoint;
}

void ChoiceNode::setOffPathCellCount(int count)
{
    // if this value is set
    offPathCellCount = count;
}

SolverFloat ChoiceNode::findPathsForward(int mineCount) const
{
    return findPaths(mineCount, true);
}

SolverFloat ChoiceNode::findPathsBack(int mineCount) const
{
    return findPaths(mineCount, false);
}

SolverFloat ChoiceNode::findPaths(int mineCount, bool forward) const
{
    if(mineCount < 0)
    {// no valid path, used too much cost
        return 0;
    }

    if(forward && edgesForward.isEmpty())
    {// no more edges to proceed through, only report a path if we're at exactly 0 mines and have reached an endpoint
        SolverFloat offPathPathCount = offPathPathCounts.value(mineCount, -1);

        if(offPathPathCount < 0)
        {// remember the value for this mine count, it's not going to change
            offPathPathCount = offPathCellCount > 0? SolverMath::choose(offPathCellCount, mineCount) : 1;
            offPathPathCounts[mineCount] = offPathPathCount;
        }

        return isEndpoint()? offPathPathCount : 0;
    }

    if((!forward && edgesBack.isEmpty()))
    {// no more edges to proceed through, only report a path if we're at exactly 0 mines and have reached an endpoint
        return (mineCount == 0 && isEndpoint())? 1 : 0;
    }

    SolverFloat sumOfEdges = 0;

    for(const Edge &edge : forward? edgesForward : edgesBack)
    {
        // rely on the assumption that the next node has already built the data
        int countAtNext = mineCount - edge.cost;
        if(countAtNext >= 0)
        {
            auto nextNodeStrong = edge.nextNode.toStrongRef();

            if(nextNodeStrong)
            {
                sumOfEdges += forward? nextNodeStrong->pathsForward[countAtNext] : nextNodeStrong->pathsBack[countAtNext];
            }
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
            pathsBack.append(findPaths(i, forward));
        }
    }
}
