#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include "SolverMinefield.h"

#include <QHash>
#include <QPair>
#include <QSharedPointer>
#include <QVector>


typedef QPair<int, int> Coordinate;
typedef QVector<Coordinate> CoordVector;

// chooses the path used by the MineStateMachine
class PathChooser
{
public:
    PathChooser(const SolverMinefield& minefield);

    void decidePath();

    const CoordVector &getPath() const;
    const CoordVector &getTailPath() const;

private:
    SolverMinefield minefield;

    CoordVector path;
    CoordVector tailPath;

    int width;
    int height;

    int countAdjacentCountCells(int x, int y) const;
};

#endif // PATHCHOOSER_H
