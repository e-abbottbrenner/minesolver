#include "RandomNumbers.h"

RandomNumbers::RandomNumbers(int seed)
{
    // this is a wrapper around the standard C++ srand.  The standard random number generator is probably good enough for this
    // we can switch to a mersenne twister or someother fancy RNG later if we need to.
    generator.seed(seed);
}

int RandomNumbers::nextRand(int range)
{
    int value = generator.bounded(range);

    return value;
}
