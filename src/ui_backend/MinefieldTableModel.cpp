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

    // TODO: icons instead of colors
    QColor decoration;

    switch(mineStatus)
    {
    case SpecialStatus::Mine:
        decoration = Qt::red;
    case SpecialStatus::GuessMine:
        decoration = Qt::yellow;
    default:
        break;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        return mineStatus > 0? QVariant(QString::number(minefield->getCell(x, y))) : QVariant();
        break;
    case Qt::DecorationRole:
        return decoration.isValid()? QVariant(decoration) : QVariant();
        break;
    case ChanceToBeMineRole:
        return solver->getChancesToBeMine()[{x, y}];
        break;
    default:
        break;
    }

    return {};
}

void MinefieldTableModel::reveal(const QModelIndex &index)
{
    // if we want animations we need to do individual data change signals dor revealed indices
    beginResetModel();

    minefield->revealCell(index.row(), index.column());

    endResetModel();
}

void MinefieldTableModel::revealAdjacent(const QModelIndex &index)
{
    // if we want animations we need to do individual data change signals dor revealed indices
    beginResetModel();

    minefield->revealAdjacents(index.row(), index.column());

    endResetModel();
}

void MinefieldTableModel::toggleGuessMine(const QModelIndex &index)
{
    minefield->toggleGuessMine(index.row(), index.column());

    emit dataChanged(index, index, {Qt::DecorationRole});
}
