#include "Minefield.h"

#include <QDebug>
#include <QMutexLocker>
#include <QPair>
#include <QSharedPointer>
#include <QStack>
#include <QTimer>

#include <random>
#include <algorithm>

typedef QPair<int, int> Coordinate;

Minefield::Minefield(int mineCount, int width, int height, int seed, QObject *parent) :
    QObject(parent), TraversableGrid(width, height), MINE_COUNT(mineCount), SEED(seed)
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

void Minefield::ensureMinefieldPopulated(int originX, int originY)
{
    QMutexLocker locker(&minefieldMutex);

    ensureMinefieldPopulatedPrivate(originX, originY);
}

void Minefield::revealAdjacents(int x, int y)
{
    QMutexLocker locker(&minefieldMutex);

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
                // already locked so don't lock a second time
                revealCellPrivate(x, y, false);
            }
        };

        traverseAdjacentCells(x, y, reveal);
    }
}

void Minefield::revealCell(int x, int y, bool force)
{
    QMutexLocker locker(&minefieldMutex);

    revealCellPrivate(x, y, force);
}

void Minefield::toggleGuessMine(int x, int y)
{
    QMutexLocker locker(&minefieldMutex);

    if(revealedMinefield[mapToArray(x, y)] == SpecialStatus::Unknown)
    {
        revealedMinefield[mapToArray(x, y)] = SpecialStatus::GuessMine;
    }
    else if(revealedMinefield[mapToArray(x, y)] == SpecialStatus::GuessMine)
    {
        revealedMinefield[mapToArray(x, y)] = SpecialStatus::Unknown;
    }

    emit cellToggled(x, y);
}

void Minefield::revealAll()
{
    QMutexLocker locker(&minefieldMutex);

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

            queueCellRevealed(x, y);
        }
    };

    traverseCells(reveal);
}

bool Minefield::isPopulated() const
{
    QMutexLocker locker(&minefieldMutex);

    return populated;
}

bool Minefield::areAllCountCellsRevealed() const
{
    QMutexLocker locker(&minefieldMutex);

    return areAllCountCellsRevealedPrivate();
}

bool Minefield::wasMineHit() const
{
    QMutexLocker locker(&minefieldMutex);

    return mineWasHit;
}

int Minefield::getMineCount() const
{
    return MINE_COUNT;
}

MineStatus Minefield::getCell(int x, int y) const
{
    QMutexLocker locker(&minefieldMutex);

    return revealedMinefield[mapToArray(x, y)];
}

MineStatus Minefield::getUnderlyingCell(int x, int y) const
{
    QMutexLocker locker(&minefieldMutex);

    return underlyingMinefield[mapToArray(x, y)];
}

QByteArray Minefield::getRevealedMinefield() const
{
    QMutexLocker locker(&minefieldMutex);

    return revealedMinefield;
}

void Minefield::revealCellPrivate(int x, int y, bool force)
{
    ensureMinefieldPopulatedPrivate(x, y);

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
        mineWasHit = true;

        emit mineHit();

        revealMines();

        revealedMinefield[cellIndex] = SpecialStatus::ExplodedMine;
    }
}

void Minefield::recursiveReveal(int x, int y)
{
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

            if(areAllCountCellsRevealedPrivate())
            {
                emit allCountCellsRevealed();
            }

            queueCellRevealed(x, y);

            if(underlyingMinefield[mapToArray(x, y)] == 0)
            {// no nearby mines, do the recursive reveal
                traverseAdjacentCells(x, y, [&] (int x, int y)
                {
                    revealStack.push({x, y});
                });
            }
        }
    }
}

void Minefield::revealMines()
{
    auto reveal = [&] (int x, int y)
    {
        int cellIndex = mapToArray(x, y);

        if(SpecialStatus::Mine == underlyingMinefield[cellIndex])
        {
            revealedMinefield[cellIndex] = underlyingMinefield[cellIndex];

            queueCellRevealed(x, y);
        }
    };

    traverseCells(reveal);
}

void Minefield::ensureMinefieldPopulatedPrivate(int originX, int originY)
{
    if(!populated)
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

        for(int i = 0; i < MINE_COUNT && i < newUnderlying.size(); ++i)
        {
            newUnderlying[i] = SpecialStatus::Mine;
        }

        std::mt19937 mersenneTwister(SEED);
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
}

bool Minefield::areAllCountCellsRevealedPrivate() const
{
    return unrevealedCount <= MINE_COUNT;
}

void Minefield::queueCellRevealed(int x, int y)
{
    cellsToReveal.append({x, y});

    QTimer::singleShot(0, this, &Minefield::deliverCellReveals);
}

void Minefield::deliverCellReveals()
{
    if(cellsToReveal.size() > 0)
    {
        auto revealed = cellsToReveal;
        cellsToReveal.clear();

        emit cellsRevealed(revealed);
    }
}

