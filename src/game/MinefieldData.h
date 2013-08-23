#ifndef MINEFIELDDATA_H
#define MINEFIELDDATA_H

#include <QObject>
#include <functional>

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

    void revealAdjacents(int x, int y);
    void revealCell(int x, int y);
    void toggleCellFlag(int x, int y);

    bool checkBounds(int x, int y) const;

    MineStatus getCell(int x, int y) const;

    MineStatus** getRevealedMinefield() const;

    // no const on these as we might want to make changes in the passed func
    void traverseAdacentCells(int x, int y, std::function<void (int, int)> func);
    void traverseCells(std::function<void (int, int)> func);

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
