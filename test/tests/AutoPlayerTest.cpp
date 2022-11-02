#include <gtest/gtest.h>

#include "AutoPlayer.h"
#include "Minefield.h"

#include <QEventLoop>
#include <QRandomGenerator>

class AutoPlayerTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        minefield.clear();
    }

    void setupMinefieldExpert()
    {
        minefield = minefield.create(99, 30, 16, QRandomGenerator::global()->generate());
    }

    void playAGame(int& winCount)
    {
        setupMinefieldExpert();

        AutoPlayer autoPlayer(minefield);

        autoPlayer.setAutoSolve(true);

        QEventLoop gameLoop;

        QObject::connect(minefield.data(), &Minefield::mineHit, &gameLoop, &QEventLoop::quit);
        QObject::connect(minefield.data(), &Minefield::allCountCellsRevealed, &gameLoop, &QEventLoop::quit);

        gameLoop.exec();

        if(!minefield->wasMineHit())
        {// game finished and wasn't a loss
            ++winCount;
        }
    }

    QSharedPointer<Minefield> minefield;
};

TEST_F(AutoPlayerTest, testWinRate)
{
    int winCount = 0;

    const int ITERATIONS = 10000;

    int lastPercentCompletion = 0;

    int gamesPlayed = 0;

    for(; gamesPlayed < ITERATIONS; ++gamesPlayed)
    {
        playAGame(winCount);

        int percentCompletion = gamesPlayed / (ITERATIONS / 100.0);

        if(percentCompletion > lastPercentCompletion)
        {
            std::cout << percentCompletion << "% Complete." << std::endl;
            lastPercentCompletion = percentCompletion;
        }
    }

    std::cout << "Won " << 100.0 * winCount / gamesPlayed
              << " percent of " << gamesPlayed << " games." << std::endl;
}
