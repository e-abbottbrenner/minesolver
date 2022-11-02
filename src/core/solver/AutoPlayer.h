#ifndef AUTOPLAYER_H
#define AUTOPLAYER_H

#include <QObject>

#include <QFutureWatcher>
#include <QMutex>
#include <QSharedPointer>

class Minefield;
class Solver;

typedef QPair<int, int> Coordinate;

class AutoPlayer : public QObject
{
    Q_OBJECT

public:
    explicit AutoPlayer(QSharedPointer<Minefield> minefield);

    void queueStep();
    void queueCalculate();

    void step();
    void calculate();

    void setAutoSolve(bool newAutoSolve);
    bool getAutoSolve() const;

    double getBestMineChance() const;

signals:
    void maxProgressChanged(int max);
    void currentProgressChanged(int progress);
    void progressStepChanged(const QString& step);

    void riskedLoss(double risk);

    void calculationStarted();
    void calculationComplete(QSharedPointer<Solver> finishedSolver);

private:
    bool autoSolve = false;

    QSharedPointer<Minefield> minefield;

    QSharedPointer<Solver> activeSolver;
    QSharedPointer<Solver> finishedSolver;

    QSharedPointer<QFutureWatcher<void>> mineChancesCalculationWatcher;

    QList<QMetaObject::Connection> recalcProgressConnections;

    double bestMineChance = 0;

    bool calculationPending = false;

    QMutex writeMutex;
    QMutex queueMutex;
    void onCalculationsComplete();
    void setActiveSolver(QSharedPointer<Solver> solver);
    void revealLowestRiskCells();
    QList<Coordinate> getOptimalCells() const;
    void reveal(int x, int y);
};

#endif // AUTOPLAYER_H
