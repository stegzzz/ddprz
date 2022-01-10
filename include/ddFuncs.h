#pragma once
#include "ddgemModel.h"

namespace spgxyz {

//*************START Least squares functions*************//
/**
 * @brief hyperbolic delay discounting function, used by and uses ddgemModel
 *
 * @param double k -- discounting parameter
 * @param void *m -- ptr to ddgemModel, used to get delays, values, and
 * undiscounted amount
 * @return double -- sum of square deviations between model values and given
 * values
 */
double hdd(double const k, void *const m);
/**
 * @brief exponential delay discounting function, used by and uses ddgemModel
 *
 * @see double hdd(double k, void *m)
 */
double edd(double const k, void *const m);

//*************END Least squares functions*************//

/**
 * @brief simple hyperbolic delay discounting
 *
 * @param double const a   --undiscounted amount
 * @param double const k   --discounting parameter
 * @param double const d   --delay
 *
 * @return double --delay discounted value
 */
double shdd(double const a, double const k, double const d);

/**
 * @brief simple exponential delay discounting
 *
 * @see shdd(double a, double k, double d)
 */
double sedd(double const a, double const k, double const d);

using ddfptr =
    double (*)(double const, double const,
               double const); /*!< type for simple delay discounting function */

/**
 * @brief hyperbolic LD eq11
 *
 * @param double K - large reward K
 * @param double x - multiplier a=xA
 * @param double d - small reward delay
 * @return double - lower bound on preference reversal zone
 */
double LDh(double const K, double const x = 0.82, double const d = 24);
/**
 * @brief hyperbolic UD eq12
 *
 * @param double k - small reward k
 * @param double K - large reward K
 * @param double x - multiplier a=xA
 * @param double d - small reward delay
 * @return double - upper bound on preference reversal zone
 */
double UDh(double const k, double const K, double const x = 0.82,
           double const d = 24);
/**
 * @brief exponential LD eq13
 *
 * @param double K - large reward K
 * @param double x - multiplier a=xA
 * @param double d - small reward delay
 * @return double - lower bound on preference reversal zone
 */
double LDe(double const K, double const x = 0.82, double const d = 24);
/**
 * @brief exponential UD eq14
 *
 * @param double k - small reward k
 * @param double K - large reward K
 * @param double x - multiplier a=xA
 * @param double d - small reward delay
 * @return double - upper bound on preference reversal zone
 */
double UDe(double const k, double const K, double const x = 0.82,
           double const d = 24);

using lfptr = double (*)(double const, double const,
                         double const); /*!< type for lower bound function */
using ufptr = double (*)(double const, double const, double const,
                         double const); /*!< type for upper bound function */
} // namespace spgxyz