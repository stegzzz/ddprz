#include "lsk.h"

namespace spgxyz {

lsk::lsk(ddtseq const &seq1, ddtseq const &seq2, double const amount)
    : s1{seq1}, s2{seq2}, amount{amount}, idpCriteriaOK{false} {
  if ((s1.size() != npseq) || (s2.size() != npseq))
    throw std::runtime_error{std::string{"ddtseq wrong size in "} + __func__ +
                             " needs " + std::to_string(npseq)};

  /**
   * @brief compute indifference points for a sequence
   */
  auto lambda = [this](ddtseq::const_iterator f, ddtseq::const_iterator l) {
    std::array<double, nd>
        result; // destination of sequence for indifference points
    auto seqi_it = result.begin();
    while (f != l) {
      *seqi_it = getIDP(f, f + na);
      seqi_it++;
      std::advance(f, na);
    }
    return result;
  };
  IndiffPts.push_back(lambda(s1.begin(), s1.end()));
  IndiffPts.push_back(lambda(s2.begin(), s2.end()));
  IndiffPts.push_back(
      avgIDPts(IndiffPts[0].begin(), IndiffPts[0].end(), IndiffPts[1].begin()));
  idpCriteriaOK = idpCriteriaCheck(IndiffPts[2]);

  if (idpCriteriaOK) {
    ddgemModelBoost ddmodel{delays, IndiffPts[2], amount};
    ddmodel.setFunc(&hdd); // hyperbolic model
    ddmodel.minimise();
    {
      auto [k, ssd] = ddmodel.getResult();
      kh = k;
      khssd = ssd;
    }
    ddmodel.setFunc(&edd); // exponential model
    ddmodel.minimise();
    {
      auto [k, ssd] = ddmodel.getResult();
      ke = k;
      kessd = ssd;
    }
  }
}

double lsk::getIDP(ddtseq::const_iterator first,
                   ddtseq::const_iterator const last) {
  if ((*first).choice == 0)
    return ((*first).amount /
            2); // S chosen on first trial the indifference point, value of L at
                // that delay is midway between 0 and smallest amount
  while ((++first) != last) {
    if ((*first).choice == 0) {
      auto previous = first - 1;
      return (((*first).amount + (*previous).amount) / 2);
    }
  }
  return (*(--first)).amount;
}

idpValues lsk::avgIDPts(idpValues::const_iterator first1,
                        idpValues::const_iterator last1,
                        idpValues::const_iterator first2) {
  idpValues result;

  auto r_it = result.begin();
  while (first1 != last1) {
    *r_it = ((*first1) + (*first2)) / 2;
    ++first1;
    ++first2;
    ++r_it;
  }
  return result;
}

kAndFit lsk::getksAndSSDs() const {
  assert(idpCriteriaOK && " error in " && __func__);
  kAndFit result{};
  result[0] = kh;
  result[1] = khssd;
  result[2] = ke;
  result[3] = kessd;
  return result;
}

void lsk::writeIndifferencePoints(std::string f) {
  auto ouf = std::ofstream(f);
  if (!ouf)
    throw std::runtime_error{std::string{"cannot open "} + f + " in " +
                             __func__};
  for (auto a : IndiffPts) {
    for (auto pt : a) {
      ouf << pt << "\t";
    }
    ouf << std::endl;
  }
  ouf.close();
}

void lsk::writeKsAndSSDs(std::string f, kAndFit data) {
  assert(idpCriteriaOK && " error in " && __func__);
  auto ouf = std::ofstream(f);
  if (!ouf)
    throw std::runtime_error{std::string{"cannot open "} + f + " in " +
                             __func__};
  for (auto d : data)
    ouf << d << std::endl;

  ouf.close();
}

bool lsk::idpCriteriaCheck(std::array<double, nd> const &idps) {
  // check the last indifference point is greater than the first indifference
  // point by at least 10% of LL -- idps[0] is last, idps[nd-1] is first
  if ((idps[0] - idps[nd - 1]) <= (amount * 0.1))
    return false;

  // check that no indifference point is greater than that at the previous delay
  // by more than 20% of LL -- idps arranged from small to large delays
  auto lim = amount * 0.2;
  for (int i = 1; i < nd; i++)
    if ((idps[i] - idps[i - 1]) > lim)
      return false;

  // both tests passed
  return true;
}

double lsk::operator()(int h) const {
  assert(idpCriteriaOK && " error in " && __func__);
  if (idpCriteriaOK)
    return (h == 0 ? ke : kh);
  return -1;
}

} // namespace spgxyz