#include "AutoPlayer.h"

#include "Minefield.h"
#include "Solver.h"

#include <QtConcurrent/QtConcurrent>
#include <QMutexLocker>
#include <QTimer>

#include "ProgressProxy.h"

AutoPlayer::AutoPlayer(QSharedPointer<Minefield> minefield)
    : minefield(minefield)
{
    connect(minefield.data(), &Minefield::cellUpdated, this, &AutoPlayer::queueCalculate, Qt::QueuedConnection);
}

void AutoPlayer::queueStep()
{
    QMutexLocker locker(&queueMutex);

    QTimer::singleShot(0, this, &AutoPlayer::step);
}

void AutoPlayer::queueCalculate()
{
    QMutexLocker locker(&queueMutex);

    calculationPending = true;

    emit calculationStarted();

    QTimer::singleShot(0, this, &AutoPlayer::calculate);
}

void AutoPlayer::setAutoSolve(bool newAutoSolve)
{
    QMutexLocker locker(&writeMutex);

    if(newAutoSolve != autoSolve)
    {
        autoSolve = newAutoSolve;

        if(autoSolve)
        {
            if(finishedSolver)
            {
                queueStep();
            }
            else if(!activeSolver)
            {
                queueCalculate();
            }
        }
    }
}

bool AutoPlayer::getAutoSolve() const
{
    return autoSolve;
}

double AutoPlayer::getBestMineChance() const
{
    return bestMineChance;
}

void AutoPlayer::step()
{
    QMutexLocker locker(&writeMutex);

    if(finishedSolver && !activeSolver && bestMineChance < 1 && !minefield->wasMineHit())
    {
        for(const auto &bestCoord : getOptimalCells())
        {
            reveal(bestCoord.first, bestCoord.second);
        }
    }
}

void AutoPlayer::calculate()
{
    QMutexLocker locker(&writeMutex);

    if(calculationPending)
    {
        calculationPending = false;

        QSharedPointer<Solver> solver(new Solver(minefield, finishedSolver? finishedSolver->getChancesToBeMine() : QHash<Coordinate, double>{}));

        QFuture<void> mineChancesFuture = QtConcurrent::run([solver] ()
        {
            solver->computeSolution();
        });

        setActiveSolver(solver);

        mineChancesCalculationWatcher = mineChancesCalculationWatcher.create();

        connect(mineChancesCalculationWatcher.data(), &QFutureWatcher<void>::finished, this, &AutoPlayer::onCalculationsComplete);
        mineChancesCalculationWatcher->setFuture(mineChancesFuture);
    }
}

void AutoPlayer::onCalculationsComplete()
{
    QMutexLocker locker(&writeMutex);

    finishedSolver = activeSolver;

    mineChancesCalculationWatcher.clear();
    activeSolver.clear();

    auto optimalCells = getOptimalCells();

    if(optimalCells.size() > 0)
    {
        bestMineChance = finishedSolver->getChancesToBeMine()[optimalCells.constFirst()];
    }

    if(autoSolve)
    {
        queueStep();
    }

    emit calculationComplete(finishedSolver);
}

void AutoPlayer::setActiveSolver(QSharedPointer<Solver> solver)
{
    if(activeSolver)
    {// cancel in progress solver so they don't stack up with lots of hard calculations
        activeSolver->cancel();
    }

    // disconnect the old solver
    for(const auto &connection: recalcProgressConnections)
    {
        disconnect(connection);
    }

    activeSolver = solver;

    // connect the new solver
    recalcProgressConnections << connect(activeSolver->getProgress().data(), &ProgressProxy::progressMade, this, &AutoPlayer::currentProgressChanged);
    recalcProgressConnections << connect(activeSolver->getProgress().data(), &ProgressProxy::progressMaximum, this, &AutoPlayer::maxProgressChanged);
    recalcProgressConnections << connect(activeSolver->getProgress().data(), &ProgressProxy::progressStep, this, &AutoPlayer::progressStepChanged);
}

QList<Coordinate> AutoPlayer::getOptimalCells() const
{
    if(minefield->isPopulated())
    {
        auto chances = finishedSolver->getChancesToBeMine();

        // order is random, that's basically what we want though
        auto coords = chances.keys();

        QList<Coordinate> bestCoords;
        double bestChance = 1;

        for(const Coordinate &coord: coords)
        {
            if(bestChance > chances[coord])
            {
                bestChance = chances[coord];
                bestCoords = {coord};
            }
            else if(bestChance == chances[coord])
            {
                bestCoords.append(coord);
            }
        }

        if(bestChance > 0 && bestCoords.size() > 0)
        {// pick a random one with the lowest odds if we didn't get to 0
            return {bestCoords[QRandomGenerator::global()->bounded(bestCoords.size())]};
        }

        return bestCoords;
    }
    else
    {
        return {{minefield->getWidth() / 2, minefield->getHeight() / 2}};
    }
}

void AutoPlayer::reveal(int x, int y)
{
    emit riskedLoss(finishedSolver->getChancesToBeMine()[{x, y}]);

    minefield->revealCell(x, y, true);
}
