#include "../include/ddgemModelBoost.h"

namespace spgxyz {
/**
 * @todo runtime handling of assert condition in release build?
 */
bool ddgemModelBoost::minimise() {
  assert(value != nullptr &&
         "value function n* not set in ddgemModel::minimise()\n");
  minSuccess = false;
  boost::uintmax_t maxit = 250;
  using boost::math::tools::brent_find_minima;
  try {

    auto [p, d] =
        boost::math::tools::brent_find_minima(*this, 0., 4., 24, maxit);
    minParam = p;
    minSSD = d;
  } catch (const std::exception &e) {
    std::cout << "__\nddgemModelBoost minimise error: message from thrown "
                 "exception was:\n   "
              << e.what() << std::endl;
    return false;
  }
  minSuccess = true;
  return true;
}

} // namespace spgxyz