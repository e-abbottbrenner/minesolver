#include "ChoiceColumn.h"

#include "ChoiceNode.h"

ChoiceColumn::ChoiceColumn(int x, int y)
    : x(x), y(y)
{
}

QSharedPointer<ChoiceNode> ChoiceColumn::getChoiceNode(const QByteArray &minefieldBytes) const
{
    // we use the state as a key to get the choice node that corresponds to that state
    // technically if width could change we'd need the width factored in, but it can't so we don't
    // this may need optimization since it doubles the memory footprint
    return choicesInColumn.value(minefieldBytes, {});
}

void ChoiceColumn::addChoiceNode(QSharedPointer<ChoiceNode> node)
{
    choicesInColumn.insert(node->getMinefield().getMinefieldBytes(), node);
}

QList<QSharedPointer<ChoiceNode> > ChoiceColumn::getChoiceNodes() const
{
    return choicesInColumn.values();
}

int ChoiceColumn::getX() const
{
    return x;
}

int ChoiceColumn::getY() const
{
    return y;
}

void ChoiceColumn::precomputePathsForward(int mineCount)
{
    for(auto choiceNode : getChoiceNodes())
    {
        choiceNode->precomputePathsForward(mineCount);
    }
}

void ChoiceColumn::precomputePathsBack(int mineCount)
{
    for(auto choiceNode : getChoiceNodes())
    {
        choiceNode->precomputePathsBack(mineCount);
    }
}

void ChoiceColumn::calculateWaysToBe(int mineCount)
{
    waysToBeMine = 0;
    waysToBeClear = 0;

    for(auto choiceNode : getChoiceNodes())
    {
        choiceNode->calculateWaysToBe(mineCount);

        waysToBeMine += choiceNode->getWaysToBeMine();
        waysToBeClear += choiceNode->getWaysToBeClear();
    }
}

double ChoiceColumn::getPercentChanceToBeMine() const
{
    // cast in case we rework the type again to be larger than a normal double
    return static_cast<double>(waysToBeMine / (waysToBeMine + waysToBeClear));
}

SolverFloat ChoiceColumn::getWaysToBeMine() const
{
    return waysToBeMine;
}

SolverFloat ChoiceColumn::getWaysToBeClear() const
{
    return waysToBeClear;
}
