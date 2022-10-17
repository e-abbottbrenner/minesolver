#ifndef MINEFIELDTABLEMODEL_H
#define MINEFIELDTABLEMODEL_H

#include <QAbstractTableModel>
#include <QFutureWatcher>
#include <QPair>
#include <QSharedPointer>
#include <QtQml/QtQml>

class Minefield;
class Solver;

typedef QPair<int, int> Coordinate;

class MinefieldTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("QML Cannot invoke constructor")

public:
    enum DataRoles
    {
        CountRole = Qt::UserRole,
        GuessMineRole,
        MineRole,
        ChanceToBeMineRole
    };

    explicit MinefieldTableModel(QObject *parent = nullptr);

    void setMinefield(QSharedPointer<Minefield> minefield);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

public slots:
    void reveal(int row, int col);
    void revealAdjacent(int row, int col);
    void toggleGuessMine(int row, int col);

private:
    QSharedPointer<Minefield> minefield;

    QHash<Coordinate, double> chancesToBeMine;

    QSharedPointer<QFutureWatcher<QHash<Coordinate, double>>> mineChancesCalculationWatcher;

    void calculateChances();

    void emitUpdateSignalForCoords(QList<Coordinate> coords);

    void applyCalculationResults();
};

Q_DECLARE_METATYPE(MinefieldTableModel)

#endif // MINEFIELDTABLEMODEL_H
