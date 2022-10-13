#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "TraversableGrid.h"

#include <QObject>
#include <functional>

typedef qint8 MineStatus;

namespace SpecialStatus
{
    const MineStatus Mine = -1;
    const MineStatus Unknown = -2;
    const MineStatus GuessMine = -3;
    const MineStatus GuessClear = -4;
}

class Minefield : public QObject, public TraversableGrid
{
    Q_OBJECT
public:
    explicit Minefield(int numMines, int width, int height, int seed, QObject *parent = 0);

    void revealAdjacents(int x, int y);
    void revealCell(int x, int y);
    void toggleCellFlag(int x, int y);

    MineStatus getCell(int x, int y) const;

    QByteArray getRevealedMinefield() const;

    ~Minefield();
    
signals:
    void cellRevealed(int x, int y);
    void mineHit();

private:
    void populateMinefield(int originX, int originY);
    void recursiveReveal(int x, int y);
    void revealAll();

    int map(int x, int y) const;

    int numMines = 0;
    int seed = 0;

    bool populated = false;

    QByteArray underlyingMinefield;
    QByteArray revealedMinefield;
};

#endif // MINEFIELD_H
