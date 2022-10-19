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

private:
    SolverMinefield minefield;

    CoordVector path;

    CoordVector pathSources;

    int width;
    int height;
};

#endif // PATHCHOOSER_H
