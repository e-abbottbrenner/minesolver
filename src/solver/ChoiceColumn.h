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

    void calculateWaysToBe(int mineCount);

    double getPercentChanceToBeMine() const;

    qint64 getWaysToBeMine() const;

    qint64 getWaysToBeClear() const;

private:
    QHash<QByteArray, QSharedPointer<ChoiceNode>> choicesInColumn;

    int x = 0;
    int y = 0;

    qint64 waysToBeMine = 0;
    qint64 waysToBeClear = 0;
};

#endif // CHOICECOLUMN_H
