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
};

#endif // PATHCHOOSER_H
