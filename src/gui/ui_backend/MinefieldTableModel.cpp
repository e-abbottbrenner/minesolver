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

    setCurrentRecalculationProgress(0);
    setMaxRecalculationProgress(0);

    setRecalculationInProgress(false);

    setBestMineChance(0);

    setCumulativeRiskOfLoss(0);

    setGameWon(false);
    setGameLost(false);

    setFlagsRemaining(minefield->getNumMines());

    autoSolve = false;

    mineChancesCalculationWatcher.clear();
    activeSolver.clear();
    finishedSolver.clear();

    this->minefield = minefield;
    calculateChances();

    connect(minefield.data(), &Minefield::mineHit, this, &MinefieldTableModel::onMineHit);
    connect(minefield.data(), &Minefield::allCountCellsRevealed, this, &MinefieldTableModel::onAllCountCellsRevealed);

    emit newMinefieldStarted();

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

    if(x >= 0 && y >= 0 && x < minefield->getWidth() && y < minefield->getHeight())
    {
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
            return finishedSolver? finishedSolver->getChancesToBeMine().value({x, y}, 0) : 0;
            break;
        case ChoiceColumnCountRole:
            return finishedSolver? finishedSolver->getColumnCounts().value({x, y}, 0) : 0;
        case SolverPathIndexRole:
            return finishedSolver? finishedSolver->getPathIndex({x, y}) : -1;
        default:
            break;
        }
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
    roles[ChoiceColumnCountRole] = "choiceColumnCount";
    roles[SolverPathIndexRole] = "solverPathIndex";

    return roles;
}

bool MinefieldTableModel::isRecalculationInProgress() const
{
    return recalculationInProgress;
}

void MinefieldTableModel::reveal(int row, int col, bool force)
{
    if(finishedSolver)
    {
        // 1 - chance to get past all the cells that were guesswork
        setCumulativeRiskOfLoss(1 - (1 - cumulativeRiskOfLoss) * (1 - finishedSolver->getChancesToBeMine()[{col, row}]));
    }

    auto coordsRevealed = minefield->revealCell(col, row, force);

    emitUpdateSignalForCoords(coordsRevealed);

    calculateChances();
}

void MinefieldTableModel::revealAdjacent(int row, int col)
{
    auto coordsRevealed = minefield->revealAdjacents(col, row);

    emitUpdateSignalForCoords(coordsRevealed);

    calculateChances();
}

void MinefieldTableModel::toggleGuessMine(int row, int col)
{
    QModelIndex toggleIndex = index(row, col);

    minefield->toggleGuessMine(col, row);

    setFlagsRemaining(flagsRemaining + (toggleIndex.data(GuessMineRole).toBool()? -1 : 1));

    emit dataChanged(toggleIndex, toggleIndex);
}

void MinefieldTableModel::revealLowestRiskCells()
{
    if(finishedSolver && !activeSolver && bestMineChance < 1)
    {
        for(const auto &bestCoord : getOptimalCells())
        {
            reveal(bestCoord.second, bestCoord.first, true);
        }
    }
}

bool MinefieldTableModel::getGameLost() const
{
    return gameLost;
}

bool MinefieldTableModel::getGameWon() const
{
    return gameWon;
}

QList<Coordinate> MinefieldTableModel::getOptimalCells() const
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
        return {{QRandomGenerator::global()->bounded(minefield->getWidth()), QRandomGenerator::global()->bounded(minefield->getHeight())}};
    }
}

const QString &MinefieldTableModel::getRecalculationStep() const
{
    return recalculationStep;
}

int MinefieldTableModel::getLogLegalFieldCount() const
{
    // no finished solver, default to the minefield width and height
    return finishedSolver? finishedSolver->getLogLegalFieldCount() : -1;
}

int MinefieldTableModel::roleForName(const QString &roleName) const
{
    auto roleNames = this->roleNames();

    auto roles = roleNames.keys();

    for(int role: roles)
    {
        if(roleNames[role] == roleName)
        {
            return role;
        }
    }

    return -1;
}

