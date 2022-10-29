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
        linkTarget(column->getOrCreateChoiceNode(minefield), cost);
    }
}

void ChoiceNode::linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost)
{
    if(cost > 0)
    {
        mineForwardEdge = edgeTarget;
    }

    // add the target to our forward edges
    edgesForward.append({edgeTarget, cost});

    // add ourself to the target's back edges
    edgeTarget->edgesBack.append({sharedFromThis(), cost});
}

void ChoiceNode::calculateWaysToBeMine(int mineCount)
{
    // we count paths in the graph with a fixed total cost to see how many ways this choice could be a mine in valid configurations of the minefield
    QList<SolverFloat> totalWaysToBeMineForForwardMineCount;

    // the mineCount input is how many total mines will distribute throughout the graph
    // since we have to distribute them before and after this choice, so we need to loop across all possible distributions
    for(int i = 0; i <= mineCount; ++i)
    {
        auto mineForwardEdgeStrong = mineForwardEdge.toStrongRef();

        // first we find the paths using mines forward
        // if i == 0, this will be fine because the function will see there are no paths forward      
        SolverFloat pathsForwardIfMine = mineForwardEdgeStrong? mineForwardEdgeStrong->findPathsForward(i - 1) : 0;

        if(tailPathCellCount > 0)
        {// if there are no tail path cells this logic is irrelevant
            // additionally, having a non-zero value for tail path cells is only possible for the trailing endpoint
            // the paths forward for mine or clear are found with the choose function
            // because there's no information on how they're distributed
            // if we're a mine we choose i - 1 from the remaining tail path cells (we are one of them)
            // if we're not a mine we choose i from the remaining tail path cells (we are one of them)
            pathsForwardIfMine = SolverMath::choose(tailPathCellCount - 1, i - 1);
        }

        // then we find the opposite count of paths using mines that go back to the start node
        SolverFloat pathsBack = findPathsBack(mineCount - i);

        // these paths combine multiplicatively
        SolverFloat waysToBeMine = pathsBack * pathsForwardIfMine;

        totalWaysToBeMineForForwardMineCount.append(waysToBeMine);
    }

    auto sum = [] (const QList<SolverFloat>& list) { SolverFloat result = 0; for(const SolverFloat &i : list) result += i; return result; };

    waysToBeMine = sum(totalWaysToBeMineForForwardMineCount);
}

SolverFloat ChoiceNode::getWaysToBeMine() const
{
    return waysToBeMine;
}

bool ChoiceNode::isEndpoint() const
{
    return endpoint;
}

void ChoiceNode::setEndpoint(bool newEndpoint)
{
    endpoint = newEndpoint;
}

void ChoiceNode::setTailPathCellCount(int count)
{
    // this value is only set for the choice node that represents the tail path
    tailPathCellCount = count;
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
        SolverFloat tailPathCount = tailPathCounts.value(mineCount, -1);

        if(tailPathCount < 0)
        {// remember the value for this mine count, it's not going to change
            tailPathCount = tailPathCellCount > 0? SolverMath::choose(tailPathCellCount, mineCount) : 1;
            tailPathCounts[mineCount] = tailPathCount;
        }

        return isEndpoint()? tailPathCount : 0;
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
