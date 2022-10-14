#ifndef RANDOMNUMBERS_H
#define RANDOMNUMBERS_H

#include <QRandomGenerator>

class RandomNumbers
{
public:
    // if we use a fancier method of generating the numbers, we might need to keep data so this is a class
    RandomNumbers(int seed);

    int nextRand(int range = 2);

private:
    QRandomGenerator generator;
};

#endif // RANDOMNUMBERS_H