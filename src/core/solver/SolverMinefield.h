#ifndef SOLVERMINEFIELD_H
#define SOLVERMINEFIELD_H

#include "MineStatus.h"
#include "TraversableGrid.h"

#include <QByteArray>
#include <QString>

// represents a potential minefield in the set of all possible states
class SolverMinefield : public TraversableGrid
{
public:
    SolverMinefield(QByteArray minefield, int width, int height);

    SolverMinefield chooseMine(int x, int y) const;
    SolverMinefield chooseClear(int x, int y) const;

    bool isLegal() const;

    QString toString() const;

    MineStatus getCell(int x, int y) const;

    const QByteArray &getMinefieldBytes() const;

    int countAdjacentUnknowns(int x, int y) const;

private:
    void validateMinefield();
    SolverMinefield chooseCellState(int x, int y, bool mine) const;

    bool legal = true;

    QByteArray minefieldBytes;
};

#endif // SOLVERMINEFIELD_H
