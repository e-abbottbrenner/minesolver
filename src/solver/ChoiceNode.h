#ifndef CHOICENODE_H
#define CHOICENODE_H

#include <QEnableSharedFromThis>

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
    void addSuccessorsToColumn(QSharedPointer<ChoiceColumn> column);

private:
    PotentialMinefield minefield;

    int x = 0;
    int y = 0;

    QList<Edge> edgesForward;
    QList<Edge> edgesReverse;

    void tryAddEdge(QSharedPointer<ChoiceColumn> column, const PotentialMinefield& minefield, int cost);
    void linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost);
};

#endif // CHOICENODE_H
