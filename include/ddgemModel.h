#pragma once
#include "types.h"
#include <array>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>

namespace spgxyz {

/**
 * @brief single parameter delay discounting model with least squares parameter
 * estimate.
 *
 *
 */
class ddgemModel {

  void init(std::vector<double> const &ds, std::vector<double> const &vs);
  void init(delayArr const &ds, idpValues const &vs);

protected:
  std::vector<double> delays;
  std::vector<double> values;
  double const A;

  bool minSuccess =
      false; /*!< true on successfull completion of minimisation */
  double minParam;
  double minSSD;

  double (*value)(double, void *) =
      nullptr; /*!< ptr to model dd function, must be set for evaluation or
                  minimisation (minimisation only applicable to derived classes)
                */
  double (*valueML)(double, double, void *) =
      nullptr; /*!< ptr to model dd function for maximum likelihood, must be set
for evaluation or minimisation (minimisation only applicable to derived classes)
*/

public:
  // TODO - delete default constructors etc.
  /**
   * @brief provide delays, values, and undiscounted amount. Uses Brent
   * minimiser.
   *
   * @param std::vector<double> const &ds -- delays
   * @param std::vector<double> const &vs -- values
   * @param double const a -- amount
   */
  ddgemModel(std::vector<double> const &ds, std::vector<double> const &vs,
             double const a);

  /**
   * @brief overload constructor to allow array construction /sa
   * ddgemModel(std::vector<double> const &ds, std::vector<double> const &vs,
   *            double const a);
   */
  ddgemModel(delayArr const &ds, idpValues const &vs, double const a);

  /**
   * @brief pass in a dd function e.g. hyperbolic model, the function takes a
   * ptr to this which it uses to retrieve values and delays. See
   * ddgemModelTest.cpp for example function. e.g. used in minimisation with
   * function hdd(double const k, void *const m)
   *
   * @param double (*v)(double, void *) -- v is ptr to function returning a
   * double and taking a double parameter (e.g. k) and a ptr to this
   *
   */
  void setFunc(double (*v)(double, void *)) { value = v; }

  /**
   * @brief set a dd function e.g. used in maximum likelihood minimisation with
   * function hdd_ml(double const k, double const g, void *const m)
   *
   * @sa setFunc(double (*v)(double, void *))
   */
  void setFunc(double (*v)(double, double, void *)) { valueML = v; }

  /**
   * @brief evaluate value function (call setFunc first) at k
   *
   * @param double k -- point to evaluate value function at
   * @return double value
   */
  double evaluate(double k);

  /**
   * @brief recover the delays
   */
  std::vector<double> const &getDelays() const { return delays; }

  /**
   * @brief recover the values
   */
  std::vector<double> const &getValues() const { return values; }

  /**
   * @brief recover the amount
   */
  double getA() const { return A; }

  /**
   * @brief returns result of minimisation
   *
   * @return std::pair<double,double> -- param, ssd; NaNs if not converged
   */
  std::pair<double, double> getResult();
}; // class ddgemModel

} // namespace spgxyz