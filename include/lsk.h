#pragma once
#include "ddFuncs.h"
#include "ddgemModelBoost.h"
#include "types.h"
#include <fstream>
#include <string>
#include <vector>

namespace spgxyz {

/**
 * @brief produce least squares ke and kh values. for small or large rewards
 * create with S1,S2 or L1,L2 respectively. computes indifference points along
 * the way. these can be obtained with functions. Uses ddgemModelBoost
 *
 * @sa ddgemModelBoost
 * @sa mlk
 *
 */
class lsk {

protected:
  ddtseq const &s1;
  ddtseq const &s2;
  double ke;
  double kessd;
  double kh;
  double khssd;
  std::vector<idpValues> IndiffPts; // value at each delay
  bool idpCriteriaOK;
  double const amount;

  /**
   * @brief get indifference point for the choices in ddtseq betweeen first and
   * last e.g. call with first= first amount at delay d, last= last amount+1 at
   * delay d.
   */
  double getIDP(ddtseq::const_iterator first,
                ddtseq::const_iterator const last);

  /**
   * @brief average indifference points in sequence 1 and sequence 2
   */
  idpValues avgIDPts(idpValues::const_iterator first1,
                     idpValues::const_iterator last1,
                     idpValues::const_iterator first2);

  bool idpCriteriaCheck(std::array<double, nd> const &idps);

public:
  /**
   * @brief two dd sequence and associated least squares stats
   *
   * @param ddtseq const &seq1
   * @param ddtseq const &seq2
   * @param double amount -- LL amount
   */
  lsk(ddtseq const &seq1, ddtseq const &seq2, double amount);
  /**
   * @brief get hyperbolic or exponential k
   * @param int h = 1 - default to hyperbolic model h==0 get exponential model k
   *
   * @return double - kh or ke
   */
  virtual double operator()(int h = 1) const;

  /**
   * @return  vector[0] seq1 indiff,vector[1] seq2 indiff, vector[2] avg indiff
   */
  std::vector<idpValues> getIndifferencePoints() const { return IndiffPts; };

  /**
   * @return result[0]=kh, result[1]=khssd, result[2]=ke, result[3]=kessd
   */
  kAndFit getksAndSSDs() const;

  std::pair<ddtseq const &, ddtseq const &> const getDDTSequences() const {
    return std::pair<ddtseq const &, ddtseq const &>(s1, s2);
  }

  /**
   * @brief Johnson & Bickel inclusion criteria met
   *
   * @return bool - true OK, false not met, no ks etc.
   */
  bool getInclusionCheck() const { return idpCriteriaOK; }

  void writeIndifferencePoints(std::string f);
  void writeKsAndSSDs(std::string f, kAndFit data);

  double getLLAmount() const { return amount; }

  lsk() = delete; // there is only one way to construct this object
  lsk(lsk const &) = delete;
  lsk(lsk &&) = delete;
  lsk &operator=(lsk const &) = delete;
  lsk &operator=(lsk &&) = delete;
};

} // namespace spgxyz