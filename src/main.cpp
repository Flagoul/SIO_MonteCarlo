#include <cstdlib>
#include <cmath>
#include <functional>
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>

#include "montecarlo/UniformSampling.h"
#include "montecarlo/ImportanceSampling.h"
#include "montecarlo/ControlVariableMethod.h"

using namespace std;

void printSampling(const MonteCarloMethod::Sampling& s, const string& name, double timeTaken) {
    cout << "-- " << name << " --" << endl;
    cout << " N. de generations : " << s.N << endl;
    cout << " Aire estimee      : " << s.areaEstimator << endl;
    cout << " IC                : " << s.confidenceInterval << endl;
    cout << " Largeur de l'IC   : " << s.confidenceInterval.delta << endl;
    cout << " Temps d'execution : " << timeTaken << "s" << endl;
    cout << endl;
}

int main () {

    // fonction dont on veut estimer l'aire
    MonteCarloMethod::Func g = [](double x) {
        return (25 + x * (x - 6) * (x - 8) * (x - 14) / 25) * exp(sqrt(1 + cos(x*x / 10)));
    };

    // borne inferieure et superieure
    double a = 0, b = 15;

    // M : nombre de valeurs dans l'echantillon, N : nombre de valeurs totales a generer
    size_t M = 10000, N = 1000000;

    // nombre de points a utiliser pour la fonction affine par morceaux
    size_t numPointsPWLFunc = 15;

    // largeur max de l'IC à 95%
    double deltaMax = 0.1;

    // temps maximum alloue pour chaque methode (en secondes)
    double maxTime = 2;

    // nombre d'étapes a faire dans le cas de generations avec largeur d'IC ou temps limite
    size_t step = 100000;

    // graine utilisee pour les generateurs
    seed_seq seed = {24, 512, 42};

    cout << endl;

    {
        UniformSampling us(g, a, b);
        us.setSeed(seed);

        clock_t start;

        stringstream name;
        name << "Echantillonage uniforme - largeur max de l'IC fixee (" << deltaMax << ")";
        start = clock();
        MonteCarloMethod::Sampling s = us.sampleWithMaxDelta(deltaMax, step);
        printSampling(s, name.str(), (double)(clock() - start) / CLOCKS_PER_SEC);

        name.str(string());
        name << "Echantillonage uniforme - temps max fixe (" << maxTime << " s)";
        start = clock();
        s = us.sampleWithMaxTime(maxTime, step);
        printSampling(s, name.str(), (double)(clock() - start) / CLOCKS_PER_SEC);

    }

    // creation des points de la fonction affine par morceaux
    Points points = Stats::createPoints(numPointsPWLFunc, g, a, b);

    {
        ImportanceSampling is(g, points.xs, points.ys);
        is.setSeed(seed);

        clock_t start;

        stringstream name;
        name << "Echantillonage preferentiel - largeur max de l'IC fixee (" << deltaMax << ")";
        start = clock();
        MonteCarloMethod::Sampling s = is.sampleWithMaxDelta(deltaMax, step);
        printSampling(s, name.str(), (double)(clock() - start) / CLOCKS_PER_SEC);

        name.str(string());
        name << "Echantillonage preferentiel - temps max fixe (" << maxTime << " s)";
        start = clock();
        s = is.sampleWithMaxTime(maxTime, step);
        printSampling(s, name.str(), (double)(clock() - start) / CLOCKS_PER_SEC);
    }

    {
        cout << "Avec M = " << M << " :" << endl;
        ControlVariable cv(g, a, b, points.xs, points.ys);
        cv.setSeed(seed);

        clock_t start;

        stringstream name;
        name << "Methode de la variable de controle - largeur max de l'IC fixee (" << deltaMax << ")";
        start = clock();
        MonteCarloMethod::Sampling s = cv.sampleWithMaxDelta(M, deltaMax, step);
        printSampling(s, name.str(), (double)(clock() - start) / CLOCKS_PER_SEC);

        name.str(string());
        name << "Methode de la variable de controle - temps max fixe (" << maxTime << " s)";
        start = clock();
        s = cv.sampleWithMaxTime(M, maxTime, step);
        printSampling(s, name.str(), (double)(clock() - start) / CLOCKS_PER_SEC);
    }


    return EXIT_SUCCESS;
}
