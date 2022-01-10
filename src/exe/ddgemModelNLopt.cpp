#include "ddgemModelNLopt.h"

namespace spgxyz {

// NLOPT
double ddgemModelNLopt::operator()(std::vector<double> const &x) {
  return (valueML(x[0], x[1], this));
}

double ddgemModelNLopt::wrap(std::vector<double> const &x,
                             std::vector<double> &grad, void *data) {
  assert(grad.empty() && "error in abcrit operator called with !grad.empty()");
  return (*reinterpret_cast<ddgemModelNLopt *>(data))(x);
}

bool ddgemModelNLopt::minimise() {
  nlopt::opt opt(nlopt::LN_NELDERMEAD, 2);
  // opt.set_xtol_rel(rel_tol);
  opt.set_maxeval(20);
  opt.set_lower_bounds(lb);
  opt.set_upper_bounds(ub);
  x.clear();
  x.push_back(valueML == &hdd_ml ? myMLK.getLSK() : myMLK.getLSK(0));
  x.push_back(g_def);
  // std::vector<double> steps{0.01, 0.001};
  // opt.set_initial_step(steps);
  auto steps = opt.get_initial_step_(x);

  opt.set_min_objective(wrap, this);
  try {
    nlopt::result nlresult = opt.optimize(x, minf);
    //    std::cout << "nlopt result " << nlresult << std::endl;
    if (nlresult > 0) {
      result = std::make_pair(std::make_pair(x[0], x[1]), minf);
      minSuccess = true;
    } else {
      result = std::make_pair(
          std::make_pair(std::numeric_limits<float>::quiet_NaN(),
                         std::numeric_limits<float>::quiet_NaN()),
          std::numeric_limits<float>::quiet_NaN());
      minSuccess = false;
      std::cout << nlresult << "\tfail" << std::endl;
    }

  } catch (std::exception &e) {
    std::cerr << "nlopt failed: " << e.what() << std::endl;
    result =
        std::make_pair(std::make_pair(std::numeric_limits<float>::quiet_NaN(),
                                      std::numeric_limits<float>::quiet_NaN()),
                       std::numeric_limits<float>::quiet_NaN());
    minSuccess = false;
    return false;
  }
  return true;
}

} // namespace spgxyz