#include "MinefieldData.h"

#include "RandomNumbers.h"

MinefieldData::MinefieldData(int numMines, int width, int height, int seed, QObject *parent) :
    QObject(parent), numMines(numMines), width(width), height(height), seed(seed)
{
    populated = false;

    underlyingMinefield.resize(width * height, SpecialStatus::Unknown);
    revealedMinefield.resize(width * height, SpecialStatus::Unknown);
    for(int i = 0; i < width; ++i)
    {
        for(int j = 0; j < height; ++j)
        {
            underlyingMinefield[map(i, j)] = SpecialStatus::Unknown;
            revealedMinefield[map(i, j)] = SpecialStatus::Unknown;
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
            if(underlyingMinefield[map(x, y)] != SpecialStatus::Mine
                    && (x > originX + 1 || x < originX - 1
                        || y > originY + 1 || y < originY - 1))
            {// not already a mine or within a cell of the origin
                if(mineLoc == 0)
                {
                    underlyingMinefield[map(x, y)] = SpecialStatus::Mine;
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
        if(underlyingMinefield[map(x, y)] == SpecialStatus::Mine)
        {
            ++mineCount;
        }
    };

    auto labelCells = [&] (int x, int y)
    {
        mineCount = 0;

        // label the cells that don't have mines with their mine counts
        if(underlyingMinefield[map(x, y)] != SpecialStatus::Mine)
        {
            traverseAdacentCells(x, y, countMine);

            underlyingMinefield[map(x, y)] = mineCount;
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
        if(revealedMinefield[map(x, y)] == SpecialStatus::GuessMine)
        {
            ++guessCount;
        }
    };

    traverseAdacentCells(x, y, countGuess);

    if(guessCount == revealedMinefield[map(x, y)])
    {
        auto reveal = [&] (int x, int y)
        {
            if(revealedMinefield[map(x, y)] != SpecialStatus::GuessMine)
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

    bool clear = underlyingMinefield[map(x, y)] != SpecialStatus::Mine;

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
    if(revealedMinefield[map(x, y)] == SpecialStatus::Unknown)
    {
        revealedMinefield[map(x, y)] = SpecialStatus::GuessMine;
    }
    else if(revealedMinefield[map(x, y)] == SpecialStatus::GuessMine)
    {
        revealedMinefield[map(x, y)] = SpecialStatus::Unknown;
    }

    emit cellRevealed(x, y);
}

void MinefieldData::recursiveReveal(int x, int y)
{
    if(underlyingMinefield[map(x, y)] != revealedMinefield[map(x, y)])
    {
        revealedMinefield[map(x, y)] = underlyingMinefield[map(x, y)];

        emit cellRevealed(x, y);

        if(underlyingMinefield[map(x, y)] == 0)
        {// no nearby mines, do the recursive reveal
            traverseAdacentCells(x, y, [this] (int x, int y) {recursiveReveal(x, y);});
        }
    }
}

void MinefieldData::revealAll()
{
    auto reveal = [&] (int x, int y)
    {
        revealedMinefield[map(x, y)] = underlyingMinefield[map(x, y)];
        emit cellRevealed(x, y);
    };

    traverseCells(reveal);
}

int MinefieldData::map(int x, int y) const
{
    return x + y * width;
}

MineStatus MinefieldData::getCell(int x, int y) const
{
    return revealedMinefield[map(x, y)];
}

QByteArray MinefieldData::getRevealedMinefield() const
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
}
