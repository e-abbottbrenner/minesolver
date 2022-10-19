#ifndef CHOICENODE_H
#define CHOICENODE_H

#include <QEnableSharedFromThis>

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include "SolverFloat.h"
#include "SolverMinefield.h"

class ChoiceColumn;

// this class represents a node in the powerset DAG. It has a state and edges for the DAG and its reverse
class ChoiceNode : public QEnableSharedFromThis<ChoiceNode>
{
public:
    ChoiceNode(const SolverMinefield& minefield, int x, int y);

    struct Edge
    {
        // this needs to be a weak pointer because we store back edges and shared pointers with looping references equal memory leaks
        QWeakPointer<ChoiceNode> nextNode;
        int cost = 0;
    };

    const SolverMinefield &getMinefield() const;

    const QList<Edge> &getEdgesForward() const;
    const QList<Edge> &getEdgesBack() const;

    // adds the successor choices (mine or clear) to the next column
    void addSuccessorsToNextColumn(QSharedPointer<ChoiceColumn> nextColumn);

    void precomputePathsForward(int mineCount);
    void precomputePathsBack(int mineCount);

    void calculateWaysToBe(int mineCount);

    SolverFloat getWaysToBeMine() const;

    SolverFloat getWaysToBeClear() const;

    bool isEndpoint() const;
    void setEndpoint(bool newEndpoint);

private:
    SolverMinefield minefield;

    int x = -1;
    int y = -1;

    QList<Edge> edgesForward;
    QList<Edge> edgesBack;

    QWeakPointer<ChoiceNode> mineForwardEdge;
    QWeakPointer<ChoiceNode> clearForwardEdge;

    QList<SolverFloat> pathsForward;
    QList<SolverFloat> pathsBack;

    SolverFloat waysToBeMine = 0;
    SolverFloat waysToBeClear = 0;

    bool endpoint = false;

    void tryAddEdge(QSharedPointer<ChoiceColumn> column, const SolverMinefield& minefield, int cost);
    void linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost);

    SolverFloat findPathsForward(int mineCount) const;
    SolverFloat findPathsBack(int mineCount) const;
    SolverFloat findPaths(int mineCount, bool forward) const;

    void precomputePaths(int mineCount, bool forward);
};

#endif // CHOICENODE_H
