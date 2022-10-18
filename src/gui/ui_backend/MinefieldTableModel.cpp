#include "MinefieldTableModel.h"

#include "Minefield.h"
#include "ProgressProxy.h"
#include "Solver.h"

#include <QColor>
#include <QtConcurrent/QtConcurrent>

MinefieldTableModel::MinefieldTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void MinefieldTableModel::setMinefield(QSharedPointer<Minefield> minefield)
{
    beginResetModel();

    this->minefield = minefield;
    // this is mathematically the initial value
    logLegalFieldCount = minefield->getWidth() * minefield->getHeight();

    endResetModel();
}

int MinefieldTableModel::rowCount(const QModelIndex &parent) const
{
    return minefield->getHeight();
}

int MinefieldTableModel::columnCount(const QModelIndex &parent) const
{
    return minefield->getWidth();
}

QVariant MinefieldTableModel::data(const QModelIndex &index, int role) const
{
    int x = index.column();
    int y = index.row();

    MineStatus mineStatus = minefield->getCell(x, y);

    switch(role)
    {
    case CountRole:
        return mineStatus;
        break;
    case MineRole:
        return SpecialStatus::Mine == mineStatus;
        break;
    case GuessMineRole:
        return SpecialStatus::GuessMine == mineStatus;
    case ChanceToBeMineRole:
        return chancesToBeMine.value({x, y}, 0);
        break;
    default:
        break;
    }

    return {};
}

QHash<int, QByteArray> MinefieldTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[CountRole] = "count";
    roles[GuessMineRole] = "guessMine";
    roles[MineRole] = "isMine";
    roles[ChanceToBeMineRole] = "chanceMine";

    return roles;
}

bool MinefieldTableModel::isRecalculationInProgress() const
{
    return recalculationInProgress;
}

void MinefieldTableModel::reveal(int row, int col)
{
    auto coordsRevealed = minefield->revealCell(col, row);

    calculateChances();

    emitUpdateSignalForCoords(coordsRevealed);
}

void MinefieldTableModel::revealAdjacent(int row, int col)
{
    auto coordsRevealed = minefield->revealAdjacents(col, row);

    calculateChances();

    emitUpdateSignalForCoords(coordsRevealed);
}

void MinefieldTableModel::toggleGuessMine(int row, int col)
{
    minefield->toggleGuessMine(col, row);

    emit dataChanged(index(row, col), index(row, col));
}

const QString &MinefieldTableModel::getRecalculationStep() const
{
    return recalculationStep;
}

int MinefieldTableModel::getLogLegalFieldCount() const
{
    return logLegalFieldCount;
}

int MinefieldTableModel::getCurrentRecalculationProgress() const
{
    return currentRecalculationProgress;
}

void MinefieldTableModel::setCurrentRecalculationProgress(int newCurrentRecalculationProgress)
{
    if(newCurrentRecalculationProgress != currentRecalculationProgress)
    {
        currentRecalculationProgress = newCurrentRecalculationProgress;
        emit currentRecalculationProgressChanged(newCurrentRecalculationProgress);
    }
}

int MinefieldTableModel::getMaxRecalculationProgress() const
{
    return maxRecalculationProgress;
}

void MinefieldTableModel::calculateChances()
{
    QSharedPointer<Solver> solver(new Solver(minefield));

    setActiveSolver(solver);

    QFuture<void> mineChancesFuture = QtConcurrent::run([solver] ()
    {
        solver->computeSolution();
    });

    mineChancesCalculationWatcher = mineChancesCalculationWatcher.create();

    connect(mineChancesCalculationWatcher.data(), &QFutureWatcher<QHash<Coordinate, double>>::finished, this, &MinefieldTableModel::applyCalculationResults);
    mineChancesCalculationWatcher->setFuture(mineChancesFuture);

    setRecalculationInProgress(true);
}

void MinefieldTableModel::emitUpdateSignalForCoords(QList<Coordinate> coords)
{
    for(Coordinate coord: coords)
    {
        QModelIndex coordIndex = index(coord.second, coord.first);
        emit dataChanged(coordIndex, coordIndex);
    }
}

void MinefieldTableModel::applyCalculationResults()
{
    chancesToBeMine = activeSolver->getChancesToBeMine();
    logLegalFieldCount = activeSolver->getLogLegalFieldCount();

    emit logLegalFieldCountChanged(logLegalFieldCount);

    emitUpdateSignalForCoords(chancesToBeMine.keys());

    mineChancesCalculationWatcher.clear();
    activeSolver.clear();
    setRecalculationInProgress(false);
}

void MinefieldTableModel::setRecalculationStep(const QString &newRecalculationStep)
{
    if(recalculationStep != newRecalculationStep)
    {
        recalculationStep = newRecalculationStep;
        emit recalculationStepChanged(newRecalculationStep);
    }
}

void MinefieldTableModel::setMaxRecalculationProgress(int newMaxRecalculationProgress)
{
    if(newMaxRecalculationProgress != maxRecalculationProgress)
    {
        maxRecalculationProgress = newMaxRecalculationProgress;
        emit maxRecalculationProgressChanged(newMaxRecalculationProgress);
    }
}

void MinefieldTableModel::setRecalculationInProgress(bool recalculation)
{
    if(recalculationInProgress != recalculation)
    {
        recalculationInProgress = recalculation;

        emit recalculationInProgressChanged(recalculation);
    }
}

void MinefieldTableModel::setActiveSolver(QSharedPointer<Solver> solver)
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

    setCurrentRecalculationProgress(0);
    setMaxRecalculationProgress(0);

    activeSolver = solver;

    // connect the new solver
    recalcProgressConnections << connect(activeSolver->getProgress().data(), &ProgressProxy::progressMade, this, &MinefieldTableModel::setCurrentRecalculationProgress);
    recalcProgressConnections << connect(activeSolver->getProgress().data(), &ProgressProxy::progressMaximum, this, &MinefieldTableModel::setMaxRecalculationProgress);
    recalcProgressConnections << connect(activeSolver->getProgress().data(), &ProgressProxy::progressStep, this, &MinefieldTableModel::setRecalculationStep);
}
