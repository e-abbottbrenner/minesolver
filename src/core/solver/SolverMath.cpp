#include "SolverMath.h"

#include <QHash>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

using boost::multiprecision::cpp_int;

namespace SolverMath
{
SolverFloat choose(int n, int k)
{
    static QMutex chooseMutex;
    QMutexLocker locker(&chooseMutex);

    static QHash<int, QList<SolverFloat>> resultsHash;

    if(!resultsHash.contains(n))
    {
        // start with the value of n choose 0
        resultsHash[n] = {1};
    }

    auto newResults = resultsHash[n];

    if(n < k || k < 0)
    {
        return 0;
    }

    int startingIndex = std::min(static_cast<int>(newResults.size()) - 1, k);
    SolverFloat nchoosek = newResults[startingIndex];

    for(int i = startingIndex + 1; i <= k; ++i)
    {
        nchoosek *= (n + 1 - i) / static_cast<SolverFloat>(i);
        newResults.append(nchoosek);
    }

    resultsHash[n] = newResults;

    return nchoosek;
}

}
