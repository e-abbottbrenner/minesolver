#include "PathChooser.h"

#include "Minefield.h"

PathChooser::PathChooser(Minefield *minefield)
    : fieldData(minefield)
{
    width = fieldData->getWidth();
    height = fieldData->getHeight();
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
    // need to fix this, a good path is critical for controlling the size of the solution space
    // let's just do a greedy algorithm that finds the next cell that minimizes the number of count cells that can still be influenced
    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            if(fieldData->getCell(x, y) < 0)
            {
                pathSources.append({x, y});
            }
        }
    }

    while(!pathSources.isEmpty())
    {
        // pull the best next coord out of the list until you have them all
        Coordinate coord = nextCoord();

        path.append(coord);
    }
}

const CoordVector &PathChooser::getPath() const
{
    return path;
}

Coordinate PathChooser::nextCoord()
{
    QHash<Coordinate, int> bestPathInfluence;
    int bestCoordinateIndex = -1;

    for(int i = 0; i < pathSources.size(); ++i)
    {
        auto newPathInfluence = pathInfluence;

        auto addInfluenceFromAdjacent = [&] (int x, int y)
        {
            if(fieldData->getCell(x, y) >= 0)
            {// adjacent count cell, may to go in the influence list
                if(newPathInfluence.contains({x, y}))
                {// coordinate is already influenced
                    int newInfluencerCount = newPathInfluence[{x, y}]--;

                    if(newInfluencerCount == 0)
                    {// this cell can no longer be influenced by future decisions on the path, remove it
                        newPathInfluence.remove({x, y});
                    }
                }
                else
                {
                    // this is a cell that is newly influenced by the choices in the path
                    // we subtract 1 because the coordinate being added to the path is one less influencer
                    int initialAdjacentUnknownCount = countAdjacentUnknowns(x, y) - 1;

                    if(initialAdjacentUnknownCount > 0)
                    {
                        newPathInfluence.insert({x, y}, initialAdjacentUnknownCount);
                    }
                }
            }
        };

        // update newPathInfluence
        fieldData->traverseAdjacentCells(pathSources[i].first, pathSources[i].second, addInfluenceFromAdjacent);

        if(bestCoordinateIndex < 0 || newPathInfluence.size() < bestPathInfluence.size())
        {// find the next coordinate that minimizes the size of the path influence
            bestCoordinateIndex = i;
            bestPathInfluence = newPathInfluence;
        }
    }

    // update path influence to the new best
    pathInfluence = bestPathInfluence;

    return pathSources.takeAt(bestCoordinateIndex);
}

int PathChooser::countAdjacentUnknowns(int x, int y) const
{
    int influencers = 0;

    fieldData->traverseAdjacentCells(x, y, [&] (int x, int y) -> void {fieldData->getCell(x, y) < 0? ++influencers : 0;});

    return influencers;
}
