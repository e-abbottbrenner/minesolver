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

int ChoiceColumn::getX() const
{
    return x;
}

int ChoiceColumn::getY() const
{
    return y;
}
