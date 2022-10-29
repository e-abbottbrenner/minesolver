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
    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
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
                    tailPath.append({x, y});
                }
            }
        }
    }

    optimizePath();
}

int PathChooser::countAdjacentCountCells(int x, int y) const
{
    int countCells = 0;

    minefield.traverseAdjacentCells(x, y, [&] (int x, int y) -> void {minefield.getCell(x, y) >= 0? ++countCells : 0;});

    return countCells;
}

void PathChooser::optimizePath()
{
    // this optimizer greedily chooses cells that have the highest number of influenced count cells that are already influenced by the path
    // in other words it prefers to visit all unknowns around cells that are adjacent to the path
    // this seems to give more consistent results than just running along rows/columns
    auto pathSources = path;
    path.clear();

    QHash<Coordinate, int> influencedCells;

    auto traverseAdjacentCountCells = [&] (int x, int y, std::function<void(int, int)> func) {
        minefield.traverseAdjacentCells(x, y, [&] (int countCellX, int countCellY) -> void { if(minefield.getCell(countCellX, countCellY) >= 0) func(countCellX, countCellY); });
    };

    auto countAlreadyInfluencedByPath = [&] (int x, int y) {
        int influencedByPath = 0;

        traverseAdjacentCountCells(x, y, [&] (int countCellX, int countCellY) -> void {
            if(influencedCells.contains({countCellX, countCellY}))
            {
                ++influencedByPath;
            }
        });

        return influencedByPath;
    };

    auto updateInfluencedCells = [&] (int x, int y) {
        traverseAdjacentCountCells(x, y, [&] (int countCellX, int countCellY) -> void {
            Coordinate coord(countCellX, countCellY);

            if(influencedCells.contains(coord))
            {
                // -1 because we're going to visit another one of its adjacent unknowns
                influencedCells.insert(coord, influencedCells[coord] - 1);
            }
            else
            {
                // -1 because we're visiting the first of its adjacent unknowns
                influencedCells.insert(coord, minefield.countAdjacentUnknowns(countCellX, countCellY) - 1);
            }

            if(influencedCells[coord] <= 0)
            {// no longer relevant to the path, all cells were visited
                influencedCells.remove(coord);
            }
        });
    };

    while(!pathSources.isEmpty())
    {
        int bestSourceIndex = 0;
        int bestInfluencedInPathCount = 0;

        for(int i = 0; i < pathSources.size(); ++i)
        {
            auto coord = pathSources[i];

            int influencedInPathCount = countAlreadyInfluencedByPath(coord.first, coord.second);

            if(influencedInPathCount < bestInfluencedInPathCount)
            {
                bestInfluencedInPathCount = influencedInPathCount;
                bestSourceIndex = i;
            }
        }

        path.append(pathSources.takeAt(bestSourceIndex));
        updateInfluencedCells(path.last().first, path.last().second);
    }
}

const CoordVector &PathChooser::getPath() const
{
    return path;
}

const CoordVector &PathChooser::getTailPath() const
{
    return tailPath;
}
