#include "PathChooser.h"

#include "MinefieldData.h"

PathChooser::PathChooser(MinefieldData *minefield)
    : fieldData(minefield)
{
    width = fieldData->getWidth();
    height = fieldData->getHeight();
    fieldCells = fieldData->getRevealedMinefield();
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
    FringeMap currentFringe;

    bool complete = false;
    while(!complete)
    {
        Coordinate coord = getNextCoord(currentFringe);
        if(coord.first != -1 && coord.second != -1)
        {
            path.append(coord);

            // TODO: update the fringe
//            for(int i = -1; i <= 1; ++i)
//            {
//                for(int j = -1; j <= 1; ++j)
//                {
//                    if(fieldData->checkBounds(x + i, y + j))
//                    {
//                        MineStatus adjacentCellStatus = fieldData->getCell(x + i, y + j);

//                        // we don't need to worry about an i == 0, j == 0 check because we know the cell at x, y is unknown
//                        if(adjacentCellStatus > 0)
//                        {// this is a count cell, its status is above 0
//                            countCellEncountered = true;

//                            adjacentCountCells.append(Coordinate(x + i, y + j));
//                        }
//                    }
//                }
//            }
        }
        else
        {
            complete = true;
        }
    }
}

// this method finds the starting coord for the path
Coordinate PathChooser::getNextCoord(const FringeMap& currentFringe) const
{

    Coordinate bestCoord(-1, -1);
    int smallestFringeIncrease;// doesn't need to be assigned here, we check best coord to see if it's invalid
    bool bestCoordHasAdjacentCountCells;

    // TODO: lambdas
    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            MineStatus coordStatus = fieldData->getCell(x, y);

            if(coordStatus == SpecialStatus::Unknown)
            {
                CoordVector adjacentCountCells;
                bool countCellEncountered;
                int fringeIncrease = 0;

                for(int i = -1; i <= 1; ++i)
                {
                    for(int j = -1; j <= 1; ++j)
                    {
                        if(fieldData->checkBounds(x + i, y + j))
                        {
                            MineStatus adjacentCellStatus = fieldData->getCell(x + i, y + j);

                            // we don't need to worry about an i == 0, j == 0 check because we know the cell at x, y is unknown
                            if(adjacentCellStatus > 0)
                            {// this is a count cell, its status is above 0
                                countCellEncountered = true;

                                adjacentCountCells.append(Coordinate(x + i, y + j));
                            }
                        }
                    }
                }

                for(CoordVector::Iterator iter = adjacentCountCells.begin(); iter != adjacentCountCells.end(); ++iter)
                {
                    if(currentFringe.contains(*iter) && currentFringe.value(*iter) == 1)
                    {
                        // our current cell is the last adjacent to this count cell
                        --fringeIncrease;
                    }
                    else
                    {
                        ++fringeIncrease;
                    }
                }

                if((bestCoord.first < 0 || bestCoord.second < 0)
                        || (!bestCoordHasAdjacentCountCells || fringeIncrease <= smallestFringeIncrease))
                {
                    bestCoord = Coordinate(x, y);
                    bestCoordHasAdjacentCountCells = countCellEncountered;
                    smallestFringeIncrease = fringeIncrease;
                }
            }
        }
    }

    return bestCoord;
}
