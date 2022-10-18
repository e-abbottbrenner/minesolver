#ifndef CHOICENODE_H
#define CHOICENODE_H

#include <QEnableSharedFromThis>

#include <QHash>
#include <QList>
#include <QSharedPointer>

#include <boost/multiprecision/cpp_int.hpp>

#include "PotentialMinefield.h"

class ChoiceColumn;

using boost::multiprecision::uint512_t;

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

    uint512_t getWaysToBeMine() const;

    uint512_t getWaysToBeClear() const;

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

    QList<uint512_t> pathsForward;
    QList<uint512_t> pathsBack;

    uint512_t waysToBeMine = 0;
    uint512_t waysToBeClear = 0;

    bool endpoint = false;

    void tryAddEdge(QSharedPointer<ChoiceColumn> column, const PotentialMinefield& minefield, int cost);
    void linkTarget(QSharedPointer<ChoiceNode> edgeTarget, int cost);

    uint512_t findPathsForward(int mineCount) const;
    uint512_t findPathsBack(int mineCount) const;
    uint512_t findPaths(int mineCount, bool forward) const;

    void precomputePaths(int mineCount, bool forward);
};

#endif // CHOICENODE_H
