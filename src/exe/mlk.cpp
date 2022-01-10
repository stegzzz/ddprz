#include "mlk.h"

namespace spgxyz {
mlk::mlk(ddtseq const &seq1, ddtseq const &seq2, double amount)
    : lsk{seq1, seq2, amount} {
  /*  auto f = [](ddtseq const &seq) {
     for (auto d : seq) {
       std::cout << d.amount << "\t" << d.delay << "\t" << d.choice <<
   std::endl;
     }
     std::cout << "__" << std::endl;
   };
   std::cout << "mlk seq1" << std::endl;
   f(seq1);
   std::cout << "mlk seq2" << std::endl;
   f(seq2); */

  if (idpCriteriaOK) {
    ddgemModelNLopt ddmodel(
        delays, IndiffPts[2], amount,
        *this); //!> initialise from IndiffPts and amount in lsk base class

    ddmodel.setFunc(&hdd_ml); // hyperbolic model
    ddmodel.minimise();
    {
      auto [k, ll] = ddmodel.getResultML();
      khml = k.first;
      ghml = k.second;
      khSumNLL = ll;
    }

    ddmodel.setFunc(&edd_ml); // exponential model
    ddmodel.minimise();
    {
      auto [k, ll] = ddmodel.getResultML();
      keml = k.first;
      geml = k.second;
      keSumNLL = ll;
    }
  }
}

kAndFit mlk::getksAndMLs() {
  assert(idpCriteriaOK && " error in " && __func__);
  kAndFit result{};
  result[0] = khml;
  result[1] = khSumNLL;
  result[2] = keml;
  result[3] = keSumNLL;
  return result;
}

std::pair<double, double> mlk::getGs() { return std::make_pair(ghml, geml); }

double mlk::operator()(int h) const {
  assert(idpCriteriaOK && " error in " && __func__);
  if (idpCriteriaOK)
    return (h == 0 ? keml : khml);
  return -1;
}
} // namespace spgxyz