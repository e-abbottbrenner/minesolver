#ifndef CHOICENODE_H
#define CHOICENODE_H

#include <QEnableSharedFromThis>

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include <boost/multiprecision/cpp_int.hpp>

#include "PotentialMinefield.h"

class ChoiceColumn;

using boost::multiprecision::cpp_int;

// this class represents a node in the powerset DAG. It has a state and edges for the DAG and its reverse
class ChoiceNode : public QEnableSharedFromThis<ChoiceNode>
{
public:
    ChoiceNode(PotentialMinefield minefield, int x, int y);

    struct Edge
    {
        // this needs to be a weak pointer because we store back edges and shared pointers with looping references equal memory leaks
        QWeakPointer<ChoiceNode> nextNode;
        int cost = 0;
    };

    const PotentialMinefield &getMinefield() const;

    const QList<Edge> &getEdgesForward() const;
    const QList<Edge> &getEdgesBack() const;

    // adds the successor choices (mine or clear) to the next column
    void addSuccessorsToNextColumn(QSharedPointer<ChoiceColumn> nextColumn);

    void precomputePathsForward(int mineCount);
    void precomputePathsBack(int mineCount);

    void calculateWaysToBe(int mineCount);

    cpp_int getWaysToBeMine() const;

    cpp_int getWaysToBeClear() const;

    bool isEndpoint() const;
    void setEndpoint(bool newEndpoint);

private:
    PotentialMinefield minefield;

    int x = -1;
    int y = -1;

    QList<Edge> edgesForward;
    QList<Edge> edgesBack;

    QWeakPointer<ChoiceNode> mineForwardEdge;
    QWeakPointer<ChoiceNode> clearForwardEdge;

    QList<cpp_int> pathsForward;
    QList<cpp_int> pathsBack;

    cpp_int waysToBeMine = 0;
    cpp_int waysToBeClear = 0;

    bool endpoint = false;

    void tryAddEdge(QSharedPointer<ChoiceColumn> column, const PotentialMinefield& minefield, int cost);
    void linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost);

    cpp_int findPathsForward(int mineCount) const;
    cpp_int findPathsBack(int mineCount) const;
    cpp_int findPaths(int mineCount, bool forward) const;

    void precomputePaths(int mineCount, bool forward);
};

#endif // CHOICENODE_H