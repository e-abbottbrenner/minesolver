#ifndef OBVIOUSCELLFLAGGER_H
#define OBVIOUSCELLFLAGGER_H

#include "SolverMinefield.h"

#include <QHash>

typedef QPair<int, int> Coordinate;

class ObviousCellFlagger
{
public:
    explicit ObviousCellFlagger(const SolverMinefield& minefield);

    const QHash<Coordinate, double> &getChancesToBeMine() const;

    void flagObviousCells();

private:
    SolverMinefield minefield;

    QHash<Coordinate, double> chancesToBeMine;

    void flagObviousAdjacents(int x, int y);
};

#endif // OBVIOUSCELLFLAGGER_H
