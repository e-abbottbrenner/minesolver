#ifndef SOLVER_H
#define SOLVER_H

#include <QList>
#include <QHash>
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
    explicit Solver(QSharedPointer<Minefield const> minefield);

    void computeSolution();

    const QHash<Coordinate, double> &getChancesToBeMine() const;

    void setLogProgress(bool newLogProgress);

    void cancel();

private:
    CoordVector path;
    QList<QSharedPointer<ChoiceColumn>> choiceColumns;

    QHash<Coordinate, double> chancesToBeMine;

    void decidePath();
    void buildSolutionGraph();
    void analyzeSolutionGraph();

    QSharedPointer<Minefield const> minefield;

    bool logProgress = false;

    bool cancelled = false;
};

#endif // SOLVER_H
