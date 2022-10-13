#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include <QPair>
#include <QVector>
#include <QHash>

class Minefield;

typedef QPair<int, int> Coordinate;
typedef QVector<Coordinate> CoordVector;
typedef QHash<Coordinate, int> FringeMap;

// chooses the path used by the MineStateMachine
class PathChooser
{
public:
    PathChooser(Minefield *minefield);

    void decidePath();

    const CoordVector &getPath() const;

private:
    Minefield *fieldData;

    CoordVector path;

    int width;
    int height;

    int countAdjacentUnknowns(int x, int y) const;

    Coordinate getNextCoord(const FringeMap &currentFringe) const;
//    FringeSet getUpdatedFringe(const FringeMap& currentFringe, Coordinate coord);
};

#endif // PATHCHOOSER_H
