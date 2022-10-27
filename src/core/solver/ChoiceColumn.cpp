#include "ChoiceColumn.h"

#include "ChoiceNode.h"
#include "SolverMinefield.h"

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
    auto precomputePaths = [mineCount] (const QSharedPointer<ChoiceNode>& choiceNode)
    {
        choiceNode->precomputePathsForward(mineCount);
    };

    // TODO: return plain map so there's a cancellable future
    QtConcurrent::blockingMap(&(*columnCalcThreadPool), getChoiceNodes(), precomputePaths);
}

void ChoiceColumn::precomputePathsBack(int mineCount)
{
    auto precomputePaths = [mineCount] (const QSharedPointer<ChoiceNode>& choiceNode)
    {
        choiceNode->precomputePathsBack(mineCount);
    };

    // TODO: return plain map so there's a cancellable future
    QtConcurrent::blockingMap(&(*columnCalcThreadPool), getChoiceNodes(), precomputePaths);
}

void ChoiceColumn::calculateWaysToBe(int mineCount)
{
    waysToBeMine = 0;
    waysToBeClear = 0;

    auto calculateWaysToBe = [&] (const QSharedPointer<ChoiceNode>& choiceNode)
    {
        choiceNode->calculateWaysToBe(mineCount);

        QMutexLocker locker(&waysToBeMutex);

        waysToBeMine += choiceNode->getWaysToBeMine();
        waysToBeClear += choiceNode->getWaysToBeClear();
    };

    // TODO: return plain map so there's a cancellable future
    QtConcurrent::blockingMap(&(*columnCalcThreadPool), getChoiceNodes(), calculateWaysToBe);
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
