#ifndef CHOICECOLUMN_H
#define CHOICECOLUMN_H

#include <QByteArray>
#include <QHash>
#include <QSharedPointer>

#include <boost/multiprecision/cpp_int.hpp>
using boost::multiprecision::uint512_t;

class ChoiceNode;

class ChoiceColumn
{
public:
    ChoiceColumn(int x, int y);

    QSharedPointer<ChoiceNode> getChoiceNode(const QByteArray& minefieldBytes) const;
    void addChoiceNode(QSharedPointer<ChoiceNode> node);

    QList<QSharedPointer<ChoiceNode>> getChoiceNodes() const;

    int getX() const;
    int getY() const;

    void precomputePathsForward(int mineCount);
    void precomputePathsBack(int mineCount);

    void calculateWaysToBe(int mineCount);

    double getPercentChanceToBeMine() const;
    uint512_t getWaysToBeMine() const;
    uint512_t getWaysToBeClear() const;

private:
    QHash<QByteArray, QSharedPointer<ChoiceNode>> choicesInColumn;

    int x = 0;
    int y = 0;

    uint512_t waysToBeMine = 0;
    uint512_t waysToBeClear = 0;
};

#endif // CHOICECOLUMN_H
