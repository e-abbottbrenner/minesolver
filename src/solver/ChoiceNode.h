#ifndef CHOICENODE_H
#define CHOICENODE_H

#include <QEnableSharedFromThis>

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include "PotentialMinefield.h"

class ChoiceColumn;

// this class represents a node in the powerset DAG. It has a state and edges for the DAG and its reverse
class ChoiceNode : public QEnableSharedFromThis<ChoiceNode>
{
public:
    ChoiceNode(PotentialMinefield minefield, int x, int y);

    struct Edge
    {
        QSharedPointer<ChoiceNode> nextNode;
        int cost = 0;
    };

    const PotentialMinefield &getMinefield() const;

    const QList<Edge> &getEdgesForward() const;
    const QList<Edge> &getEdgesReverse() const;

    // adds the successor choices (mine or clear) to the next column
    void addSuccessorsToNextColumn(QSharedPointer<ChoiceColumn> nextColumn);

    void precomputePathsForward(int mineCount);
    void precomputePathsReverse(int mineCount);

    void calculateWaysToBe(int mineCount);

    qint64 getWaysToBeMine() const;

    qint64 getWaysToBeClear() const;

private:
    PotentialMinefield minefield;

    int x = -1;
    int y = -1;

    QList<Edge> edgesForward;
    QList<Edge> edgesReverse;

    QSharedPointer<ChoiceNode> mineForwardEdge;
    QSharedPointer<ChoiceNode> clearForwardEdge;

    QList<qint64> pathsForward;
    QList<qint64> pathsReverse;

    qint64 waysToBeMine = 0;
    qint64 waysToBeClear = 0;

    void tryAddEdge(QSharedPointer<ChoiceColumn> column, const PotentialMinefield& minefield, int cost);
    void linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost);

    int findPathsForward(int mineCount) const;
    int findPathsReverse(int mineCount) const;
    int findPaths(int mineCount, bool forward) const;

    void precomputePaths(int mineCount, bool forward);
};

#endif // CHOICENODE_H
