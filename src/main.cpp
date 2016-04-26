#include <cstdlib>
#include <cmath>
#include <functional>
#include <iostream>
#include <ctime>

#include "UniformSampling.h"
#include "ImportanceSampling.h"

using namespace std;

int main () {

    MonteCarloMethod::Func g = [](double x) {
        return (25 + x * (x - 6) * (x - 8) * (x - 14) / 25) * exp(sqrt(1 + cos(x*x / 10)));
    };

    seed_seq seed = {24, 512, 42};

    {
        // 601.989
        UniformSampling us(g);
        us.setSeed(seed);

        clock_t start = clock();
        MonteCarloMethod::Sampling s = us.sample(100000);

        cout << s.areaEstimator << ", " << s.confidenceInterval.toString() << endl;
        cout << (double) (clock() - start) / CLOCKS_PER_SEC << "s" << endl;
    }

    MonteCarloMethod::Func f = [](double x){
        return 60;
    };

    Points points = Stats::createPoints(30, g, 0, 15);

    {
        ImportanceSampling is(g);
        is.setSeed(seed);

        clock_t start = clock();
        MonteCarloMethod::Sampling s = is.sample(100000, f, points.xs, points.ys);

        cout << s.areaEstimator << ", " << s.confidenceInterval.toString() << endl;
        cout << (double) (clock() - start) / CLOCKS_PER_SEC << " s" << endl;
    }


    return EXIT_SUCCESS;
}
