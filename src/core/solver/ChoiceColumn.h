#ifndef CHOICECOLUMN_H
#define CHOICECOLUMN_H

#include "SolverFloat.h"

#include <QByteArray>
#include <QFuture>
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

    QFuture<void> precomputePathsForward(int mineCount);
    QFuture<void> precomputePathsBack(int mineCount);

    QFuture<void> calculateWaysToBeMine(int mineCount);

    double getPercentChanceToBeMine() const;
    SolverFloat getWaysToBeMine() const;
    SolverFloat getWaysToBeClear() const;
    
    void setValidMinefieldCount(SolverFloat count);

private:
    static void precomputePathsForwardForNode(const QSharedPointer<ChoiceNode>& choiceNode, int mineCount);
    static void precomputePathsBackForNode(const QSharedPointer<ChoiceNode>& choiceNode, int mineCount);
    static void calculateWaysToBeMineForNode(const QSharedPointer<ChoiceNode>& choiceNode, ChoiceColumn* column, int mineCount);

    QHash<QByteArray, QSharedPointer<ChoiceNode>> choicesInColumn;

    int x = 0;
    int y = 0;

    QMutex waysToBeMutex;

    SolverFloat waysToBeMine = 0;
    SolverFloat validMinefieldCount = 0;
};

#endif // CHOICECOLUMN_H
