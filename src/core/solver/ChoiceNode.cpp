#include "ChoiceNode.h"

#include "ChoiceColumn.h"

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
    QList<double> totalWaysToBeMineForForwardMineCount;
    QList<double> totalWaysToBeClearForForwardMineCount;

    // the mineCount input is how many total mines will distribute throughout the graph
    // since we have to distribute them before and after this choice, so we need to loop across all possible distributions
    for(int i = 0; i <= mineCount; ++i)
    {
        auto mineForwardEdgeStrong = mineForwardEdge.toStrongRef();
        auto clearFowardEdgeStrong = clearForwardEdge.toStrongRef();

        // first we find the paths using mines forward
        // if i == 0, this will be fine because the function will see there are no paths forward      
        double pathsForwardIfMine = mineForwardEdgeStrong? mineForwardEdgeStrong->findPathsForward(i - 1) : 0;
        double pathsForwardIfClear = clearFowardEdgeStrong? clearFowardEdgeStrong->findPathsForward(i) : 0;

        // then we find the opposite count of paths using mines that go back to the start node
        double pathsBack = findPathsBack(mineCount - i);

        // these paths combine multiplicatively
        double waysToBeMine = pathsBack * pathsForwardIfMine;
        double waysToBeClear = pathsBack * pathsForwardIfClear;

        totalWaysToBeMineForForwardMineCount.append(waysToBeMine);
        totalWaysToBeClearForForwardMineCount.append(waysToBeClear);
    }

    auto sum = [] (const QList<double>& list) { double result = 0; for(const double &i : list) result += i; return result; };

    waysToBeMine = sum(totalWaysToBeMineForForwardMineCount);
    waysToBeClear = sum(totalWaysToBeClearForForwardMineCount);
}

double ChoiceNode::getWaysToBeMine() const
{
    return waysToBeMine;
}

double ChoiceNode::getWaysToBeClear() const
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

double ChoiceNode::findPathsForward(int mineCount) const
{
    return findPaths(mineCount, true);
}

double ChoiceNode::findPathsBack(int mineCount) const
{
    return findPaths(mineCount, false);
}

double ChoiceNode::findPaths(int mineCount, bool forward) const
{
    if(mineCount < 0)
    {// no valid path, used too much cost
        return 0;
    }

    if((forward && edgesForward.isEmpty())
            || (!forward && edgesBack.isEmpty()))
    {// no more edges to proceed through, only report a path if we're at exactly 0 mines and have reached an endpoint
        return mineCount == 0 && isEndpoint()? 1 : 0;
    }

    double sumOfEdges = 0;

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
