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
    // hopefully double is enough to store the ways it can be a mine... Might need to change data types around eventually
    return static_cast<double>(waysToBeMine) / (waysToBeMine + waysToBeClear);
}

qint64 ChoiceColumn::getWaysToBeMine() const
{
    return waysToBeMine;
}

qint64 ChoiceColumn::getWaysToBeClear() const
{
    return waysToBeClear;
}
