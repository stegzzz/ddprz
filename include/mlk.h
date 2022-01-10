#pragma once
#include "ddFuncsML.h"
#include "ddgemModelNLopt.h"
#include "lsk.h"
#include "types.h"


namespace spgxyz {

/**
 * @brief produce maximum likelihood ke and kh values. for small or large
 * rewards create with S1,S2 or L1,L2 respectively. computes indifference points
 * along the way. these can be obtained with functions. Constructs base class
 * lsk and uses least squares estimates as initial values. Uses ddgemModelNLopt
 *
 * @sa ddgemModelNLopt
 * @sa lsk
 *
 */
class mlk : public lsk {
  double keml;
  double geml;
  double keSumNLL; //!< sum negative log likelihood
  double khml;
  double ghml;
  double khSumNLL; //!< sum negative log likelihood

public:
  mlk(ddtseq const &seq1, ddtseq const &seq2, double amount);
  /**
   * @brief get hyperbolic or exponential k
   * @param int h = 1 - default to hyperbolic model h==0 get exponential model k
   *
   * @return double - kh or ke
   */
  double operator()(int h = 1) const override;

  /**
   * @brief access k values in LSK base class
   *
   * @param int h = 1 - default to hyperbolic model h==0 get exponential model k
   *
   * @return double - kh or ke
   */
  double getLSK(int h = 1) { return h == 1 ? kh : ke; }

  kAndFit getksAndMLs();

  std::pair<double, double> getGs();
};

} // namespace spgxyz