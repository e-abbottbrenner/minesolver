#include "Minefield.h"

#include <QDebug>
#include <QPair>
#include <QStack>
#include <QTimer>

#include <random>
#include <algorithm>

typedef QPair<int, int> Coordinate;

Minefield::Minefield(int mineCount, int width, int height, int seed, QObject *parent) :
    QObject(parent), TraversableGrid(width, height), mineCount(mineCount), seed(seed)
{
    populated = false;

    unrevealedCount = width * height;

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
    int bannedCells = 0;

    QList<int> clearIndices;

    auto addClearIndex = [&] (int x, int y) {
        clearIndices.append(mapToArray(x, y));
        ++bannedCells;
    };

    addClearIndex(originX, originY);

    // count the number of cells within bounds that aren't allowed to be mines
    traverseAdjacentCells(originX, originY, addClearIndex);

    std::sort(clearIndices.begin(), clearIndices.end());

    int freeCells = underlyingMinefield.size() - bannedCells;

    QByteArray newUnderlying(freeCells, SpecialStatus::Unknown);

    for(int i = 0; i < mineCount && i < newUnderlying.size(); ++i)
    {
        newUnderlying[i] = SpecialStatus::Mine;
    }

    std::mt19937 mersenneTwister(seed);
    std::shuffle(newUnderlying.begin(), newUnderlying.end(), mersenneTwister);

    for(int index: clearIndices)
    {
        // newUnderlying is the size of underlyingMinefield - bannedCells
        // these indices function on an array of the size of underlyingMinefield
        // they are sorted from smallest to largest
        // these clear indices are effectively missing from newUnderlying
        // the first index they are missing from is the smallest index in clearIndices
        // so once a value is inserted for that index, the array will shift and the next index will have a valid position
        newUnderlying.insert(index, SpecialStatus::Unknown);
    }

    underlyingMinefield = newUnderlying;

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

void Minefield::revealAdjacents(int x, int y)
{
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
                revealCell(x, y);
            }
        };

        traverseAdjacentCells(x, y, reveal);
    }
}

void Minefield::revealCell(int x, int y, bool force)
{
    if(!populated)
    {
        populateMinefield(x, y);
    }

    int cellIndex = mapToArray(x, y);

    if(revealedMinefield[cellIndex] == SpecialStatus::GuessMine && !force)
    {// not allowed to reveal guesses
        return;
    }

    bool clear = underlyingMinefield[cellIndex] != SpecialStatus::Mine;

    if(clear)
    {
        recursiveReveal(x, y);
    }
    else
    {
        emit mineHit();

        revealMines();

        revealedMinefield[cellIndex] = SpecialStatus::ExplodedMine;
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

            --unrevealedCount;

            if(unrevealedCount <= mineCount)
            {
                emit allCountCellsRevealed();
            }

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

void Minefield::revealAll()
{
    auto reveal = [&] (int x, int y)
    {
        int cellIndex = mapToArray(x, y);

        if(revealedMinefield[cellIndex] != underlyingMinefield[cellIndex])
        {
            revealedMinefield[cellIndex] = underlyingMinefield[cellIndex];

            if(revealedMinefield[cellIndex] == SpecialStatus::Mine)
            {
                revealedMinefield[cellIndex] = SpecialStatus::UnexplodedMine;
            }

            emit cellUpdated(x, y);
        }
    };

    traverseCells(reveal);
}

void Minefield::revealMines()
{
    auto reveal = [&] (int x, int y)
    {
        int cellIndex = mapToArray(x, y);

        if(SpecialStatus::Mine == underlyingMinefield[cellIndex])
        {
            revealedMinefield[cellIndex] = underlyingMinefield[cellIndex];

            emit cellUpdated(x, y);
        }
    };

    traverseCells(reveal);
}

bool Minefield::isPopulated() const
{
    return populated;
}

int Minefield::getMineCount() const
{
    return mineCount;
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

QSharedPointer<Minefield> Minefield::clone() const
{
    QSharedPointer<Minefield> cloneField(new Minefield(mineCount, getWidth(), getHeight(), seed));
    cloneField->populated = populated;
    cloneField->underlyingMinefield = underlyingMinefield;
    cloneField->revealedMinefield = revealedMinefield;

    return cloneField;
}

Minefield::~Minefield()
{
}
