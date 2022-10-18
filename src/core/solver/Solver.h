#ifndef SOLVER_H
#define SOLVER_H

#include "ChoiceColumn.h"

#include <QList>
#include <QHash>
#include <QPair>
#include <QSharedPointer>
#include <QVector>
#include <QObject>

typedef QPair<int, int> Coordinate;
typedef QVector<Coordinate> CoordVector;

class ChoiceColumn;
class Minefield;
class ProgressProxy;

class Solver
{
public:
    explicit Solver(QSharedPointer<Minefield const> minefield);
    ~Solver();

    void computeSolution();

    const QHash<Coordinate, double> &getChancesToBeMine() const;
    const QHash<Coordinate, int> &getColumnCounts() const;
    int getLogLegalFieldCount() const;

    int getPathIndex(const Coordinate& coord) const;

    void setLogProgress(bool newLogProgress);

    void cancel();

    QSharedPointer<ProgressProxy> getProgress() const;

private:
    CoordVector path;
    QList<QSharedPointer<ChoiceColumn>> choiceColumns;

    QHash<Coordinate, double> chancesToBeMine;
    QHash<Coordinate, int> columnCounts;
    double legalFieldCount;

    void decidePath();
    void buildSolutionGraph();
    void analyzeSolutionGraph();

    QSharedPointer<Minefield const> minefield;

    bool logProgress = false;

    bool cancelled = false;

    QSharedPointer<ProgressProxy> progress;
};

#endif // SOLVER_H
