#ifndef PIECEWISE_LINEAR_FUNCTION_H
#define PIECEWISE_LINEAR_FUNCTION_H

#include <vector>
#include <functional>
#include <cstdint>

/**
 * Regroupe les informations du "morceau" d'une fonction affine par morceaux.
 */
struct Piece {

    // fonction lineaire prenant un double et renvoyant un double
    typedef std::function<double(double)> LinearFunc;

    double x0, x1;    // bornes de l'intervalle definissant le morceau
    double y0, y1;    // ordonnees correspodant aux bornes
    double A_k;       // aire sous la fonction f_k
    LinearFunc f_k;   // fonction associée à ce morceau k.
};

struct PiecewiseLinearFunction {
    std::vector<Piece> pieces;   // "morceaux" de la fonction
    double A = 0;                // aire totale sous la fonction

    PiecewiseLinearFunction(const std::vector<double>& xs, const std::vector<double>& ys);

    /**
     * Recherche dichotomique afin de trouver dans quel intervalle x se trouve.
     *
     * @param x l'abscisse dont on veut connaître l'intervalle.
     * @return l'indice du morceau dans lequel x se trouve.
     */
    uint64_t findPiece(double x) const;

    /**
     * Simplifie l'ecriture de l'application de la fonction affine par morceau sur une valeur donnee.
     * Effectue un appel a findPiece en interne.
     *
     * @param x L'abscisse dont on veut connaitre l'ordonnee.
     * @return l'ordonnee.
     */
    double operator()(double x) const;
};

#endif // PIECEWISE_LINEAR_FUNCTION_H
