#include "nlopt.hpp"
#include <iomanip>
#include <iostream>
#include <vector>
// https://nlopt.readthedocs.io/en/latest/NLopt_Tutorial/

double myfunc(unsigned n, const double *x, double *grad, void *my_func_data);
double myconstraint(unsigned n, const double *x, double *grad, void *data);
typedef struct {
  double a, b;
} my_constraint_data;

int main(int argc, char *argv[]) {

  nlopt::opt opt(nlopt::LD_MMA, 2);
  std::vector<double> lb(2);
  lb[0] = -HUGE_VAL;
  lb[1] = 0;
  opt.set_lower_bounds(lb);
  opt.set_min_objective(myfunc, NULL);
  my_constraint_data data[2] = {{2, 0}, {-1, 1}};
  opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
  opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);
  opt.set_xtol_rel(1e-4);
  std::vector<double> x(2);
  x[0] = 1.234;
  x[1] = 5.678;
  double minf;

  try {
    nlopt::result result = opt.optimize(x, minf);
    std::cout << "found minimum at f(" << x[0] << "," << x[1]
              << ") = " << std::setprecision(10) << minf << std::endl;
  } catch (std::exception &e) {
    std::cout << "nlopt failed: " << e.what() << std::endl;
  }
}

double myfunc(unsigned n, const double *x, double *grad, void *my_func_data) {
  if (grad) {
    grad[0] = 0.0;
    grad[1] = 0.5 / sqrt(x[1]);
  }
  return sqrt(x[1]);
}

double myconstraint(unsigned n, const double *x, double *grad, void *data) {
  my_constraint_data *d = (my_constraint_data *)data;
  double a = d->a, b = d->b;
  if (grad) {
    grad[0] = 3 * a * (a * x[0] + b) * (a * x[0] + b);
    grad[1] = -1.0;
  }
  return ((a * x[0] + b) * (a * x[0] + b) * (a * x[0] + b) - x[1]);
}