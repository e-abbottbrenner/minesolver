#include <gtest/gtest.h>

#include "ChoiceColumn.h"
#include "ChoiceNode.h"
#include "Minefield.h"
#include "Solver.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QRandomGenerator>
#include <QtConcurrent/QtConcurrent>

class SolverTest : public ::testing::Test
{
protected:
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

    void logProbabilityBuckets(QSharedPointer<Solver> solver, QSharedPointer<Minefield> minefield)
    {
        auto mineChances = solver->getChancesToBeMine();

        auto coords = mineChances.keys();

        QMutexLocker probabilityLocker(&probabilityMutex);

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

    void analyzeNewMinefield(int seed)
    {
        QSharedPointer<Minefield> minefield(new Minefield(120, 20, 20, seed));
        QSharedPointer<Solver> solver(new Solver(minefield.data()));
        solver->setLogProgress(false);

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

        logProbabilityBuckets(solver, minefield);
    }

    void evaluateProbabilityBuckets()
    {
        std::cout << std::endl;

        int totalOutOfTolerance = 0;

        for(int bucket = 0; bucket <= 100; bucket += 5)
        {// for each bucket
            int mineCount = probabilityBucketsSampledAsMine[bucket];
            int clearCount = probabilityBucketsSampledAsClear[bucket];

            // simple margin of error formula for 99.99% confidence taken from online sources
            double tolerance = 194.5 / std::sqrt(mineCount + clearCount);

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

                if(!withinTolerance)
                {
                    totalOutOfTolerance++;
                }
            }
        }

        EXPECT_LT(totalOutOfTolerance, 2) << "Too many buckets out of tolerance. This many should only happen in 1/10000 runs";
    }

    void testSolverProbabilities(const int ITERATIONS = 1000)
    {
        QMutex outputMutex;

        int lastPercentCompletion = 0;

        QElapsedTimer timeSpent;

        timeSpent.start();

        QFutureSynchronizer<void> synchronizer;

        for(int i = 0; i < ITERATIONS; ++i)
        {
            QFuture<void> analyzeFuture = QtConcurrent::run([this, i, ITERATIONS, &timeSpent, &lastPercentCompletion, &outputMutex] ()
            {
                analyzeNewMinefield(QRandomGenerator::global()->generate());

                int percentCompletion = i / (ITERATIONS / 100);

                // don't have race conditions aruond the percent completion output
                QMutexLocker outputLocker(&outputMutex);

                if(percentCompletion > lastPercentCompletion)
                {
                    qint64 elapsedMS = timeSpent.elapsed();
                    double velocityMS = static_cast<double>(i) / elapsedMS;

                    int remainingMS = (ITERATIONS - i) / velocityMS;
                    int remainingSeconds = remainingMS / 1000;
                    int remainingMinutes = remainingSeconds / 60;

                    std::cout << percentCompletion << "% Complete. Estimated time to completion is " << remainingMinutes << " minutes and " << (remainingSeconds % 60) << " seconds." << std::endl;
                    lastPercentCompletion = percentCompletion;
                }
            });

            synchronizer.addFuture(analyzeFuture);
        }

        synchronizer.waitForFinished();

        std::cout << "100% Complete." << std::endl;

        // evaluate the bucket results
        evaluateProbabilityBuckets();
    }

    QHash<int, int> probabilityBucketsSampledAsMine;
    QHash<int, int> probabilityBucketsSampledAsClear;
    QHash<int, double> probabilityBucketAverageMineChance;

    QMutex probabilityMutex;
};

TEST_F(SolverTest, testSolverProbabilitiesQuick)
{
    testSolverProbabilities(1000);
}

TEST_F(SolverTest, testSolverProbabilitiesLong)
{
    testSolverProbabilities(100000);
}
