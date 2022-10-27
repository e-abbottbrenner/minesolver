#ifndef CHOICECOLUMN_H
#define CHOICECOLUMN_H

#include "SolverFloat.h"

#include <QByteArray>
#include <QHash>
#include <QMutex>
#include <QSharedPointer>

class ChoiceNode;
class SolverMinefield;

class ChoiceColumn
{
public:
    ChoiceColumn(int x, int y);

    QSharedPointer<ChoiceNode> getOrCreateChoiceNode(const SolverMinefield& minefield);
    void addChoiceNode(QSharedPointer<ChoiceNode> node);

    QList<QSharedPointer<ChoiceNode>> getChoiceNodes() const;

    int getX() const;
    int getY() const;

    void precomputePathsForward(int mineCount);
    void precomputePathsBack(int mineCount);

    void calculateWaysToBe(int mineCount);

    double getPercentChanceToBeMine() const;
    SolverFloat getWaysToBeMine() const;
    SolverFloat getWaysToBeClear() const;

private:
    QHash<QByteArray, QSharedPointer<ChoiceNode>> choicesInColumn;

    int x = 0;
    int y = 0;

    QMutex waysToBeMutex;

    SolverFloat waysToBeMine = 0;
    SolverFloat waysToBeClear = 0;
};

#endif // CHOICECOLUMN_H
