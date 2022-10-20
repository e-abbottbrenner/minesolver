#include "SolverMath.h"

using boost::multiprecision::cpp_int;

namespace SolverMath
{
cpp_int factorial(int n)
{
    cpp_int result = 1;

    for(int i = 1; i <= n; ++i)
    {
        result *= i;
    }

    return result;
}

SolverFloat choose(int n, int k)
{
    if(n < k)
    {
        return 0;
    }

    // standard formula for choose
    return (factorial(n) / (factorial(k) * factorial(n - k))).convert_to<SolverFloat>();
}

}
