#pragma once
#include "ddgemModelNLopt.h"
#include "mlk.h"
namespace spgxyz {
//*************START Maximum likelihood functions*************//
/**
 * @brief hyperbolic delay discounting function, used by and uses
 * ddgemModelNLopt
 *
 * @param double k -- discounting parameter
 * @param double g -- choice difference sensitivity parameter Cavagnaro et al
 * (2016)
 * @param void *m -- ptr to ddgemModelNLopt, used to get delays, values,
 * undiscounted amount, and responses
 * @return double -- sum of negative LL values for observed responses
 */
double hdd_ml(double const k, double const g, void *const m);
/**
 * @brief exponential delay discounting function, used by and uses ddgemModel
 *
 * @see double hdd_ml(double k, void *m)
 */
double edd_ml(double const k, double const g, void *const m);

/**
 * @brief p(smaller soon chosen)
 *
 * @param double g -- choice difference sensitivity parameter Cavagnaro et al
 * (2016)
 * @param double V -- value of large reward
 * @param double v -- value of small reward
 * @return double  -- p(ss chosen)
 */
double pss(double const g, double const V, double const v);
//*************END Maximum likelihood functions*************//
} // namespace spgxyz