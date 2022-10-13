#include "PotentialMinefield.h"

#include "MineStatus.h"

PotentialMinefield::PotentialMinefield(QByteArray minefield, int width, int height)
    : TraversableGrid(width, height), minefield(minefield)
{

}

PotentialMinefield PotentialMinefield::chooseMine(int x, int y) const
{
    PotentialMinefield resultField(*this);

    auto updateForMine = [&] (int i, int j)
    {
        int cellAddress = mapToArray(i, j);
        // the cell we're updating is a count cell
        if(resultField.minefield[cellAddress] >= 0)
        {
            // since it has a new adjacent mine, we reduce the count in the cell by 1
            int updatedValue = resultField.minefield[cellAddress]--;

            // if the count drops below 0, the field is not legal because the count would be a lie
            if(updatedValue < 0)
            {
                resultField.legal = false;
            }
        }
    };

    // first we mark the cell we're choosing as visited
    resultField.minefield[mapToArray(x, y)] = SpecialStatus::Visited;

    // then we update all adjacent cells in the result so that their counts correspond to this cell being a mine
    resultField.traverseAdjacentCells(x, y, updateForMine);

    return resultField;
}

PotentialMinefield PotentialMinefield::chooseClear(int x, int y) const
{
    PotentialMinefield resultField(*this);

    int allAdjacentsKnown = true;

    // Checks if the state of the cell is known. If all states surrounding a cell are known it has to have a count of zero for the board to be legal
    auto checkIfAllAdjacentsAreStillKnown = [&] (int l, int m)
    {
        MineStatus status = resultField.minefield[mapToArray(l, m)];

        if(SpecialStatus::Visited != status && status < 0)
        {// we found an unknown adjacent cell
            allAdjacentsKnown = false;
        }
    };

    auto updateForClear = [&] (int i, int j)
    {
        // start from true, the lambda will change this to false if it finds an unknown
        allAdjacentsKnown = true;

        // need to be certain the cell is ok, if all its adjacents are positive and visited it must be zero
        resultField.traverseAdjacentCells(i, j, checkIfAllAdjacentsAreStillKnown);

        if(allAdjacentsKnown && resultField.minefield[mapToArray(i, j)] > 0)
        {// there's no way this count can reach zero now, so it's not a legal state
            resultField.legal = false;
        }
    };

    // do this first so we mark it visited for isKnown
    resultField.minefield[mapToArray(x, y)] = SpecialStatus::Visited;

    resultField.traverseAdjacentCells(x, y, updateForClear);

    return resultField;
}

bool PotentialMinefield::isLegal() const
{
    return legal;
}
