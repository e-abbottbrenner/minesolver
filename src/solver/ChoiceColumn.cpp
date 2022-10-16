#include "ChoiceColumn.h"

#include "ChoiceNode.h"

using boost::multiprecision::cpp_rational;

int ChoiceColumn::choiceColumnsConstructed = 0;

ChoiceColumn::ChoiceColumn(int x, int y)
    : x(x), y(y)
{
    // tracking for tests to verify no leaks
    ++choiceColumnsConstructed;
}

ChoiceColumn::~ChoiceColumn()
{
    --choiceColumnsConstructed;
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

void ChoiceColumn::precomputePathsReverse(int mineCount)
{
    for(auto choiceNode : getChoiceNodes())
    {
        choiceNode->precomputePathsReverse(mineCount);
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
    return static_cast<double>(static_cast<cpp_rational>(waysToBeMine) / (waysToBeMine + waysToBeClear));
}

cpp_int ChoiceColumn::getWaysToBeMine() const
{
    return waysToBeMine;
}

cpp_int ChoiceColumn::getWaysToBeClear() const
{
    return waysToBeClear;
}

int ChoiceColumn::getChoiceColumnsConstructed()
{
    return choiceColumnsConstructed;
}
