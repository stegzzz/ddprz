#pragma once
#include "ddFuncs.h"
#include "ddFuncsML.h"
#include "ddgemModel.h"
#include "types.h"
#include <array>
#include <nlopt.hpp>

namespace spgxyz {
class mlk;

/**
 * @brief used for delay discounting model with maximum likelihood estimate of k
 * (hyperbolic and exponential)
 *
 */
class ddgemModelNLopt : public ddgemModel {

  mlk &myMLK;
  std::vector<double> x;                      /*!< nlopt x values */
  std::vector<double> lb{0.000001, 0.000001}; /*!< nlopt lower bounds */
  std::vector<double> ub{5, 5};               /*!< nlopt uppper bounds */
  double rel_tol = 1e-6;                      //!< nlopt relative tolerance
  double g_def = 0.01;                        //!< initial value pss g
  double minf;
  std::pair<std::pair<double, double>, double>
      result; //!< result of ml minimisation

public:
  // TODO - delete default constructors etc.
  /**
   * @brief provide delays, values, and undiscounted amount. Uses NLopt
   * minimiser.
   *
   * @param std::vector<double> const &ds -- delays
   * @param std::vector<double> const &vs -- values
   * @param double const a -- amount
   * @param mlk &mlk -- required to get values for optimisation
   */
  ddgemModelNLopt(std::vector<double> const &ds, std::vector<double> const &vs,
                  double const a, mlk &mlk)
      : ddgemModel(ds, vs, a), myMLK(mlk){};

  /**
   * @brief overload constructor to allow array construction /sa
   * ddgemModelNLopt(std::vector<double> const &ds, std::vector<double> const
   * &vs, double const a)
   *
   */
  ddgemModelNLopt(delayArr const &ds, idpValues const &vs, double const a,
                  mlk &m)
      : ddgemModel(ds, vs, a), myMLK(m){};

  mlk &getMLK() { return myMLK; }

  /**
   * @brief returns result of minimisation
   *
   * @return std::pair<double,double> -- param, ssd; NaNs if not converged
   */
  std::pair<std::pair<double, double>, double> getResultML() { return result; }

  /**
   * @brief performs the minimisation returning true on success
   *
   * Uses
   */
  bool minimise();

  // NLOPT
  double operator()(std::vector<double> const &x); /*!< for nlopt */

  static double wrap(std::vector<double> const &x, std::vector<double> &grad,
                     void *data); /*!< for nlopt */
};

} // namespace spgxyz
