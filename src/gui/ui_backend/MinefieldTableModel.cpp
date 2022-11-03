#include "MinefieldTableModel.h"

#include "AutoPlayer.h"
#include "Minefield.h"
#include "Solver.h"

#include <QColor>

MinefieldTableModel::MinefieldTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

MinefieldTableModel::~MinefieldTableModel()
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

    if(autoPlayer)
    {
        autoPlayer->disconnect();
        // delete later, we'll also want to auto delete it with the thread
        autoPlayer->deleteLater();
    }

    autoPlayer = new AutoPlayer(minefield);

    connect(autoPlayer, &AutoPlayer::calculationStarted, this, &MinefieldTableModel::onCalculationStarted, Qt::QueuedConnection);
    connect(autoPlayer, &AutoPlayer::calculationComplete, this, &MinefieldTableModel::applyCalculationResults, Qt::QueuedConnection);

    connect(autoPlayer, &AutoPlayer::riskedLoss, this, &MinefieldTableModel::updateRiskOfLoss, Qt::QueuedConnection);

    connect(autoPlayer, &AutoPlayer::maxProgressChanged, this, &MinefieldTableModel::setMaxRecalculationProgress, Qt::QueuedConnection);
    connect(autoPlayer, &AutoPlayer::currentProgressChanged, this, &MinefieldTableModel::setCurrentRecalculationProgress, Qt::QueuedConnection);
    connect(autoPlayer, &AutoPlayer::progressStepChanged, this, &MinefieldTableModel::setRecalculationStep, Qt::QueuedConnection);

    setFlagsRemaining(minefield->getMineCount());

    recalcPending = true;

    finishedSolver.clear();

    this->minefield = minefield;

    autoPlayer->queueCalculate();

    connect(minefield.data(), &Minefield::mineHit, this, &MinefieldTableModel::onMineHit, Qt::QueuedConnection);
    connect(minefield.data(), &Minefield::allCountCellsRevealed, this, &MinefieldTableModel::onAllCountCellsRevealed, Qt::QueuedConnection);
    connect(minefield.data(), &Minefield::cellToggled, this, &MinefieldTableModel::onCellToggled, Qt::QueuedConnection);
    connect(minefield.data(), &Minefield::cellsRevealed, this, &MinefieldTableModel::onCellsRevealed, Qt::QueuedConnection);

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
        case UnexplodedMineRole:
            return SpecialStatus::UnexplodedMine == mineStatus;
            break;
        case ExplodedMineRole:
            return SpecialStatus::ExplodedMine == mineStatus;
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
    roles[UnexplodedMineRole] = "isUnexplodedMine";
    roles[ExplodedMineRole] = "isExplodedMine";
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
        updateRiskOfLoss(finishedSolver->getChancesToBeMine()[{col, row}]);
    }

    minefield->revealCell(col, row, force);

    if(autoPlayer)
    {// need to trigger calculations with the updated board
        autoPlayer->queueCalculate();
    }
}

void MinefieldTableModel::revealAdjacent(int row, int col)
{
    minefield->revealAdjacents(col, row);

    if(autoPlayer)
    {// need to trigger calculations with the updated board
        autoPlayer->queueCalculate();
    }
}

void MinefieldTableModel::toggleGuessMine(int row, int col)
{
    QModelIndex toggleIndex = index(row, col);

    minefield->toggleGuessMine(col, row);

    setFlagsRemaining(flagsRemaining + (toggleIndex.data(GuessMineRole).toBool()? -1 : 1));

    prepareDataChanged(row, col, row, col);
}

void MinefieldTableModel::revealLowestRiskCells()
{
    if(autoPlayer)
    {
        autoPlayer->queueStep();
    }
}

void MinefieldTableModel::flagGuaranteedMines()
{
    auto chances = finishedSolver->getChancesToBeMine();
    auto coords = chances.keys();

    for(auto coord: coords)
    {
        if(minefield->getCell(coord.first, coord.second) == SpecialStatus::Unknown && chances[coord] == 1)
        {
            toggleGuessMine(coord.second, coord.first);
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

QModelIndex MinefieldTableModel::nullIndex() const
{
    return QModelIndex();
}

double MinefieldTableModel::getBestMineChance() const
{
    return bestMineChance;
}

void MinefieldTableModel::setAutoSolve(bool newAutoSolve)
{
    autoPlayer->setAutoSolve(true);
}

bool MinefieldTableModel::getAutoSolve() const
{
    return autoPlayer->getAutoSolve();
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

void MinefieldTableModel::onCalculationStarted()
{
    setCurrentRecalculationProgress(0);
    setMaxRecalculationProgress(0);
    setRecalculationInProgress(true);
}

void MinefieldTableModel::emitUpdateSignalForCoords(QList<Coordinate> coords)
{
    int minRow = minefield->getHeight();
    int minCol = minefield->getWidth();
    int maxRow = 0;
    int maxCol = 0;

    for(Coordinate coord: coords)
    {
        minRow = std::min(minRow, coord.second);
        minCol = std::min(minRow, coord.first);
        maxRow = std::max(maxRow, coord.second);
        maxCol = std::max(maxCol, coord.first);
    }

    if(coords.size() > 0)
    {
        prepareDataChanged(minRow, minCol, maxRow, maxCol);
    }
}

void MinefieldTableModel::prepareDataChanged(int minRow, int minCol, int maxRow, int maxCol)
{
    dataChangedMinRow = std::min(minRow, dataChangedMinRow);
    dataChangedMinCol = std::min(minCol, dataChangedMinCol);
    dataChangedMaxRow = std::max(maxRow, dataChangedMaxRow);
    dataChangedMaxCol = std::max(maxCol, dataChangedMaxCol);

    if(!dataChangedPending)
    {
        dataChangedPending = true;

        QTimer::singleShot(0, this, &MinefieldTableModel::deliverDataChanged);
    }
}

void MinefieldTableModel::deliverDataChanged()
{
    if(dataChangedPending)
    {
        emit dataChanged(index(dataChangedMinRow, dataChangedMinCol), index(dataChangedMaxRow, dataChangedMaxCol));

        dataChangedMinRow = dataChangedMinCol = std::numeric_limits<int>::max();
        dataChangedMaxRow = dataChangedMaxCol = 0;

        dataChangedPending = false;
    }
}

void MinefieldTableModel::applyCalculationResults(QSharedPointer<Solver> solver)
{
    finishedSolver = solver;

    setBestMineChance(autoPlayer->getBestMineChance());

    emit logLegalFieldCountChanged(getLogLegalFieldCount());

    emitUpdateSignalForCoords(finishedSolver->getChancesToBeMine().keys());

    setRecalculationInProgress(false);

    if(getAutoSolve())
    {
        flagGuaranteedMines();
    }
}

void MinefieldTableModel::updateRiskOfLoss(double additionalRisk)
{
    setCumulativeRiskOfLoss(1 - (1 - cumulativeRiskOfLoss) * (1 - additionalRisk));
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

        if(won)
        {
            minefield->revealAll();
        }

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

void MinefieldTableModel::onMineHit()
{
    setGameLost(true);
}

void MinefieldTableModel::onAllCountCellsRevealed()
{
    setGameWon(true);
}

void MinefieldTableModel::onCellToggled(int x, int y)
{
    prepareDataChanged(y, x, y, x);
}

void MinefieldTableModel::onCellsRevealed(QList<Coordinate> revealed)
{
    for(Coordinate coord: revealed)
    {
        prepareDataChanged(coord.second, coord.first, coord.second, coord.first);
    }
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
