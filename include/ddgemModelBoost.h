#pragma once
#include "ddgemModel.h"
#include "types.h"
#include <array>
#include <boost/math/tools/minima.hpp>
#include <type_traits>


namespace spgxyz {

/**
 * @brief used for delay discounting model with least squares estimate of k
 * (hyperbolic and exponential)
 *
 */
class ddgemModelBoost : public ddgemModel {

  /**
   * @brief functor for boost minimisation
   *
   * @param double k -- evaluation point
   * @return double -- value
   */
  double operator()(double k) { return evaluate(k); }
  template <class F, class T>
  friend std::pair<T, T>
  boost::math::tools::brent_find_minima(F f, T min, T max, int bits,
                                        boost::uintmax_t &max_iter)
      BOOST_NOEXCEPT_IF(BOOST_MATH_IS_FLOAT(T) &&noexcept(std::declval<F>()(
          std::declval<T>()))); // allow boost access to private ()

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
  ddgemModelBoost(std::vector<double> const &ds, std::vector<double> const &vs,
                  double const a)
      : ddgemModel(ds, vs, a) {}
  /**
   * @brief overload constructor to allow array construction /sa
   * ddgemModelBoost(std::vector<double> const &ds, std::vector<double> const
   * &vs, double const a)
   *
   */
  ddgemModelBoost(delayArr const &ds, idpValues const &vs, double const a)
      : ddgemModel(ds, vs, a) {}

  /**
   * @brief performs the minimisation returning true on success
   *
   * Uses
   */
  bool minimise();
};

/**
 * @brief functor wrapper for boost minimisation
 */
class dmod {

  double (*fnc)(double, void *);
  ddgemModel &mod;

public:
  dmod(double (*f)(double, void *), ddgemModel &m) : fnc(f), mod(m) {}
  double operator()(double k) { return fnc(k, &mod); }
};

} // namespace spgxyz
