#include "ChoiceColumn.h"

#include "ChoiceNode.h"
#include "SolverMinefield.h"

#include <QFutureSynchronizer>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadPool>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

Q_GLOBAL_STATIC(QThreadPool, columnCalcThreadPool);


ChoiceColumn::ChoiceColumn(int x, int y)
    : x(x), y(y)
{
}

QSharedPointer<ChoiceNode> ChoiceColumn::getOrCreateChoiceNode(const SolverMinefield &minefield)
{
    // we use the state as a key to get the choice node that corresponds to that state
    // technically if width could change we'd need the width factored in, but it can't so we don't
    // this may need optimization since it doubles the memory footprint
    auto node = choicesInColumn.value(minefield.getMinefieldBytes(), {});

    if(node.isNull())
    {
        node = node.create(minefield, x, y);
        addChoiceNode(node);
    }

    return node;
}

void ChoiceColumn::addChoiceNode(QSharedPointer<ChoiceNode> node)
{
    choicesInColumn.insert(node->getMinefield().getMinefieldBytes(), node);
}

QList<QSharedPointer<ChoiceNode> > ChoiceColumn::getChoiceNodes() const
{
    return choicesInColumn.values();
}

QList<QList<QSharedPointer<ChoiceNode>>> ChoiceColumn::getChoiceNodesSliced(int slices) const
{
    auto choiceNodes = getChoiceNodes();

    QList<QList<QSharedPointer<ChoiceNode>>> choiceNodesSliced;

    int sliceSize = choiceNodes.size() / slices;

    for(int i = 0; i < slices; ++i)
    {
        int pos = i * sliceSize;

        choiceNodesSliced.append(choiceNodes.sliced(pos, sliceSize));
    }

    int remainder = choiceNodes.size() % slices;

    for(int i = 1; i <= remainder; ++i)
    {
        // spread out the remainder over the slices
        // note this cannot go out of bounds because remainder is at most slices - 1 and the size of choiceNodesSiced is slices
        choiceNodesSliced[i].append(choiceNodes[choiceNodes.size() - i]);
    }

    return choiceNodesSliced;
}

int ChoiceColumn::getX() const
{
    return x;
}

int ChoiceColumn::getY() const
{
    return y;
}

void ChoiceColumn::precomputePathsForward(int mineCount)
{
    auto choiceNodesSliced = getChoiceNodesSliced(columnCalcThreadPool->maxThreadCount());

    QFutureSynchronizer<void> threadSyncer;

    auto precomputePaths = [&] (const QList<QSharedPointer<ChoiceNode>>& choiceNodes)
    {
        for(auto choiceNode: choiceNodes)
        {
            choiceNode->precomputePathsForward(mineCount);
        }
    };

    for(auto choiceNodeList : choiceNodesSliced)
    {
        threadSyncer.addFuture(QtConcurrent::run(&(*columnCalcThreadPool), precomputePaths, choiceNodeList));
    }

    threadSyncer.waitForFinished();
}

void ChoiceColumn::precomputePathsBack(int mineCount)
{
    auto choiceNodesSliced = getChoiceNodesSliced(columnCalcThreadPool->maxThreadCount());

    QFutureSynchronizer<void> threadSyncer;

    auto precomputePaths = [&] (const QList<QSharedPointer<ChoiceNode>>& choiceNodes)
    {
        for(auto choiceNode: choiceNodes)
        {
            choiceNode->precomputePathsBack(mineCount);
        }
    };

    for(auto choiceNodeList : choiceNodesSliced)
    {
        threadSyncer.addFuture(QtConcurrent::run(&(*columnCalcThreadPool), precomputePaths, choiceNodeList));
    }

    threadSyncer.waitForFinished();
}

void ChoiceColumn::calculateWaysToBe(int mineCount)
{
    QMutex waysToBeMutex;

    waysToBeMine = 0;
    waysToBeClear = 0;

    auto choiceNodesSliced = getChoiceNodesSliced(columnCalcThreadPool->maxThreadCount());

    QFutureSynchronizer<void> threadSyncer;

    auto calculateWaysToBe = [&] (const QList<QSharedPointer<ChoiceNode>>& choiceNodes)
    {
        for(auto choiceNode: choiceNodes)
        {
            choiceNode->calculateWaysToBe(mineCount);

            QMutexLocker locker(&waysToBeMutex);

            waysToBeMine += choiceNode->getWaysToBeMine();
            waysToBeClear += choiceNode->getWaysToBeClear();
        }
    };

    for(auto choiceNodeList : choiceNodesSliced)
    {
        threadSyncer.addFuture(QtConcurrent::run(&(*columnCalcThreadPool), calculateWaysToBe, choiceNodeList));
    }

    threadSyncer.waitForFinished();
}

double ChoiceColumn::getPercentChanceToBeMine() const
{
    // cast in case we rework the type again to be larger than a normal double
    return static_cast<double>(waysToBeMine / (waysToBeMine + waysToBeClear));
}

SolverFloat ChoiceColumn::getWaysToBeMine() const
{
    return waysToBeMine;
}

SolverFloat ChoiceColumn::getWaysToBeClear() const
{
    return waysToBeClear;
}
