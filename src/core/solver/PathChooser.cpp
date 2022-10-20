#include "PathChooser.h"

#include "SolverMinefield.h"

#include <QDebug>

PathChooser::PathChooser(const SolverMinefield &minefield)
    : minefield(minefield)
{
    width = minefield.getWidth();
    height = minefield.getHeight();
}

/*
 * This method figures out the path to use when constructing the mine state machine
 * It used to be more complicated but the complexity wasn't the optimization that was promised (see first rule of optimization)
 */
void PathChooser::decidePath()
{
    path.clear();
    cellsOffPath.clear();

    auto tryAddCellToPath = [&] (int x, int y)
    {
        MineStatus status = minefield.getCell(x, y);

        // some cells can be pre-visited because they're already known to be mines or clear
        // we also leave out any cells with no adjacents, these are just represented as a raw number
        if(status < 0 && status != SpecialStatus::Visited)
        {
            if(countAdjacentCountCells(x, y) > 0)
            {
                path.append({x, y});
            }
            else
            {
                // rather than building a state machine for these, we're just going to assign counts for this section of the field with n choose k math
                // this works because there's no information on them besides how many mines are in them
                cellsOffPath.append({x, y});
            }
        }
    };

    // going along the rows/columns seems to work about the same as greedy on square boards and better on rectangular...
    if(width > height)
    {
        for(int x = 0; x < width; ++x)
        {
            for(int y = 0; y < height; ++y)
            {
                tryAddCellToPath(x, y);
            }
        }
    }
    else
    {
        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                tryAddCellToPath(x, y);
            }
        }
    }
}

int PathChooser::countAdjacentCountCells(int x, int y) const
{
    int countCells = 0;

    minefield.traverseAdjacentCells(x, y, [&] (int x, int y) -> void {minefield.getCell(x, y) >= 0? ++countCells : 0;});

    return countCells;
}

const CoordVector &PathChooser::getPath() const
{
    return path;
}

const CoordVector &PathChooser::getCellsOffPath() const
{
    return cellsOffPath;
}
