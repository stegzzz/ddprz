#include "TestXMLGenerator.h"

namespace spgxyz {

TestXMLGenerator::TestXMLGenerator(
    double k, double g,
    std::tuple<delayArr const &, amountArr const &, amountArr const &> data,
    bool model, int id, std::string ouf)
    : k(k), g(g), exponentialModel(model), id(id), ouf(ouf) {
  auto [delays, sAmounts, lAmounts] = data;

  std::seed_seq seeds{rd(), rd(), rd(), rd(), rd()};
  rnd.seed(seeds);
#if PRINTXMLGENERATIONFORMAT
  int N = 0;
#endif

  for (int series : {1, 2}) { // first and second series
    int n = 0;
    for (int amounts : {1, 2}) { // small and large rewards
      auto Amounts = amounts == 1 ? sAmounts : lAmounts;
      for (auto d : delays) {
        for (auto a : Amounts) { // for each amount
          dataElements.push_back(
              xmlData(series, ++n, getResponse(d, a, Amounts[na - 1])));
#if PRINTXMLGENERATIONFORMAT
          ++N;
          std::cout << "n= " << N << " series= " << series << " now= " << a
                    << " later= " << Amounts[na - 1] << " in= " << d << " attr "
                    << dataElements.back().attributeTag() << std::endl;
#endif

        } // for(auto d: delays){
      }   // for (auto a : Amounts){// for each amount
    }     // for(int n:{1,2}){//small and large rewards
  }       // for(int n:{1,2}){//first and second series
  createXML();
} // TestXMLGenerator::TestXMLGenerator(double ke, double kh, double g,
  // std::tuple<delayArr const &, amountArr const &, amountArr const &> data,
  // std::string ouf)

int TestXMLGenerator::getResponse(double d, double a, double A) {
  std::function<double(double const a, double const k, double const d)> ddfunc =
      exponentialModel ? sedd : shdd;
  double pss = 1 / (1 + exp(g * (ddfunc(A, k, d) - a)));
  /* auto result = (ud(rnd) > pss) ? 2 : 1;
  std::cout << std::setw(8) << pss << "\t" << A << "\t" << d << "\t" << a
            << "\t" << result << std::endl; */
  if (ud(rnd) > pss)
    return 2;
  return 1;

} // int getResponse(double d, double a, double A){

void TestXMLGenerator::randomisePreXMLData() {
  for (auto &a : dataElements)
    a.setRandomOrderKey(lud(rnd));
  std::sort(dataElements.begin(), dataElements.end());
}
void TestXMLGenerator::orderPreXMLData() {
  for (auto &a : dataElements)
    a.setOrderKey();
  std::sort(dataElements.begin(), dataElements.end());
}
void TestXMLGenerator::printPreXMLData() {
  for (auto &a : dataElements)
    a.print();
}

void TestXMLGenerator::createXML() {
  xmlDoc.Parse(startXML.c_str());
  auto rootPtr = xmlDoc.RootElement();
  rootPtr->SetAttribute("attr", "ID");
  rootPtr->SetText(id);
  for (auto x : dataElements) {
    auto buff = rootPtr->InsertNewChildElement("data");
    buff->SetAttribute("attr", x.attributeTag().c_str());
    buff->SetText(std::to_string(x.getValue()).c_str());
  }
}

void TestXMLGenerator::refreshXML() {
  xmlDoc.Clear();
  createXML();
}

void TestXMLGenerator::printXMLData() {
  tinyxml2::XMLPrinter printer{};
  xmlDoc.Print(&printer);
  std::cout << std::string{printer.CStr()};
}

void TestXMLGenerator::saveXMLData() { saveXMLData(ouf); }

void TestXMLGenerator::saveXMLData(std::string f) {
  auto fp = std::fopen(f.c_str(), "w");
  tinyxml2::XMLPrinter printer{fp};
  xmlDoc.Print(&printer);
  std::fclose(fp);
}

void xmlData::print() {
  std::cout << "n= " << n << " series= " << series << " value= " << value
            << " attr tag " << attributeTag() << std::endl;
}

std::string xmlData::attributeTag() {
  std::ostringstream os("S1", std::ios_base::ate);
  os << std::setw(2) << std::setfill('0') << series << "_" << std::setw(2)
     << std::setfill('0') << n;
  return os.str();
}

} // namespace spgxyz
