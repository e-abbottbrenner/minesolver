//#include "PathChooser.h"

//#include "MinefieldData.h"

//PathChooser::PathChooser(MinefieldData *minefield)
//    : fieldData(minefield)
//{
//    width = fieldData->getWidth();
//    height = fieldData->getHeight();
//    fieldCells = fieldData->getRevealedMinefield();
//}

///*
// * This method figures out the path to use when constructing the mine state machine
// *
// * It works by being greedy and trying to minimize the fringe as it tries to traverse
// * This does not guarantee that we'll get an optimal path, but it does guarantee that
// * we will never have more nodes in our fringe than is absolutely necessary
// */
//void PathChooser::decidePath()
//{
//    FringeMap currentFringe;

//    bool complete = false;
//    while(!complete)
//    {
//        Coordinate coord = getNextCoord(currentFringe);
//        if(coord.first != -1 && coord.second != -1)
//        {
//            path.append(coord);

//            // takes a cell near coord and updates the fringe to account for it
//            auto updateFringe = [&] (int x, int y)
//            {
//                Coordinate fringeCoord(x, y);

//                if(fieldData->getCell(x, y) >= 0 && currentFringe.contains(fringeCoord))
//                {// one less unknown cell influencing this fringe cell
//                    int remainingInfluencers = currentFringe.value(fringeCoord);
//                    if(remainingInfluencers > 1)
//                    {
//                        currentFringe.insert(fringeCoord, remainingInfluencers - 1);
//                    }
//                    else
//                    {// no more influencers left, it's not in our fringe anymore
//                        currentFringe.remove(fringeCoord);
//                    }
//                }
//                else
//                {
//                    int influencers = countAdjacentUnknowns(x, y);

//                    if(influencers > 1)
//                    {// we've already found one influencer in coord
//                        currentFringe.insert(fringeCoord, influencers - 1);
//                    }
//                }
//            };

//            // update the fringe
//            fieldData->traverseAdacentCells(coord.first, coord.second, updateFringe);
//        }
//        else
//        {
//            complete = true;
//        }
//    }
//}

//int PathChooser::countAdjacentUnknowns(int x, int y) const
//{
//    int influencers = 0;

//    fieldData->traverseAdacentCells(x, y, [&] (int x, int y) -> void {fieldData->getCell(x, y) == SpecialStatus::Unknown? ++influencers : NULL;});

//    return influencers;
//}

//// this method finds the starting coord for the path
//Coordinate PathChooser::getNextCoord(const FringeMap& currentFringe) const
//{
//    Coordinate bestCoord(-1, -1);
//    int smallestFringeDelta;// doesn't need to be assigned here, we check best coord to see if it's invalid
//    bool bestCoordHasAdjacentCountCells;

//    // figure out if the coordinate x, y is the best so far and update the local variables above
//    auto findBestCoord = [&] (int x, int y)
//    {
//        MineStatus coordStatus = fieldData->getCell(x, y);

//        if(coordStatus == SpecialStatus::Unknown)
//        {// status is unknown, so this could be a cell in our path
//            CoordVector adjacentCountCells;
//            int fringeDelta = 0;

//            // adds the cell at x, y to the adjacentCountCells if it's a count cell
//            auto getAdjacentCountCells = [&] (int x, int y)
//            {
//                MineStatus adjacentCellStatus = fieldData->getCell(x, y);

//                // we don't need to worry about an i == 0, j == 0 check because we know the cell at x, y is unknown
//                if(adjacentCellStatus >= 0)
//                {// this is a count cell, its status is above 0
//                    adjacentCountCells.append(Coordinate(x, y));
//                }
//            };

//            // construct the list of adjacent count cells
//            fieldData->traverseAdacentCells(x, y, getAdjacentCountCells);

//            // use the list of adjacent count cells to compute the change in our fringe
//            for(CoordVector::Iterator coordIter = adjacentCountCells.begin(); coordIter != adjacentCountCells.end(); ++coordIter)
//            {
//                if(currentFringe.contains(*coordIter) && currentFringe.value(*coordIter) == 1)
//                {
//                    // our current cell is the last adjacent to this count cell so the fringe decreases
//                    --fringeDelta;
//                }
//                else if(!currentFringe.contains(*coordIter) &&
//                        countAdjacentUnknowns(coordIter->first, coordIter->second) > 1)
//                {// the cell is new to the fringe and has more than one influencer
//                    ++fringeDelta;
//                }
//            }

//            if(bestCoord.first < 0 || bestCoord.second < 0 // negative coordinates indicates this is the first iteration
//                    || !bestCoordHasAdjacentCountCells // the current best had no adjacent count cells, we want to save these for last
//                    || fringeDelta <= smallestFringeDelta) // take the cell that has the smallest fringe delta
//            {
//                bestCoord = Coordinate(x, y);
//                bestCoordHasAdjacentCountCells = !adjacentCountCells.isEmpty();
//                smallestFringeDelta = fringeDelta;
//            }
//        }
//    };

//    // traverse all the cells and find the best coordinate
//    fieldData->traverseCells(findBestCoord);

//    return bestCoord;
//}
