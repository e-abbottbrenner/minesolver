#include "MinefieldTableModel.h"

#include "Minefield.h"
#include "Solver.h"

#include <QColor>

MinefieldTableModel::MinefieldTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void MinefieldTableModel::setMinefield(QSharedPointer<Minefield> minefield)
{
    beginResetModel();

    this->minefield = minefield;
    solver = solver.create(minefield);

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
        return solver->getChancesToBeMine()[{x, y}];
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

void MinefieldTableModel::reveal(int row, int col)
{
    // if we want animations we need to do individual data change signals dor revealed indices
    beginResetModel();

    minefield->revealCell(col, row);

    endResetModel();
}

void MinefieldTableModel::revealAdjacent(int row, int col)
{
    // if we want animations we need to do individual data change signals dor revealed indices
    beginResetModel();

    minefield->revealAdjacents(col, row);

    endResetModel();
}

void MinefieldTableModel::toggleGuessMine(int row, int col)
{
    minefield->toggleGuessMine(col, row);

    emit dataChanged(index(row, col), index(row, col));
}