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
    int bannedCells = 0;

    // count the number of cells within bounds that aren't allowed to be mines
    traverseAdacentCells(originX, originY, [&] (int, int) {++bannedCells;});

    int freeCells = width * height - bannedCells;

    for(int mineCount = 0; mineCount < numMines && freeCells > 0; ++mineCount, --freeCells)
    {
        int mineLoc = random.nextRand(freeCells);

        auto placeMines = [&] (int x, int y)
        {
            if(underlyingMinefield[x][y] != SpecialStatus::Mine
                    && (x > originX + 1 || x < originX - 1
                        || y > originY + 1 || y < originY - 1))
            {// not already a mine or within a cell of the origin
                if(mineLoc == 0)
                {
                    underlyingMinefield[x][y] = SpecialStatus::Mine;
                }

                --mineLoc;
            }
        };

        // place the mine at mineLoc in the remaining free cells
        traverseCells(placeMines);
    }

    // declare this out here so that both can capture it
    int mineCount = 0;

    auto countMine = [&] (int x, int y)
    {
        // increase count if the cell atx, y is a mine
        if(underlyingMinefield[x][y] == SpecialStatus::Mine)
        {
            ++mineCount;
        }
    };

    auto labelCells = [&] (int x, int y)
    {
        mineCount = 0;

        // label the cells that don't have mines with their mine counts
        if(underlyingMinefield[x][y] != SpecialStatus::Mine)
        {
            traverseAdacentCells(x, y, countMine);

            underlyingMinefield[x][y] = mineCount;
        }
    };

    traverseCells(labelCells);

    populated = true;
}

bool MinefieldData::checkBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

void MinefieldData::revealAdjacents(int x, int y)
{
    int guessCount = 0;

    auto countGuess = [&] (int x, int y)
    {
        if(revealedMinefield[x][y] == SpecialStatus::GuessMine)
        {
            ++guessCount;
        }
    };

    traverseAdacentCells(x, y, countGuess);

    if(guessCount == revealedMinefield[x][y])
    {
        auto reveal = [&] (int x, int y)
        {
            if(revealedMinefield[x][y] != SpecialStatus::GuessMine)
            {
                revealCell(x, y);
            }
        };

        traverseAdacentCells(x, y, reveal);
    }
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
    if(revealedMinefield[x][y] == SpecialStatus::Unknown)
    {
        revealedMinefield[x][y] = SpecialStatus::GuessMine;
    }
    else if(revealedMinefield[x][y] == SpecialStatus::GuessMine)
    {
        revealedMinefield[x][y] = SpecialStatus::Unknown;
    }

    emit cellRevealed(x, y);
}

void MinefieldData::recursiveReveal(int x, int y)
{
    if(underlyingMinefield[x][y] != revealedMinefield[x][y])
    {
        revealedMinefield[x][y] = underlyingMinefield[x][y];

        emit cellRevealed(x, y);

        if(underlyingMinefield[x][y] == 0)
        {// no nearby mines, do the recursive reveal
            traverseAdacentCells(x, y, [this] (int x, int y) {recursiveReveal(x, y);});
        }
    }
}

void MinefieldData::revealAll()
{
    auto reveal = [&] (int x, int y)
    {
        revealedMinefield[x][y] = underlyingMinefield[x][y];
        emit cellRevealed(x, y);
    };

    traverseCells(reveal);
}

MineStatus MinefieldData::getCell(int x, int y) const
{
    return revealedMinefield[x][y];
}

MineStatus** MinefieldData::getRevealedMinefield() const
{
    return revealedMinefield;
}

void MinefieldData::traverseAdacentCells(int x, int y, std::function<void (int, int)> func)
{
    for(int i = -1; i <= 1; ++i)
    {
        for(int j = -1; j <= 1; ++j)
        {
            if(checkBounds(x + i, y + j))
            {
                func(x + i, y + j);
            }
        }
    }
}

void MinefieldData::traverseCells(std::function<void (int, int)> func)
{
    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            func(x, y);
        }
    }
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
