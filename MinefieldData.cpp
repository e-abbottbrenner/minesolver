#include "MinefieldData.h"

#include "RandomNumbers.h"

MinefieldData::MinefieldData(int numMines, int width, int height, int seed, QObject *parent) :
    QObject(parent), numMines(numMines), width(width), height(height), seed(seed)
{
    populated = false;

    underlyingMinefield = new MineStatus*[width];
    revealedMinefield = new MineStatus*[width];
    for(int i = 0; i < width; ++i)
    {
        underlyingMinefield[i] = new MineStatus[height];
        revealedMinefield[i] = new MineStatus[height];
        for(int j = 0; j < height; ++j)
        {
            underlyingMinefield[i][j] = SpecialStatus::Unknown;
            revealedMinefield[i][j] = SpecialStatus::Unknown;
        }
    }
}

void MinefieldData::populateMinefield(int originX, int originY)
{
    RandomNumbers random(seed);

    // this is slow, but it's guaranteed to work and I like that
    // I also doubt it's slow enough to notice
    int freeCells = width * height - 9;

    for(int mineCount = 0; mineCount < numMines && freeCells > 0; ++mineCount, --freeCells)
    {
        int mineLoc = random.nextRand(freeCells);

        // place the mine at mineLoc in the remaining free cells
        for(int x = 0; x < width; ++x)
        {
            for(int y = 0; y < height; ++y)
            {
                if(underlyingMinefield[x][y] != SpecialStatus::Mine
                        && x > originX + 1 && x < originX - 1
                        && y > originY + 1 && y < originY - 1)
                {// not already a mine or within a cell of the origin
                    if(mineLoc == 0)
                    {
                        underlyingMinefield[x][y] = SpecialStatus::Mine;
                    }

                    --mineLoc;
                }
            }
        }
    }

    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            int mineCount = 0;

            // label the cells that don't have mines with their mine counts
            if(underlyingMinefield[x][y] == SpecialStatus::Unknown)
            {
                for(int i = -1; i <= 1; ++i)
                {
                    for(int j = -1; j <= 1; ++j)
                    {
                        if(x + i >= 0 && x + i < width
                                && y + j >= 0 && y +j < height
                                && underlyingMinefield[x + i][y + j] == SpecialStatus::Mine)
                        {
                            ++mineCount;
                        }
                    }
                }

                underlyingMinefield[x][y] = mineCount;
            }
        }
    }

    populated = true;
}

void MinefieldData::revealCell(int x, int y)
{
    if(!populated)
    {
        populateMinefield(x, y);
    }

    bool clear = underlyingMinefield[x][y] != SpecialStatus::Mine;

    if(clear)
    {
        recursiveReveal(x, y);
    }
    else
    {
        revealAll();

        emit mineHit();
    }
}

void MinefieldData::toggleCellFlag(int x, int y)
{
    if(revealedMinefield[x][y] = SpecialStatus::Unknown)
    {
        revealedMinefield[x][y] = SpecialStatus::GuessMine;
    }
    else if(revealedMinefield[x][y] == SpecialStatus::GuessMine)
    {
        revealedMinefield[x][y] = SpecialStatus::Unknown;
    }
}

void MinefieldData::recursiveReveal(int x, int y)
{
    revealedMinefield[x][y] = underlyingMinefield[x][y];

    emit cellRevealed(x, y);

    if(underlyingMinefield[x][y] == 0)
    {// no nearby mines, do the recursive reveal
        for(int i = -1; i <= 1; ++i)
        {
            for(int j = -1; j <= 1; ++j)
            {
                recursiveReveal(x + i, y + j);
            }
        }
    }
}

void MinefieldData::revealAll()
{
    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            revealedMinefield[x][y] = underlyingMinefield[x][y];
            emit cellRevealed(x, y);
        }
    }
}

MineStatus MinefieldData::getCell(int x, int y) const
{
    return revealedMinefield[x][y];
}

int MinefieldData::getWidth() const
{
    return width;
}

int MinefieldData::getHeight() const
{
    return height;
}

MinefieldData::~MinefieldData()
{
    for(int i = 0; i < width; ++i)
    {
        delete[] underlyingMinefield[i];
        delete[] revealedMinefield[i];
    }

    delete[] underlyingMinefield;
    delete[] revealedMinefield;
}
