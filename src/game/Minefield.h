#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "TraversableGrid.h"
#include <QObject>

#include "MineStatus.h"

#include <functional>
#include <QList>

class Minefield : public QObject, public TraversableGrid
{
    Q_OBJECT
public:
    explicit Minefield(int numMines, int width, int height, int seed, QObject *parent = 0);

    void revealAdjacents(int x, int y);
    void revealCell(int x, int y);
    void toggleGuessMine(int x, int y);

    MineStatus getCell(int x, int y) const;
    MineStatus getUnderlyingCell(int x, int y) const;

    int getNumMines() const;

    QByteArray getRevealedMinefield() const;

    ~Minefield();

signals:
    void cellUpdated(int x, int y);
    void mineHit();

private:
    void populateMinefield(int originX, int originY);
    void recursiveReveal(int x, int y);

    void revealAll();

    int numMines = 0;
    int seed = 0;

    bool populated = false;

    QByteArray underlyingMinefield;
    QByteArray revealedMinefield;
};

#endif // MINEFIELD_H
