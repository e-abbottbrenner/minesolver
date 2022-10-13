#ifndef POTENTIALMINEFIELD_H
#define POTENTIALMINEFIELD_H

#include "TraversableGrid.h"

#include <QByteArray>

// represents a potential minefield in the set of all possible states
class PotentialMinefield : public TraversableGrid
{
public:
    PotentialMinefield(QByteArray minefield, int width, int height);

    PotentialMinefield chooseMine(int x, int y) const;
    PotentialMinefield chooseClear(int x, int y) const;

    bool isLegal() const;

    const QByteArray &getMinefieldBytes() const;

private:
    bool legal = true;

    QByteArray minefieldBytes;
};

#endif // POTENTIALMINEFIELD_H
