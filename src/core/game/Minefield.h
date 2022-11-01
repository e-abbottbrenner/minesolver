#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "TraversableGrid.h"
#include <QObject>

#include "MineStatus.h"

#include <functional>
#include <QList>
#include <QPair>

typedef QPair<int, int> Coordinate;

class Minefield : public QObject, public TraversableGrid
{
    Q_OBJECT
public:
    explicit Minefield(int mineCount, int width, int height, int seed, QObject *parent = 0);

    void populateMinefield(int originX, int originY);

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
    void cellUpdated(int x, int y);
    void mineHit();
    void allCountCellsRevealed();

private:
    QList<Coordinate> recursiveReveal(int x, int y);

    void revealMines();

    int mineCount = 0;
    int seed = 0;

    int unrevealedCount = 0;

    bool populated = false;

    bool mineWasHit = false;

    QByteArray underlyingMinefield;
    QByteArray revealedMinefield;
};

#endif // MINEFIELD_H
