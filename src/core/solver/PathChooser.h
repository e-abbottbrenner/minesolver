#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include <QHash>
#include <QPair>
#include <QSharedPointer>
#include <QVector>

class Minefield;

typedef QPair<int, int> Coordinate;
typedef QVector<Coordinate> CoordVector;

// chooses the path used by the MineStateMachine
class PathChooser
{
public:
    PathChooser(QSharedPointer<Minefield const> minefield);

    void decidePath();

    const CoordVector &getPath() const;

private:
    QSharedPointer<Minefield const> minefield;

    CoordVector path;

    CoordVector pathSources;

    int width;
    int height;

    Coordinate nextCoord();

    int countAdjacentUnknowns(int x, int y) const;

    // this tracks the count cells that are influenced by the path
    // the cell is the key and the value is the number of adjacent unknown cells outside of the path
    // when all adjacent unknown cells are in the path, the count will be zero and the cell can be removed
    // the best next coordinate corresponds to the smallest increase in the size of this hash
    // this is because the size of this set exponentially affects the upper bound of the choice column corresponding to the coord in the path
    // because cells that can still be influenced by branching decisions in the path will grow the states exponentially
    QHash<Coordinate, int> pathInfluence;

//    FringeSet getUpdatedFringe(const FringeMap& currentFringe, Coordinate coord);
};

#endif // PATHCHOOSER_H
