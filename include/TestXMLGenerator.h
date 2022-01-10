#pragma once
#include "ddFuncs.h"
#include "ddgemModel.h"
#include "tinyxml2.h"
#include <algorithm>
#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <tuple>


#define PRINTXMLGENERATIONFORMAT 0
// PRINTXMLGENERATIONFORMAT true will print XML "tag map" format of the xml that
// is being generated
namespace spgxyz {

class xmlData;
/**
 * @brief generate xml packet for simulated ddGEM data. XML data for 160 choices
 * for the delays, amounts to be used in study NB generated responses are
 * probabilistic and use hyperbolic or exponential model
 */
class TestXMLGenerator {
private:
  double k;
  double g;
  std::string ouf;
  bool exponentialModel;
  std::random_device rd;
  std::mt19937 rnd;
  std::uniform_real_distribution<> ud{0,
                                      1}; //!< probilistic response generation
  std::uniform_int_distribution<> lud{0, 10000}; //!< sorting xmlData
  std::vector<xmlData> dataElements; //!< data held in an xml precursor form,
                                     //!< xml constructed from this
  tinyxml2::XMLDocument xmlDoc{};
  int id;
  void createXML(); //!< populate xmlDoc with pre xml dataElements

  /* clang-format off */
  std::string startXML{"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                       "<myData attr =\"ID\">"
                       "<saveEnv save =\"false\"/>"
                       "</myData>"};
  /* clang-format on */

public:
  TestXMLGenerator(
      double k, double g,
      std::tuple<delayArr const &, amountArr const &, amountArr const &> data,
      bool model, int id, std::string ouf);

  /**
   * @brief generate 0 (small choice) or 1 (large choice) for the small
   * immediate reward a against the larger reward A at delay d. Uses Eq 4
   * ddGem from Cavagnaro et al (2016)
   *
   */
  int getResponse(double d, double a, double A);

  void refreshXML(); //!< recreate xmlDoc with pre xml dataElements, e.g. after
                     //!< randomisation

  /**
   * @brief prints the xml precursor data vector dataElements
   */
  void printPreXMLData();
  void randomisePreXMLData();
  void orderPreXMLData();

  void printXMLData();
  void saveXMLData();              //!< save under default file name
  void saveXMLData(std::string f); //!< select different file name

  TestXMLGenerator() = delete; // there is only one way to construct this object
  TestXMLGenerator(TestXMLGenerator const &) = delete;
  TestXMLGenerator(TestXMLGenerator &&) = delete;
  TestXMLGenerator &operator=(TestXMLGenerator const &) = delete;
  TestXMLGenerator &operator=(TestXMLGenerator &&) = delete;
};

/**
 * @brief information to create xml data element, can be stored in vector and
 * randomise/sorted prior to making tinyxml2 document.
 */
class xmlData {
  int series;
  int n;
  int value; //!< response 1 (small choice) or 2 (large choice)
  int key;

public:
  xmlData(int series, int n, int value)
      : series(series), n(n), value(value), key(n) {}
  /**
   * @brief make n the sort key so that orginal ordering can be restored
   */
  void setOrderKey() { key = n; }

  /**
   * @brief set sort key to random value
   */
  void setRandomOrderKey(int k) { key = k; }

  int getSeries() { return series; }
  int getN() { return n; }
  int getValue() { return value; }

  void print();

  bool operator<(xmlData const &o) { return key < o.key; }

  /**
   * @brief construct require xml attr tag based on values contained here
   */
  std::string attributeTag();
  xmlData() = delete;
};

} // namespace spgxyz