#ifndef CHOICECOLUMN_H
#define CHOICECOLUMN_H

#include <QByteArray>
#include <QHash>
#include <QSharedPointer>

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::cpp_int;

class ChoiceNode;

class ChoiceColumn
{
public:
    ChoiceColumn(int x, int y);
    virtual ~ChoiceColumn();

    QSharedPointer<ChoiceNode> getChoiceNode(const QByteArray& minefieldBytes) const;
    void addChoiceNode(QSharedPointer<ChoiceNode> node);

    QList<QSharedPointer<ChoiceNode>> getChoiceNodes() const;

    int getX() const;
    int getY() const;

    void precomputePathsForward(int mineCount);
    void precomputePathsReverse(int mineCount);
    void calculateWaysToBe(int mineCount);

    double getPercentChanceToBeMine() const;

    cpp_int getWaysToBeMine() const;

    cpp_int getWaysToBeClear() const;

    static int getChoiceColumnsConstructed();

private:
    QHash<QByteArray, QSharedPointer<ChoiceNode>> choicesInColumn;

    int x = 0;
    int y = 0;

    cpp_int waysToBeMine = 0;
    cpp_int waysToBeClear = 0;

    static int choiceColumnsConstructed;
};

#endif // CHOICECOLUMN_H
