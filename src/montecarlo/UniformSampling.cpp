#include <ctime>

#include <vector>
#include "UniformSampling.h"

UniformSampling::UniformSampling(const MonteCarloMethod::Func& g, double a, double b)
        :
        MonteCarloMethod(g), a(a), b(b),
        distribution(std::uniform_real_distribution<double>(0, 1))
{
    if (b <= a) {
        throw std::invalid_argument("b doit etre plus grand que a");
    }
}

MonteCarloMethod::Sampling UniformSampling::sampleWithSize(size_t N) {
    init();

    sample(N);
    return createSampling((double)(clock() - start) / CLOCKS_PER_SEC);
}

MonteCarloMethod::Sampling UniformSampling::sampleWithMaxWidth(double maxWidth, size_t step) {
    init();

    do {
        sample(step);
    } while (halfWidth * 2 > maxWidth);

    return createSampling((double)(clock() - start) / CLOCKS_PER_SEC);
}

MonteCarloMethod::Sampling UniformSampling::sampleWithMinTime(double maxTime, size_t step) {
    init();
    double curTime = 0;

    do {
        clock_t beg = clock();
        sample(step);
        curTime += (double)(clock() - beg) / CLOCKS_PER_SEC;
    } while (curTime < maxTime);

    return createSampling(curTime);
}

void UniformSampling::setSeed(const std::seed_seq &seed) {
    std::seed_seq copy = seed;
    generator.seed(copy);
}

void UniformSampling::sample(size_t step) {
    for (size_t i = 0; i < step; ++i) {
        double X = distribution(generator) * (b - a) + a;
        double Y = g(X);

        sum += Y;
        sumSquares += Y * Y;
    }

    numGen += step;

    mean = sum / numGen;
    double var = (sumSquares / numGen) - mean * mean;
    stdDev = (b-a) * sqrt(var / numGen);
    halfWidth = 1.96 * (b - a) * sqrt(var / numGen);
}

MonteCarloMethod::Sampling UniformSampling::createSampling(double timeElapsed) const {
    double areaEstimator = (b-a) * mean;
    return {areaEstimator, stdDev, ConfidenceInterval(areaEstimator, halfWidth), numGen, timeElapsed};
}