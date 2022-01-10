#include "ddFuncsML.h"
namespace spgxyz {
double hdd_ml(double const k, double const g, void *const m) {
  /* auto f = [](ddtseq const &seq) {
    for (auto d : seq) {
      std::cout << d.amount << "\t" << d.delay << "\t" << d.choice << std::endl;
    }
    std::cout << "__" << std::endl;
  }; */

  ddgemModelNLopt *mod = static_cast<ddgemModelNLopt *>(m);
  mlk const &modMLK = mod->getMLK();
  auto [s1, s2] = modMLK.getDDTSequences();
  double ll = 0;
  double A = modMLK.getLLAmount();
  /*  std::cout << "hdd_ml s1" << std::endl;
   f(s1);
   std::cout << "hdd_ml s2" << std::endl;
   f(s2); */
  for (int i = 0; i < s1.size(); i++) {
    auto trial = s1[i];
    /* std::cout << "s1"
              << "\t" << trial.amount << "\t" << trial.choice << "\t"
              << trial.delay << std::endl; */
    double ps = pss(g, shdd(A, k, trial.delay), shdd(trial.amount, k, 0));
    ll += (trial.choice == 0) ? log(ps) : log(1 - ps);
    trial = s2[i];
    /* std::cout << "s2"
              << "\t" << trial.amount << "\t" << trial.choice << "\t"
              << trial.delay << std::endl; */
    ps = pss(g, shdd(A, k, trial.delay), shdd(trial.amount, k, 0));
    ll += (trial.choice == 0) ? log(ps) : log(1 - ps);
    /* std::cout << ll << "\t" << ps << "\n"; */
  }
  /* std::cout << std::endl;
  switch (fpclassify(ll)) {
  case FP_INFINITE:
    printf("infinite");
    break;
  case FP_NAN:
    printf("NaN");
    break;
  case FP_ZERO:
    printf("zero");
    break;
  case FP_SUBNORMAL:
    printf("subnormal");
    break;
  case FP_NORMAL:
    printf("normal");
    break;
  }
  std::cout << k << "\t" << g << "\tx" << ll << std::endl; */
  return -ll;
}

double edd_ml(double const k, double const g, void *const m) {
  ddgemModelNLopt *mod = static_cast<ddgemModelNLopt *>(m);
  mlk const &modMLK = mod->getMLK();
  auto [s1, s2] = modMLK.getDDTSequences();
  double ll = 0;
  double A = modMLK.getLLAmount();
  for (int i = 0; i < s1.size(); i++) {
    auto trial = s1[i];
    double ps = pss(g, sedd(A, k, trial.delay), sedd(trial.amount, k, 0));
    ll += (trial.choice == 0) ? log(ps) : log(1 - ps);
    trial = s2[i];
    ps = pss(g, sedd(A, k, trial.delay), sedd(trial.amount, k, 0));
    ll += (trial.choice == 0) ? log(ps) : log(1 - ps);
  }
  return -ll;
}

double pss(double const g, double const V, double const v) {
  double minp = 0.0000001; // prevent log(0)
  double result = 1 / (1 + exp(g * (V - v)));
  return result < minp ? minp : result;
}

} // namespace spgxyz