#ifndef SOLVER_H
#define SOLVER_H

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

class Solver : public QObject
{
    Q_OBJECT

public:
    explicit Solver(QSharedPointer<Minefield const> minefield);

    void computeSolution();

    const QHash<Coordinate, double> &getChancesToBeMine() const;

    void setLogProgress(bool newLogProgress);

    void cancel();

signals:
    void progressMaximum(int max);
    void progressMade(int progress);
    void progressStep(const QString& progressStep);

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

    int progress = 0;
};

#endif // SOLVER_H
