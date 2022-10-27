#include "ObviousCellFlagger.h"
#include "Minefield.h"

ObviousCellFlagger::ObviousCellFlagger(const SolverMinefield &minefield)
    : minefield(minefield)
{

}

const QHash<Coordinate, double> &ObviousCellFlagger::getChancesToBeMine() const
{
    return chancesToBeMine;
}

void ObviousCellFlagger::flagObviousCells()
{
    int previousKnownCount = -1;

    while(previousKnownCount != chancesToBeMine.count())
    {
        previousKnownCount = chancesToBeMine.count();

        for(int x = 0; x < minefield.getWidth(); ++x)
        {
            for(int y = 0; y < minefield.getHeight(); ++y)
            {
                flagObviousAdjacents(x, y);
            }
        }
    }
}

void ObviousCellFlagger::flagObviousAdjacents(int x, int y)
{
    auto markAdjacentAsMines = [&] (int x, int y)
    {
        MineStatus status = minefield.getCell(x, y);

        if(status < 0 && status != SpecialStatus::Visited)
        {// unknown, unvisited cell, mark it mine
            minefield = minefield.chooseMine(x, y);

            chancesToBeMine.insert({x, y}, 1);
        }
    };

    auto markAdjacentAsClear = [&] (int x, int y)
    {
        MineStatus status = minefield.getCell(x, y);

        if(status < 0 && status != SpecialStatus::Visited)
        {// unknown, unvisited cell, mark it clear
            minefield = minefield.chooseClear(x, y);

            chancesToBeMine.insert({x, y}, 0);
        }
    };

    MineStatus status = minefield.getCell(x, y);

    if(status > 0 && status == minefield.countAdjacentUnknowns(x, y))
    {// the count is the same as the number of adjacent unknown cells, they are all mines
        minefield.traverseAdjacentCells(x, y, markAdjacentAsMines);
    } else
    if(status == 0)
    {
        minefield.traverseAdjacentCells(x, y, markAdjacentAsClear);
    }
}
