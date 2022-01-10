#include "../include/ddgemModel.h"

namespace spgxyz {
ddgemModel::ddgemModel(std::vector<double> const &ds,
                       std::vector<double> const &vs, double const a)
    : A(a) {
  init(ds, vs);
}

ddgemModel::ddgemModel(delayArr const &ds, idpValues const &vs, double const a)
    : A(a) {
  init(ds, vs);
}
/**
 * @todo runtime handling of assert condition in release build?
 */
void ddgemModel::init(std::vector<double> const &ds,
                      std::vector<double> const &vs) {
  assert(ds.size() == vs.size() &&
         "delays and values unequal sizes in ddgemModel.init()");
  for (auto d : ds)
    delays.push_back(d);
  for (auto v : vs)
    values.push_back(v);
}

void ddgemModel::init(delayArr const &ds, idpValues const &vs) {
  assert(ds.size() == vs.size() &&
         "delays and values unequal sizes in ddgemModel.init()");
  for (auto d : ds)
    delays.push_back(d);
  for (auto v : vs)
    values.push_back(v);
}

std::pair<double, double> ddgemModel::getResult() {
  if (minSuccess == true)
    return std::make_pair(minParam, minSSD);
  return std::make_pair(std::numeric_limits<float>::quiet_NaN(),
                        std::numeric_limits<float>::quiet_NaN());
}

/**
 * @todo runtime handling of assert condition in release build?
 */
double ddgemModel::evaluate(double k) {
  assert(value != nullptr &&
         "value function n* not set in ddgemModel::minimise()\n");
  return value(k, this);
}

} // namespace spgxyz