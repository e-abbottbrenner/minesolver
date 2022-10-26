#ifndef SOLVER_H
#define SOLVER_H

#include "ChoiceColumn.h"
#include "SolverMinefield.h"

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
    Solver(QSharedPointer<Minefield const> gameMinefield, QHash<Coordinate, double> previousMineChances = {});
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
    // the tail path is all the unknown cells that have no adjacent count cells, these can be solved with math formulas instead of algorithmic analysis
    CoordVector tailPath;
    QList<QSharedPointer<ChoiceColumn>> choiceColumns;

    QHash<Coordinate, double> chancesToBeMine;
    QHash<Coordinate, int> columnCounts;
    SolverFloat legalFieldCount;

    void flagObviousCells();
    void decidePath();
    void buildSolutionGraph();
    void analyzeSolutionGraph();

    SolverMinefield startingMinefield;

    bool logProgress = false;

    bool cancelled = false;

    int mineCount = 0;

    bool minefieldPopulated = true;

    QSharedPointer<ProgressProxy> progress;

    void prepareStartingMinefield(const QHash<Coordinate, double> &previousMineChances);
};

#endif // SOLVER_H
