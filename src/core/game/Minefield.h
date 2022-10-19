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
    explicit Minefield(int numMines, int width, int height, int seed, QObject *parent = 0);

    void populateMinefield(int originX, int originY);

    QList<Coordinate> revealAdjacents(int x, int y);
    QList<Coordinate> revealCell(int x, int y);
    void toggleGuessMine(int x, int y);

    MineStatus getCell(int x, int y) const;
    MineStatus getUnderlyingCell(int x, int y) const;

    int getNumMines() const;

    QByteArray getRevealedMinefield() const;

    QSharedPointer<Minefield> clone() const;

    ~Minefield();

    bool isPopulated() const;

signals:
    void cellUpdated(int x, int y);
    void mineHit();

private:
    QList<Coordinate> recursiveReveal(int x, int y);

    QList<Coordinate> revealAll();

    int numMines = 0;
    int seed = 0;

    bool populated = false;

    QByteArray underlyingMinefield;
    QByteArray revealedMinefield;
};

#endif // MINEFIELD_H
