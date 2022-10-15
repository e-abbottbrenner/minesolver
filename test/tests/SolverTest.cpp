#include <gtest/gtest.h>

#include "Minefield.h"
#include "Solver.h"

#include <QDebug>
#include <QRandomGenerator>

class SolverTest : public ::testing::Test
{
protected:
    void resetMinefield(int seed)
    {
        minefield = minefield.create(100, 30, 30, seed);
        solver = solver.create(minefield.data());
        solver->setLogProgress(false);
    }

    int probabilityBucket(double probability) const
    {
        for(int i = 0; i <= 100; i += 5)
        {
            if(std::abs(probability * 100 - i) <= 2.5)
            {
                return i;
            }
        }

        ADD_FAILURE() << "Bucket not found for" << probability;
        return 0;
    }

    void logProbabilityBuckets()
    {
        auto mineChances = solver->getChancesToBeMine();

        auto coords = mineChances.keys();
        for(Coordinate coord : coords)
        {
            double probability = mineChances[coord];

            MineStatus status = minefield->getUnderlyingCell(coord.first, coord.second);

            int bucket = probabilityBucket(probability);

            if(status == SpecialStatus::Mine)
            {
                ASSERT_NE(0, probability) << "mine found when predicted chance was 0";

                probabilityBucketsSampledAsMine.insert(bucket, probabilityBucketsSampledAsMine[bucket] + 1);
            }
            else
            {
                ASSERT_NE(1, probability) << "no mine found when predicted chance was 1";

                probabilityBucketsSampledAsClear.insert(bucket, probabilityBucketsSampledAsClear[bucket] + 1);
            }

            double currentAverage = probabilityBucketAverageMineChance.value(bucket, 0);
            int newAverageDenominator = probabilityBucketsSampledAsClear[bucket] + probabilityBucketsSampledAsMine[bucket];

            // calculate the new average
            probabilityBucketAverageMineChance.insert(bucket, ((currentAverage * (newAverageDenominator - 1)) + probability) / newAverageDenominator);
        }
    }

    void playField()
    {
        minefield->revealCell(QRandomGenerator::global()->bounded(minefield->getWidth()),
                              QRandomGenerator::global()->bounded(minefield->getHeight()));

        solver->computeSolution();
        auto mineChances = solver->getChancesToBeMine();

        // when the size is the same as the number of mines that means it's nothing but mines
        while(mineChances.size() > minefield->getNumMines())
        {
            // log the probabilities
            logProbabilityBuckets();

            bool revealedSomething = false;

            // click on something that isn't a mine
            auto coords = mineChances.keys();
            for(Coordinate coord : coords)
            {
                if(mineChances[coord] == 0)
                {// reveal all of the cells with no chance of being a mine
                    minefield->revealCell(coord.first, coord.second);
                    revealedSomething = true;
                }
            }

            if(!revealedSomething)
            { // need to progress somehow, just pick the first option that isn't a mine
                for(Coordinate coord: coords)
                {
                    // cheat so we can progress through the whole board
                    if(minefield->getUnderlyingCell(coord.first, coord.second) != SpecialStatus::Mine)
                    {
                        qDebug() << "cheating to reveal cell" << coord;
                        minefield->revealCell(coord.first, coord.second);
                        break;
                    }
                }
            }

            // compute again
            solver->computeSolution();
            mineChances = solver->getChancesToBeMine();
        }
    }

    void evaluateProbabilityBuckets()
    {
        for(int bucket = 0; bucket <= 100; bucket += 5)
        {// for each bucket
            int mineCount = probabilityBucketsSampledAsMine[bucket];
            int clearCount = probabilityBucketsSampledAsClear[bucket];

            // simple margin of error formula for 99% confidence taken from online sources
            double tolerance = 128.8 / std::sqrt(mineCount + clearCount);

            if(mineCount > 0 || clearCount > 0)
            {
                double percentage = 100.0 * mineCount / (mineCount + clearCount);
                double averageBucketPercentage = 100.0 * probabilityBucketAverageMineChance[bucket];

                EXPECT_NEAR(averageBucketPercentage, percentage, tolerance)
                        << "probability bucket " << bucket << " inconsistent with results "
                        << mineCount << " mines and " << clearCount << " clear."
                        << " Percentage is " << percentage << " vs " << averageBucketPercentage;
            }
        }
    }

    QHash<int, int> probabilityBucketsSampledAsMine;
    QHash<int, int> probabilityBucketsSampledAsClear;
    QHash<int, double> probabilityBucketAverageMineChance;

    QSharedPointer<Solver> solver;
    QSharedPointer<Minefield> minefield;
};

TEST_F(SolverTest, testSolverProbabilities)
{
    for(int i = 0; i < 1000; ++i)
    {
        std::cout << "playing game " << i << std::endl;

        resetMinefield(QRandomGenerator::global()->bounded(1000));

        // play 100 random games
        playField();
    }

    // evaluate the bucket results
    evaluateProbabilityBuckets();
}
