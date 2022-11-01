#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "TraversableGrid.h"
#include <QObject>

#include "MineStatus.h"

#include <functional>
#include <QList>
#include <QMutex>
#include <QPair>

typedef QPair<int, int> Coordinate;

class Minefield : public QObject, public TraversableGrid
{
    Q_OBJECT
public:
    explicit Minefield(int mineCount, int width, int height, int seed, QObject *parent = 0);

    void ensureMinefieldPopulated(int originX, int originY);

    void revealAdjacents(int x, int y);
    void revealAll();

    void revealCell(int x, int y, bool force = false);
    void toggleGuessMine(int x, int y);

    MineStatus getCell(int x, int y) const;
    MineStatus getUnderlyingCell(int x, int y) const;

    int getMineCount() const;

    QByteArray getRevealedMinefield() const;

    bool isPopulated() const;

    bool areAllCountCellsRevealed() const;
    bool wasMineHit() const;

signals:
    void cellsRevealed(QList<Coordinate> cells);
    void cellToggled(int x, int y);
    void mineHit();
    void allCountCellsRevealed();

private:
    void queueCellRevealed(int x, int y);
    void deliverCellReveals();

    void revealCellPrivate(int x, int y, bool force);
    void recursiveReveal(int x, int y);

    void revealMines();

    void ensureMinefieldPopulatedPrivate(int originX, int originY);

    bool areAllCountCellsRevealedPrivate() const;

    const int MINE_COUNT = 0;
    const int SEED = 0;

    int unrevealedCount = 0;

    bool populated = false;

    bool mineWasHit = false;

    QList<Coordinate> cellsToReveal;

    QByteArray underlyingMinefield;
    QByteArray revealedMinefield;

    mutable QMutex minefieldMutex;
};

#endif // MINEFIELD_H
