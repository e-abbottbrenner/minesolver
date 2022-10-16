#ifndef MINEFIELDTABLEMODEL_H
#define MINEFIELDTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QtQml/QtQml>

class Minefield;
class Solver;

class MinefieldTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("QML Cannot invoke constructor")

public:
    enum DataRoles
    {
        ChanceToBeMineRole = Qt::UserRole
    };

    explicit MinefieldTableModel(QObject *parent = nullptr);

    void setMinefield(QSharedPointer<Minefield> minefield);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void reveal(const QModelIndex& index);
    void revealAdjacent(const QModelIndex& index);
    void toggleGuessMine(const QModelIndex& index);

private:
    QSharedPointer<Minefield> minefield;
    QSharedPointer<Solver> solver;
};

#endif // MINEFIELDTABLEMODEL_H
