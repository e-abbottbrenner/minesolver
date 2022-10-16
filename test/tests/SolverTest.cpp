#include <gtest/gtest.h>

#include "ChoiceColumn.h"
#include "ChoiceNode.h"
#include "Minefield.h"
#include "Solver.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QRandomGenerator>

class SolverTest : public ::testing::Test
{
protected:
    void resetMinefield(int seed)
    {
        minefield = minefield.create(120, 20, 20, seed);
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

    void revealAndTest()
    {
        QList<Coordinate> clearCoords;

        for(int x = 0; x < minefield->getWidth(); x++)
        {
            for(int y = 0; y < minefield->getHeight(); ++y)
            {
                if(minefield->getUnderlyingCell(x, y) != SpecialStatus::Mine)
                {
                    clearCoords.append({x, y});
                }
            }
        }

        int loops = QRandomGenerator::global()->bounded(1, 10);

        for(int i = 1; i <= loops; ++i)
        {// reveal some random clear cells to start, this should create more variety in the states than just revealing one
            Coordinate coord = clearCoords.takeAt(QRandomGenerator::global()->bounded(clearCoords.size()));
            minefield->revealCell(coord.first, coord.second);
        }

        solver->computeSolution();

        logProbabilityBuckets();
    }

    void evaluateProbabilityBuckets()
    {
        std::cout << std::endl;

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

                double difference = std::abs(percentage - averageBucketPercentage);
                bool withinTolerance = difference <= tolerance;

                std::cout << "Probability bucket " << bucket << " {" << std::endl;
                std::cout << "Average bucket percentage: " << averageBucketPercentage << std::endl;
                std::cout << "Sampled percentage: " << percentage << std::endl;
                std::cout << "Difference: " << difference << std::endl;
                std::cout << "Sample size: " << (mineCount + clearCount) << std::endl;
                std::cout << "Tolerance: " << tolerance << std::endl;
                std::cout << "Within tolerance: " << withinTolerance << std::endl;
                std::cout << "}" << std::endl << std::endl;

                EXPECT_TRUE(withinTolerance);
            }
        }
    }

    void testSolverProbabilities(const int ITERATIONS = 1000)
    {
        int lastPercentCompletion = 0;

        QElapsedTimer timeSpent;

        timeSpent.start();

        for(int i = 0; i < ITERATIONS; ++i)
        {
            resetMinefield(QRandomGenerator::global()->generate());

            // play 100 random games
            revealAndTest();

            solver.clear();

            // verify the solver destructor deconstructs all its data structures
            ASSERT_EQ(0, ChoiceNode::getChoiceNodesConstructed());
            ASSERT_EQ(0, ChoiceColumn::getChoiceColumnsConstructed());

            int percentCompletion = i / (ITERATIONS / 100);

            if(percentCompletion != lastPercentCompletion)
            {
                qint64 elapsedMS = timeSpent.elapsed();
                double velocityMS = static_cast<double>(i) / elapsedMS;

                int remainingMS = (ITERATIONS - i) / velocityMS;
                int remainingSeconds = remainingMS / 1000;
                int remainingMinutes = remainingSeconds / 60;

                std::cout << percentCompletion << "% Complete. Estimated time to completion is " << remainingMinutes << " minutes and " << (remainingSeconds % 60) << " seconds." << std::endl;
                lastPercentCompletion = percentCompletion;
            }
        }

        // evaluate the bucket results
        evaluateProbabilityBuckets();
    }

    QHash<int, int> probabilityBucketsSampledAsMine;
    QHash<int, int> probabilityBucketsSampledAsClear;
    QHash<int, double> probabilityBucketAverageMineChance;

    QSharedPointer<Solver> solver;
    QSharedPointer<Minefield> minefield;
};

TEST_F(SolverTest, testSolverProbabilitiesQuick)
{
    testSolverProbabilities(1000);
}

TEST_F(SolverTest, testSolverProbabilitiesLong)
{
    testSolverProbabilities(100000);
}
