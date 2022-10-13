#ifndef CHOICECOLUMN_H
#define CHOICECOLUMN_H

#include <QByteArray>
#include <QHash>
#include <QSharedPointer>

class ChoiceNode;

class ChoiceColumn
{
public:
    ChoiceColumn(int x, int y);

    QSharedPointer<ChoiceNode> getChoiceNode(const QByteArray& minefieldBytes) const;
    void addChoiceNode(QSharedPointer<ChoiceNode> node);

    QList<QSharedPointer<ChoiceNode>> getChoiceNodes() const;

    int getX() const;
    int getY() const;

private:
    QHash<QByteArray, QSharedPointer<ChoiceNode>> choicesInColumn;
    int x = 0;
    int y = 0;
};

#endif // CHOICECOLUMN_H
