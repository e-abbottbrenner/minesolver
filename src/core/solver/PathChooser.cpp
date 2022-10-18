#include "PathChooser.h"

#include "Minefield.h"

#include <QDebug>

PathChooser::PathChooser(QSharedPointer<const Minefield> minefield)
    : minefield(minefield)
{
    width = minefield->getWidth();
    height = minefield->getHeight();
}

/*
 * This method figures out the path to use when constructing the mine state machine
 *
 * It works by being greedy and trying to minimize the fringe as it tries to traverse
 * This does not guarantee that we'll get an optimal path, but it does guarantee that
 * we will never have more nodes in our fringe than is absolutely necessary
 */
void PathChooser::decidePath()
{
    path.clear();

    // going along the rows/columns seems to work about the same as greedy on square boards and better on rectangular...

    if(width > height)
    {
        for(int x = 0; x < width; ++x)
        {
            for(int y = 0; y < height; ++y)
            {
                if(minefield->getCell(x, y) < 0)
                {
                    path.append({x, y});
                }
            }
        }
    }
    else
    {
        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                if(minefield->getCell(x, y) < 0)
                {
                    path.append({x, y});
                }
            }
        }
    }
}

const CoordVector &PathChooser::getPath() const
{
    return path;
}
