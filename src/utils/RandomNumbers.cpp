#include "RandomNumbers.h"

#include <QtGlobal>

RandomNumbers::RandomNumbers(int seed)
{
    // this is a wrapper around the standard C++ srand.  The standard random number generator is probably good enough for this
    // we can switch to a mersenne twister or someother fancy RNG later if we need to.
    qsrand(seed);
}

int RandomNumbers::nextRand(int range)
{
    int value = qrand();

    // add 1 so max is inclusive
    value = value % range;

    return value;
}
