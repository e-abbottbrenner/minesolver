#ifndef SOLVERFLOAT_H
#define SOLVERFLOAT_H

#include <boost/multiprecision/cpp_bin_float.hpp>

namespace SolverMath
{
    using namespace boost::multiprecision;
    // for the solver, we don't need floats below 1
    // we also need a lot of exponent for larger boards
    // we also don't need a lot of digits
    // so this configuration gives us plenty of everything we need and is fast too
    typedef number<backends::cpp_bin_float<32, backends::digit_base_2, void, std::int32_t, 0, 524285>, et_off> SolverFloat;
}

typedef SolverMath::SolverFloat SolverFloat;

#endif // SOLVERFLOAT_H
