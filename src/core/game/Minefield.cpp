#include "Minefield.h"

#include <QDebug>
#include <QPair>
#include <QRandomGenerator>
#include <QStack>
#include <QTimer>

typedef QPair<int, int> Coordinate;

Minefield::Minefield(int numMines, int width, int height, int seed, QObject *parent) :
    QObject(parent), TraversableGrid(width, height), numMines(numMines), seed(seed)
{
    populated = false;

    underlyingMinefield.resize(width * height, SpecialStatus::Unknown);
    revealedMinefield.resize(width * height, SpecialStatus::Unknown);
    for(int i = 0; i < width; ++i)
    {
        for(int j = 0; j < height; ++j)
        {
            underlyingMinefield[mapToArray(i, j)] = SpecialStatus::Unknown;
            revealedMinefield[mapToArray(i, j)] = SpecialStatus::Unknown;
        }
    }
}

void Minefield::populateMinefield(int originX, int originY)
{
    QRandomGenerator random;
    random.seed(seed);

    // this is slow, but it's guaranteed to work and I like that
    // I also doubt it's slow enough to notice
    int bannedCells = 0;

    // count the number of cells within bounds that aren't allowed to be mines
    traverseAdjacentCells(originX, originY, [&] (int, int) {++bannedCells;});

    int freeCells = getWidth() * getHeight() - bannedCells;

    for(int mineCount = 0; mineCount < numMines && freeCells > 0; ++mineCount, --freeCells)
    {
        int mineLoc = random.bounded(freeCells);

        auto placeMines = [&] (int x, int y)
        {
            if(underlyingMinefield[mapToArray(x, y)] != SpecialStatus::Mine
                    && (x > originX + 1 || x < originX - 1
                        || y > originY + 1 || y < originY - 1))
            {// not already a mine or within a cell of the origin
                if(mineLoc == 0)
                {
                    underlyingMinefield[mapToArray(x, y)] = SpecialStatus::Mine;
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
        if(underlyingMinefield[mapToArray(x, y)] == SpecialStatus::Mine)
        {
            ++mineCount;
        }
    };

    auto labelCells = [&] (int x, int y)
    {
        mineCount = 0;

        // label the cells that don't have mines with their mine counts
        if(underlyingMinefield[mapToArray(x, y)] != SpecialStatus::Mine)
        {
            traverseAdjacentCells(x, y, countMine);

            underlyingMinefield[mapToArray(x, y)] = mineCount;
        }
    };

    traverseCells(labelCells);

    populated = true;
}

QList<Coordinate> Minefield::revealAdjacents(int x, int y)
{
    QList<Coordinate> coordsRevealed;

    int guessCount = 0;

    auto countGuess = [&] (int x, int y)
    {
        if(revealedMinefield[mapToArray(x, y)] == SpecialStatus::GuessMine)
        {
            ++guessCount;
        }
    };

    traverseAdjacentCells(x, y, countGuess);

    if(guessCount == revealedMinefield[mapToArray(x, y)])
    {
        auto reveal = [&] (int x, int y)
        {
            if(revealedMinefield[mapToArray(x, y)] != SpecialStatus::GuessMine)
            {
                coordsRevealed.append(revealCell(x, y));
            }
        };

        traverseAdjacentCells(x, y, reveal);
    }

    return coordsRevealed;
}

QList<Coordinate> Minefield::revealCell(int x, int y)
{
    if(!populated)
    {
        populateMinefield(x, y);
    }

    if(revealedMinefield[mapToArray(x, y)] == SpecialStatus::GuessMine)
    {// not allowed to reveal guesses
        return {};
    }

    bool clear = underlyingMinefield[mapToArray(x, y)] != SpecialStatus::Mine;

    if(clear)
    {
        return recursiveReveal(x, y);
    }
    else
    {
        return revealAll();

        emit mineHit();
    }
}

void Minefield::toggleGuessMine(int x, int y)
{
    if(revealedMinefield[mapToArray(x, y)] == SpecialStatus::Unknown)
    {
        revealedMinefield[mapToArray(x, y)] = SpecialStatus::GuessMine;
    }
    else if(revealedMinefield[mapToArray(x, y)] == SpecialStatus::GuessMine)
    {
        revealedMinefield[mapToArray(x, y)] = SpecialStatus::Unknown;
    }

    emit cellUpdated(x, y);
}

QList<Coordinate> Minefield::recursiveReveal(int x, int y)
{
    QList<Coordinate> coordsRevealed;
    QStack<Coordinate> revealStack;

    revealStack.push({x, y});

    while(!revealStack.isEmpty())
    {
        Coordinate coord = revealStack.pop();

        int x = coord.first;
        int y = coord.second;

        if(underlyingMinefield[mapToArray(x, y)] != revealedMinefield[mapToArray(x, y)])
        {
            revealedMinefield[mapToArray(x, y)] = underlyingMinefield[mapToArray(x, y)];
            coordsRevealed.append(coord);

            emit cellUpdated(x, y);

            if(underlyingMinefield[mapToArray(x, y)] == 0)
            {// no nearby mines, do the recursive reveal
                traverseAdjacentCells(x, y, [&] (int x, int y)
                {
                    revealStack.push({x, y});
                });
            }
        }
    }

    return coordsRevealed;
}

QList<Coordinate> Minefield::revealAll()
{
    QList<Coordinate> coordsRevealed;

    auto reveal = [&] (int x, int y)
    {
        if(revealedMinefield[mapToArray(x, y)] != underlyingMinefield[mapToArray(x, y)])
        {
            revealedMinefield[mapToArray(x, y)] = underlyingMinefield[mapToArray(x, y)];
            coordsRevealed.append({x, y});
            emit cellUpdated(x, y);
        }
    };

    traverseCells(reveal);

    return coordsRevealed;
}

int Minefield::getNumMines() const
{
    return numMines;
}

MineStatus Minefield::getCell(int x, int y) const
{
    return revealedMinefield[mapToArray(x, y)];
}

MineStatus Minefield::getUnderlyingCell(int x, int y) const
{
    return underlyingMinefield[mapToArray(x, y)];
}

QByteArray Minefield::getRevealedMinefield() const
{
    return revealedMinefield;
}

Minefield::~Minefield()
{
}
