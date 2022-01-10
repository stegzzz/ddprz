#include "ddFuncs.h"
namespace spgxyz {
double LDh(double const K, double const x, double const d) {
  return (1 - x) / (x * K) + d;
}

double UDh(double const k, double const K, double const x, double const d) {
  double num = (1 + k * d) / x;
  num -= 1;
  return num / K;
}
double LDe(double const K, double const x, double const d) {
  double subt = std::log(x) / K;
  return d - subt;
}
double UDe(double const k, double const K, double const x, double const d) {
  double num = (k * d) - std::log(x);
  return num / K;
}

double hdd(double const k, void *const m) {
  ddgemModel *mod = static_cast<ddgemModel *>(m);
  auto ds = mod->getDelays();
  auto vs = mod->getValues();
  auto a = mod->getA();
  double result = 0;
  auto dit = ds.begin();
  auto ditE = ds.end();
  auto vit = vs.begin();
  while (dit != ditE) {
    auto v = shdd(a, k, *dit);
    result += pow(v - (*vit), 2);
    dit++;
    vit++;
  }
  return result;
}
double shdd(double const a, double const k, double const d) {
  return a / (1 + k * d);
}

double edd(double const k, void *const m) {
  ddgemModel *mod = static_cast<ddgemModel *>(m);
  auto ds = mod->getDelays();
  auto vs = mod->getValues();
  auto a = mod->getA();
  double result = 0;
  auto dit = ds.begin();
  auto ditE = ds.end();
  auto vit = vs.begin();
  while (dit != ditE) {
    auto v = sedd(a, k, *dit);
    result += pow(v - (*vit), 2);
    dit++;
    vit++;
  }
  return result;
}

double sedd(double const a, double const k, double const d) {
  return a * exp(-k * d);
}

} // namespace spgxyz