double MinefieldTableModel::getBestMineChance() const
{
    return bestMineChance;
}

void MinefieldTableModel::setAutoSolve(bool newAutoSolve)
{
    if(newAutoSolve != autoSolve)
    {
        autoSolve = newAutoSolve;

        if(autoSolve)
        {
            revealLowestRiskCells();
        }
    }
}

double MinefieldTableModel::getCumulativeRiskOfLoss() const
{
    return cumulativeRiskOfLoss;
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
    QSharedPointer<Solver> solver(new Solver(minefield, finishedSolver? finishedSolver->getChancesToBeMine() : QHash<Coordinate, double>{}));

    setActiveSolver(solver);

    QFuture<void> mineChancesFuture = QtConcurrent::run([solver] ()
    {
        solver->computeSolution();
    });

    mineChancesCalculationWatcher = mineChancesCalculationWatcher.create();

    connect(mineChancesCalculationWatcher.data(), &QFutureWatcher<void>::finished, this, &MinefieldTableModel::applyCalculationResults);
    mineChancesCalculationWatcher->setFuture(mineChancesFuture);

    setRecalculationInProgress(true);
}

void MinefieldTableModel::emitUpdateSignalForCoords(QList<Coordinate> coords)
{
    int maxRow = 0;
    int maxCol = 0;
    int minRow = minefield->getHeight();
    int minCol = minefield->getWidth();

    for(Coordinate coord: coords)
    {
        maxRow = std::max(maxRow, coord.second);
        maxCol = std::max(maxCol, coord.first);
        minRow = std::min(minRow, coord.second);
        minCol = std::min(minRow, coord.first);
    }

    emit dataChanged(index(minRow, minCol), index(maxRow, maxCol));
}

void MinefieldTableModel::applyCalculationResults()
{
    finishedSolver = activeSolver;

    auto optimalCells = getOptimalCells();

    if(optimalCells.size() > 0)
    {
        setBestMineChance(finishedSolver->getChancesToBeMine()[optimalCells.constFirst()]);
    }

    emit logLegalFieldCountChanged(getLogLegalFieldCount());

    emitUpdateSignalForCoords(finishedSolver->getChancesToBeMine().keys());

    mineChancesCalculationWatcher.clear();
    activeSolver.clear();
    setRecalculationInProgress(false);

    if(autoSolve)
    {
        QTimer::singleShot(0, this, &MinefieldTableModel::revealLowestRiskCells);
    }
}

void MinefieldTableModel::setRecalculationStep(const QString &newRecalculationStep)
{
    if(recalculationStep != newRecalculationStep)
    {
        recalculationStep = newRecalculationStep;
        emit recalculationStepChanged(newRecalculationStep);
    }
}

void MinefieldTableModel::setCumulativeRiskOfLoss(double risk)
{
    if(risk != cumulativeRiskOfLoss)
    {
        cumulativeRiskOfLoss = risk;

        emit cumulativeRiskOfLossChanged(risk);
    }
}

void MinefieldTableModel::setBestMineChance(double chance)
{
    if(bestMineChance != chance)
    {
        bestMineChance = chance;

        emit bestMineChanceChanged(chance);
    }
}

void MinefieldTableModel::setGameWon(bool won)
{
    if(won != gameWon)
    {
        gameWon = won;
        emit gameWonChanged(won);
    }
}

void MinefieldTableModel::setGameLost(bool lost)
{
    if(lost != gameLost)
    {
        gameLost = lost;
        emit gameLostChanged(lost);
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

void MinefieldTableModel::onMineHit()
{
    setGameLost(true);
}

void MinefieldTableModel::onAllCountCellsRevealed()
{
    setGameWon(true);
}

int MinefieldTableModel::getFlagsRemaining() const
{
    return flagsRemaining;
}

void MinefieldTableModel::setFlagsRemaining(int newFlagsRemaining)
{
    if (flagsRemaining != newFlagsRemaining)
    {
        flagsRemaining = newFlagsRemaining;
        emit flagsRemainingChanged(flagsRemaining);
    }
}
