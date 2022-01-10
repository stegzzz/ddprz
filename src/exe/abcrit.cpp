#include "abcrit.h"

namespace spgxyz {

double abcrit::computeD() {
  double ud = ub(k, K, x, d);
  double ld = lb(K, x, d);
  return (ud + ld) / 2;
}

double abcrit::operator()(double const x, double const d) {
  this->x = x;
  this->d = d;
  this->D = computeD();
  double a = computeA();
  double b = computeB();
  return a * b;
}

double abcrit::computeA() {
  double minuend = dd(A, K, D);        // V(LL) in D
  double subtrahend = dd(x * A, k, d); // V(SS) in d
  return minuend - subtrahend;
}
double abcrit::computeB() {
  double minuend = x * A;              // V(SS) now
  double subtrahend = dd(A, K, D - d); // V(LL) in D-d
  return minuend - subtrahend;
}
// NLOPT
double abcrit::operator()(std::vector<double> const &x,
                          std::vector<double> &grad) {
  assert(grad.empty() && "error in abcrit operator called with !grad.empty()");
  return -(this->operator()(x[0], x[1]));
}

double abcrit::wrap(std::vector<double> const &x, std::vector<double> &grad,
                    void *data) {
  return (*reinterpret_cast<abcrit *>(data))(x, grad);
}

double abcritOptBoost::operator()(double const d) {
  return -abcrit::operator()(thisX, d);
}

/**
 * @todo runtime handling of assert condition in release build?
 */
bool abcritOptBoost::maximise() {

  maxSuccess = false;
  boost::uintmax_t maxit = 250;
  using boost::math::tools::brent_find_minima;
  try {

    auto [p, d] =
        boost::math::tools::brent_find_minima(*this, 5., 180., 24, maxit);
    maxParam = p;
    maxABCrit = d;
  } catch (const std::exception &e) {
    std::cout << "__\nabcritOptBoost maximise error: message from thrown "
                 "exception was:\n   "
              << e.what() << std::endl;
    return false;
  }
  maxSuccess = true;
  return true;
}

} // namespace spgxyz