#include "PotentialMinefield.h"

#include "MineStatus.h"

PotentialMinefield::PotentialMinefield(QByteArray minefield, int width, int height)
    : TraversableGrid(width, height), minefieldBytes(minefield)
{

}

PotentialMinefield PotentialMinefield::chooseMine(int x, int y) const
{
    return chooseCellState(x, y, true);
}

PotentialMinefield PotentialMinefield::chooseClear(int x, int y) const
{
    return chooseCellState(x, y, false);
}

bool PotentialMinefield::isLegal() const
{
    return legal;
}

QString PotentialMinefield::toString() const
{
    QString result;

    result += QString("legal? ") + (legal? "true" : "false") + " | ";

    for(int x = 0; x < getWidth(); ++x)
    {
        for(int y = 0; y < getHeight(); ++y)
        {
            MineStatus status = minefieldBytes[mapToArray(x, y)];

            if(SpecialStatus::Visited == status)
            {
                result += "V ";
            }
            else if(status < 0)
            {
                result += "U ";
            }
            else
            {
                result += QString::number(status) + " ";
            }
        }

        result += " | ";
    }

    return result;
}

const QByteArray &PotentialMinefield::getMinefieldBytes() const
{
    return minefieldBytes;
}

PotentialMinefield PotentialMinefield::chooseCellState(int x, int y, bool mine) const
{
    PotentialMinefield resultField(*this);

    int allAdjacentsKnown = true;

    // Checks if the state of the cell is known. If all states surrounding a cell are known it has to have a count of zero for the board to be legal
    auto checkIfAllAdjacentsAreStillKnown = [&] (int l, int m)
    {
        MineStatus status = resultField.minefieldBytes[mapToArray(l, m)];

        if(SpecialStatus::Visited != status && status < 0)
        {// we found an unknown adjacent cell
            allAdjacentsKnown = false;
        }
    };

    auto updateCell = [&] (int i, int j)
    {
        int cellAddress = mapToArray(i, j);
        // the cell we're updating is a count cell
        if(resultField.minefieldBytes[cellAddress] >= 0)
        {
            if(mine)
            {
                // since it has a new adjacent mine, we reduce the count in the cell by 1
                // can't just do -- because of char arithmetic, need to cast to ints to get proper signage
                qint8 updatedValue = static_cast<qint8>(resultField.minefieldBytes[cellAddress]) - 1;

                if(updatedValue >= 0)
                {
                    resultField.minefieldBytes[cellAddress] = updatedValue;
                }
                else
                {
                    // if the count drops below 0, the field is not legal because the count would be a lie
                    resultField.legal = false;
                }
            }

            // we can also visit the last node adjacent node and leave an illegal state if we're a mine
            // start from true, the lambda will change this to false if it finds an unknown
            allAdjacentsKnown = true;

            // need to be certain the cell is ok, if all its adjacents are positive and visited it must be zero
            resultField.traverseAdjacentCells(i, j, checkIfAllAdjacentsAreStillKnown);

            if(allAdjacentsKnown && resultField.minefieldBytes[mapToArray(i, j)] > 0)
            {// there's no way this count can reach zero now, so it's not a legal state
                resultField.legal = false;
            }
        }
    };

    // first we mark the cell we're choosing as visited
    resultField.minefieldBytes[mapToArray(x, y)] = SpecialStatus::Visited;

    // then we update all adjacent cells in the result so that their counts correspond to this cell being a mine
    resultField.traverseAdjacentCells(x, y, updateCell);

    return resultField;
}
