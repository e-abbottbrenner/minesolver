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

    auto tryAddCellToPath = [&] (int x, int y)
    {
        MineStatus status = minefield.getCell(x, y);

        // some cells can be pre-visited because they're already known to be mines or clear
        if(status < 0 && status != SpecialStatus::Visited)
        {
            path.append({x, y});
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

const CoordVector &PathChooser::getPath() const
{
    return path;
}
