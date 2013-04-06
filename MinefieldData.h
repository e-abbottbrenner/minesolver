#ifndef MINEFIELDDATA_H
#define MINEFIELDDATA_H

#include <QObject>

typedef int MineStatus;

namespace SpecialStatus
{
    const MineStatus Mine = -1;
    const MineStatus Unknown = -2;
    const MineStatus GuessMine = -3;
    const MineStatus GuessClear = -4;
}

class MinefieldData : public QObject
{
    Q_OBJECT
public:
    explicit MinefieldData(int numMines, int width, int height, int seed, QObject *parent = 0);

    void revealCell(int x, int y);
    void toggleCellFlag(int x, int y);

    MineStatus getCell(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

    ~MinefieldData();
    
signals:
    void cellRevealed(int x, int y);
    void mineHit();

private:
    void populateMinefield(int originX, int originY);
    void recursiveReveal(int x, int y);
    void revealAll();

    int numMines;
    int width;
    int height;
    int seed;

    bool populated;

    MineStatus** underlyingMinefield;
    MineStatus** revealedMinefield;
};

#endif // MINEFIELDDATA_H
