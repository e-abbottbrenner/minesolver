#ifndef SOLVER_H
#define SOLVER_H

#include <QList>
#include <QPair>
#include <QSharedPointer>
#include <QVector>

typedef QPair<int, int> Coordinate;
typedef QVector<Coordinate> CoordVector;

class ChoiceColumn;
class Minefield;

class Solver
{
public:
    explicit Solver(Minefield *minefield);

    void computeSolution();

private:
    CoordVector path;
    QList<QSharedPointer<ChoiceColumn>> choiceColumns;

    void decidePath();
    void buildSolutionGraph();

    Minefield* minefield;
};

#endif // SOLVER_H
