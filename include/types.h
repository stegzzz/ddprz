#pragma once
#include <array>

namespace spgxyz {

using arrsz = size_t;
constexpr arrsz nkf{4};         //!< number of k and fit values
constexpr arrsz nd{5};          //!< number of delays
constexpr arrsz na{8};          //!< number of amounts
constexpr arrsz npseq{nd * na}; //!< length sequence (for each S1, S2, L1, L2)
constexpr arrsz ns2{48};        //!< number of stage 2 values
struct ddt {
  double amount = 0;
  double delay = 0;
  int choice = 0;
};
using ddtseq = std::array<ddt, npseq>; //!< ddt array
using idpValues =
    std::array<double, nd>; //!< indifference points (values at each delay)
using delayArr = std::array<double, nd>;  //!< delays
using amountArr = std::array<double, na>; //!< amounts
using s2seq = std::array<double, ns2>;    //!< stage 2 sequence
/**
 * k and measure of fit information -- kh, khssd, ke, kessd for lsk, -- kh,
 * khsumMinusLL, ke, kesumMinusLL for mlk,
 */
using kAndFit = std::array<double, nkf>;
constexpr delayArr delays{0.25, 1, 6, 12, 24};
constexpr amountArr smallAmounts{4, 82, 205, 328, 451, 574, 679, 820};
constexpr amountArr largeAmounts{5, 100, 250, 400, 550, 700, 850, 1000};
enum class s2idx : std::size_t {
  d,
  abc,
  ld,
  ud,
  ll1,
  ll2,
  ls1,
  ls2,
  ss1,
  ss2
}; //!< indices for vector storing stage 2 data

enum class binFileType { xml, idps, s2data };

#ifndef USINGUTILS
template <typename E>
constexpr typename std::underlying_type<E>::type getEnumValue(E e) {
  return static_cast<typename std::underlying_type<E>::type>(e);
};
#endif

} // namespace spgxyz