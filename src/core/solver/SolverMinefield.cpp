#include "SolverMinefield.h"

#include "MineStatus.h"

SolverMinefield::SolverMinefield(QByteArray minefield, int width, int height)
    : TraversableGrid(width, height), minefieldBytes(minefield)
{

}

SolverMinefield SolverMinefield::chooseMine(int x, int y) const
{
    return chooseCellState(x, y, true);
}

SolverMinefield SolverMinefield::chooseClear(int x, int y) const
{
    return chooseCellState(x, y, false);
}

bool SolverMinefield::isLegal() const
{
    return legal;
}

QString SolverMinefield::toString() const
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

MineStatus SolverMinefield::getCell(int x, int y) const
{
    return minefieldBytes[mapToArray(x, y)];
}

const QByteArray &SolverMinefield::getMinefieldBytes() const
{
    return minefieldBytes;
}

int SolverMinefield::countAdjacentUnknowns(int x, int y) const
{
    int adjacentUnknownCount = 0;

    // Checks if the state of the cell is known. If all states surrounding a cell are known it has to have a count of zero for the board to be legal
    auto updateAdjacentUnknownCount = [&] (int l, int m)
    {
        MineStatus status = minefieldBytes[mapToArray(l, m)];

        if(SpecialStatus::Visited != status && status < 0)
        {// we found an unknown adjacent cell
            ++adjacentUnknownCount;
        }
    };

    traverseAdjacentCells(x, y, updateAdjacentUnknownCount);

    return adjacentUnknownCount;
}

void SolverMinefield::validateMinefield()
{
    for(int x = 0; x < getWidth(); ++x)
    {
        for(int y = 0; y < getHeight(); ++y)
        {
            MineStatus status = minefieldBytes[mapToArray(x, y)];

            // Invalid status is -1, if a count cell reaches it then it's invalid
            if(status == SpecialStatus::Invalid ||
                    (status >= 0 && countAdjacentUnknowns(x, y) < status))
            {// this is a contradiction, it's not possible for there to be enough mines to satisfy status
                legal = false;
                return;
            }
        }
    }

    legal = true;
}

SolverMinefield SolverMinefield::chooseCellState(int x, int y, bool mine) const
{
    SolverMinefield resultField(*this);

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
                // Note: if we go below 0 on the count it will reach the SpecialStatus::Invalid state
                qint8 updatedValue = static_cast<qint8>(resultField.minefieldBytes[cellAddress]) - 1;

                resultField.minefieldBytes[cellAddress] = updatedValue;
            }
        }
    };

    // first we mark the cell we're choosing as visited
    resultField.minefieldBytes[mapToArray(x, y)] = SpecialStatus::Visited;

    // then we update all adjacent cells in the result so that their counts correspond to this cell being a mine
    resultField.traverseAdjacentCells(x, y, updateCell);

    resultField.validateMinefield();

    return resultField;
}
