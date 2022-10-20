#include "SolverMath.h"

#include <QList>
#include <QMutex>
#include <QMutexLocker>

using boost::multiprecision::cpp_int;

namespace SolverMath
{
cpp_int factorial(int n)
{
    static QMutex factorialMutex;

    QMutexLocker factorialLocker(&factorialMutex);

    static QList<cpp_int> previousFactorials = {1};

    cpp_int result = previousFactorials[std::min(n, static_cast<int>(previousFactorials.size() - 1))];

    for(int i = previousFactorials.size(); i <= n; ++i)
    {
        result *= i;
        previousFactorials.append(result);
    }

    return result;
}

SolverFloat choose(int n, int k)
{
    if(n < k || k < 0)
    {
        return 0;
    }

    // standard formula for choose
    return (factorial(n) / (factorial(k) * factorial(n - k))).convert_to<SolverFloat>();
}

}
