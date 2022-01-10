#pragma once
#include "ddFuncs.h"
#include <boost/math/tools/minima.hpp>
#include <cassert>
#include <type_traits>
#include <vector>

namespace spgxyz {
/**
 * @brief base class for computing delay discounting ab criterion
 *
 * @todo check no destructor needed, ok with default copy and move semantics?
 */
class abcrit {

protected:
  double x;
  double d;
  double D;
  double const K;
  double const k;
  double const A;
  lfptr const lb;  /*!< lower bound function */
  ufptr const ub;  /*!< upper bound function */
  ddfptr const dd; /*!< delay discounting function */

  /**
   * @brief return mean of LD and UD
   */
  double computeD();

  /**
   * @brief compute LL-SS at t=0
   *
   * \see computeB()
   */
  double computeA();

  /**
   * @brief compute SS-LL at t=d
   *
   * \see computeA()
   */
  double computeB();

public:
  /**
   * @brief construct abstract base class for computing ab criterion, see
   * OptimiseDD.java and latex\work\ddgem \see abcritOptBoost for optimisation
   * class
   * @param double const K
   * @param lfptr const l    --function for computing lower bound
   * @param ufptr const u    --function for computing upper bound
   * @param ddfptr const d   --function for computing delay discounted value
   * @param double const A = 1000   --large amount
   * @param double const m = 1.4    --k=mK
   *
   */
  abcrit(double const K, lfptr const l, ufptr const u, ddfptr const d,
         double const A = 1000, double const m = 1.4)
      : K(K), k(m * K), lb(l), ub(u), dd(d), A(A) {}

  /**
   * @brief this is a functor class, the value of abcrit is obtained e.g.
   * abcritE = abe(0.8,&LDe,&HDe, &shdd);  //exponential model
   * double val=abcritE(0.5, 50);
   *
   * @param double x    --a=xA
   * @param double d    --ss delay
   */
  double operator()(double const x, double const d);

  // MAKE this a functor so it can be initialised with K and h or e UD/LD
  // function then  called with x,d
  // then call ae,be or ah,bh according to model

  // NLOPT
  double operator()(std::vector<double> const &x,
                    std::vector<double> &grad); /*!< for nlopt */

  static double wrap(std::vector<double> const &x, std::vector<double> &grad,
                     void *data); /*!< for nlopt */
};

/**
 * @brief find d which maximises abcrit. takes data from two mlk objects, one
 * for a small reward and one for a large reward. computes value of d which
 * maximises ab crit for question would you like S in d or L in D with D set to
 * (ud-ld)/2
 */
class abcritOptBoost : public abcrit {
  double thisX;
  bool maxSuccess;
  double maxParam;
  double maxABCrit;

  template <class F, class T>
  friend std::pair<T, T>
  boost::math::tools::brent_find_minima(F f, T min, T max, int bits,
                                        boost::uintmax_t &max_iter)
      BOOST_NOEXCEPT_IF(BOOST_MATH_IS_FLOAT(T) &&noexcept(std::declval<F>()(
          std::declval<T>()))); // allow boost access to private ()
                                /**
                                 * @brief this is a functor class \sa abcrit::()()
                                 *
                                 * @param double d    --ss delay
                                 */

public:
  /**
   * @param double const K          - large amount K
   * @param lfptr const l           - lower bound function
   * @param ufptr const u           - upper bound function
   * @param ddfptr const d          - simple delay discounting function
   * @param double const A = 1000   - large amount
   * @param double const m = 1.4    - m = k/K
   * @param double const x = 0.82   - x = a/A, a = small amount
   *
   */
  abcritOptBoost(double const K, lfptr const l, ufptr const u, ddfptr const d,
                 double const A = 1000, double const m = 1.4,
                 double const x = 0.82)
      : abcrit{K, l, u, d, A, m}, thisX{x},
        maxSuccess{false}, maxParam{0}, maxABCrit{0} {}

  double operator()(double const d);

  double getMaxParam() { return maxSuccess ? maxParam : -99; }
  double getMaxABCrit() { return maxSuccess ? maxABCrit : -99; }

  /**
   * @brief performs the minimisation returning true on success
   *
   * Uses
   */
  bool maximise();
};
} // namespace spgxyz