#include "ddgem.h"

/**
 * \file ddgem.cpp
 * Contains int main(int argc, char *argv[]) which processes
 * command line options and launches
 */

namespace spgxyz {
std::string const credentialsFile{"access.txt"};

#ifdef AWS
std::string const cgiDir{"/opt/bitnami/apache/cgi-bin/"};
#else
std::string const cgiDir{"c:/Apache24/cgi-bin/"};
#endif

std::string const preHTMLString{
    "Content-type:text/plain\r\n\r\n"
    "<html>\n<head>\n<title>Hello</title>\n</head>\n<body>\n"};
std::string const postHTMLString{"</body>\n</html>\n"};
std::string const badXMLFile{"badxml.xml"};

std::string inf; // binary file input for option 6
spgxyz::binFileType bft =
    spgxyz::binFileType::xml; // binary file type indicator -- read xml or
                              // indifference points

std::string envFile{"ddgemEnv.txt"}; //!< stores a cgicc environment for
                                     //!< debugging used in void printGetPost()
int ID;
} // namespace spgxyz

namespace po = boost::program_options;
const int nopts = 7;
/* clang-format off */
std::array<std::string, nopts> options = {
    "Option ID=1 option, print xml packet in cgi environment envFile ("+spgxyz::cgiDir+spgxyz::envFile+", change file with option -x)",
    "Option ID=2 option, run process on data in envFile (runCGI() equivalent), suffix option to label output (DEBUG only)",
    "Option ID=3 option, generate test xml data using k_test, model, and g, change k, model, and g using options. Output file specified with --testXMLOutput. Optionally create randomised xml also.",
    "Option ID=4 option, generate and print xml tags used for extracting data from hedwig's flat xml.",
    "Option ID=5 option, abcrit testing",
    "Option ID=6 option, read binary file from db, set binFileType inputFile option. xml=0 default",
    "Option ID=7 option, get results data from db"
    };
/* clang-format on */

int option = 0;
bool ver = false;
std::string helpStr{
    "delay discounting, option IDs and brief descriptions follow"};

double k_test = 0.05;
double K_test = 0.025;
double g_test = 0.01; // was 0.1 for test data sets 1..5, changed 8/5/21
bool exponentialModel_test = false;
double A_test = 1000;
double a_test = 820;
std::string xmlTestDataOutputFile = "testXMLPacket.xml";
int generatedXMLID = 9999;
bool randomXML = false;
bool summariseData = false;
bool onlyValid = false;
std::string suffix = "";
std::string daDir{"/documents/rwork/e178/ddgem/"};
std::string idValidFile{
    "valid_ids.txt"};                 //!< written by e178/ddgem/dataAnalysis.r
std::string idAllFile{"all_ids.txt"}; //!< written by e178/ddgem/dataAnalysis.r
std::string stage1DF{"stage1DataValues.txt"};
std::string stage2DF{"stage2DataValues.txt"};
std::string dataFile = stage1DF;

#ifdef VER
std::string version{VER};
#else
std::string version{"version not set"};
#endif

/**
 * @brief entry point. with no parameters enters cgi mode. parameterised enters
 * command line mode, see options.
 */
int main(int argc, char *argv[]) {
  po::variables_map vm;
  po::options_description desc{"Options"};
  std::string no{"Option IDs valid values 1.."};
  no.append(std::to_string(nopts));

  try {
    // clang-format off
    desc.add_options()
    ("help,h", "print help and list opt ID descriptions")
    ("version,v",po::bool_switch(&ver), "print version information")
    ("opt,o", po::value<int>(&option)->default_value(0), no.c_str())
    ("xml,x", po::value<std::string>(&spgxyz::envFile)->default_value(spgxyz::envFile), "cgi environment file in cgi-bin containing xmlpacket")
    ("ktest,k", po::value<double>(&k_test)->default_value(k_test), "k value for generating test data xml/testing abcrit (small k)")
    ("Ktest,K", po::value<double>(&K_test)->default_value(K_test), "K value for testing abcrit (large k)")
    ("atest,a", po::value<double>(&a_test)->default_value(a_test), "a value for testing abcrit (small amount)")
    ("Atest,A", po::value<double>(&A_test)->default_value(A_test), "A value for testing abcrit (large amount)")
    ("exponentialModeltest,e", po::bool_switch(&exponentialModel_test), "use exponential model for generating xml test data/testing abcrit, default hyperbolic")
    ("gParam,g", po::value<double>(&g_test)->default_value(g_test), "g value for generating test data xml")
    ("testXMLOutput", po::value<std::string>(&xmlTestDataOutputFile)->default_value(xmlTestDataOutputFile), "output file for generated test data xml")
    ("testXMLOutputID,i", po::value<int>(&generatedXMLID)->default_value(generatedXMLID), "ID attr file for generated test data xml")
    ("randomXML,r", po::bool_switch(&randomXML), "also generate randomised order test data xml file")
    ("suffix,s", po::value<std::string>(&suffix)->default_value(suffix), "suffix for labelling output (option 2)")
     ("inputFile,f", po::value<std::string>(&spgxyz::inf)->default_value(""), "binary input file for option 6")
     ("binFileType,t", po::value<int>(reinterpret_cast<int*>(&spgxyz::bft))->default_value(spgxyz::getEnumValue(spgxyz::binFileType::xml)), "binary input file type for option 6. 0=xml, 1=idps, 2=s2data")
     ("printSummary,p", po::bool_switch(&summariseData), "print data summary for option 7")
     ("onlyValid", po::bool_switch(&onlyValid), "only get data for valid cases (critJB==1 & critExpK==1) option 7")
     ("S1dataFile", po::value<std::string>(&stage1DF)->default_value(stage1DF), "change output file for option 7")
     ("S2dataFile", po::value<std::string>(&stage2DF)->default_value(stage2DF), "change output file for option 7")
      ;
    // clang-format on
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      spgxyz::help(desc);
      return 0;
    }
    if (ver == true) {
      std::cout << "version: " << version << std::endl;
      return true;
    }
  } catch (const po::error &ex) {
    std::cerr << ex.what() << "\n-";
  }

  switch (option) {
  case 1:
    std::cout << options[option - 1] << std::endl;
    try {
      spgxyz::printPost();
    } catch (std::runtime_error e) {
      std::cerr << "printGetPost failed: " << e.what() << std::endl;
    };
    break;
  case 2:
    std::cout << options[option - 1] << std::endl;
    try {
      spgxyz::doProcess();
    } catch (std::runtime_error e) {
      std::cerr << "doProcess failed: " << e.what() << std::endl;
    };
    break;
  case 3:
    std::cout << options[option - 1] << std::endl;
    try {
      spgxyz::generateTestXML();
    } catch (std::runtime_error e) {
      std::cerr << "generateTestXML failed: " << e.what() << std::endl;
    };
    break;
  case 4:
    std::cout << options[option - 1] << std::endl;
    try {
      spgxyz::generatePrintXMLTags();
    } catch (std::runtime_error e) {
      std::cerr << "generatePrintXMLTags: " << e.what() << std::endl;
    };
    break;
  case 5:
    std::cout << options[option - 1] << std::endl;
    try {
      spgxyz::abcritCalcs();
    } catch (std::runtime_error e) {
      std::cerr << "abcritCalcs: " << e.what() << std::endl;
    };
    break;
  case 6:
    std::cout << options[option - 1] << std::endl;
    try {
      spgxyz::readDBBinary(spgxyz::inf, spgxyz::bft);
    } catch (std::runtime_error e) {
      std::cerr << "readDBBinary: " << e.what() << std::endl;
    };
    break;
  case 7:
    std::cout << options[option - 1] << std::endl;
    try {
      std::string idFile = onlyValid ? idValidFile : idAllFile;
      auto df = getenv("USERPROFILE") + daDir + idFile;
      std::cout << "Using ids from: " << df << std::endl;
      auto s_ids = spgxyz::readFile(df);
      std::vector<int> valid_ids;
      std::transform(s_ids.begin(), s_ids.end(), std::back_inserter(valid_ids),
                     [](std::string const &s) -> int { return std::stoi(s); });

      auto tmp = spgxyz::readData(
          spgxyz::getCredentials(spgxyz::cgiDir + spgxyz::credentialsFile),
          "stegzzz", valid_ids);
      if (summariseData) { // print onscreen summary before writing
        for (auto &mysqld : tmp) {
          mysqld.printData();
          std::cout << "average small idps" << std::endl;
          for (auto &d : mysqld.getIDPs(true))
            std::cout << d << "\t";
          std::cout << std::endl;
          std::cout << "average large idps" << std::endl;
          for (auto &d : mysqld.getIDPs(false))
            std::cout << d << "\t";
          std::cout << std::endl;
        }
      }

      dataFile = stage1DF;
      std::ofstream ouf{getenv("USERPROFILE") + daDir + dataFile};
      for (auto &mysqld : tmp)
        ouf << mysqld.getDataString() << "\n";
      ouf.flush();
      ouf.close();

      dataFile = stage2DF;
      std::ofstream ouf1{getenv("USERPROFILE") + daDir + dataFile};
      for (auto &mysqld : tmp)
        ouf1 << mysqld.getDataStringS2() << "\n";
      ouf1.flush();
      ouf1.close();

      std::cout << "\n"
                << valid_ids.size()
                << " s1 values (ks etc.) written to: " << daDir + stage1DF
                << "\ns2 values (delay, abc etc.) written to: "
                << daDir + stage2DF << " bye!" << std::endl;
    } catch (std::exception e) {
      std::cout << "readData: " << e.what() << std::endl;
    };
    break;
  case 0:
    try {
      spgxyz::runCGI();
    } catch (std::runtime_error e) {
      std::cerr << "runCGI failed: " << e.what() << std::endl;
    };
    break;
  default:
    std::cout << "__\nInvalid option ID\n" << std::endl;
    spgxyz::help(desc);
    break;

  } // switch(testID)

  return 0;
} // main

namespace spgxyz {
void listOptions() {
  std::cout << "__\n" << __func__ << std::endl;
  std::cout << "Available options ddgem:-" << std::endl;
  for (auto s : options)
    std::cout << "\t" << s << std::endl;
}

void help(po::options_description const &desc) {
  std::cout << helpStr << std::endl;
  std::cout << desc << std::endl;
  listOptions();
}

void printPost() {
  std::cout << "__\n" << __func__ << std::endl;
  cgicc::Cgicc formData;

  formData.restore(cgiDir + envFile);
  tinyxml2::XMLDocument doc{};
  if (doc.Parse(formData.getEnvironment().getPostData().c_str()) !=
      tinyxml2::XML_SUCCESS)
    throw std::runtime_error{std::string{"Parse xml string failed in "} +
                             __func__};
  tinyxml2::XMLPrinter printer{};
  doc.Print(&printer);
  std::cout << std::string{printer.CStr()};
}

void doProcess() {
  std::cout << "__\n" << __func__ << std::endl;
  cgicc::Cgicc formData;
  formData.restore(cgiDir + envFile);
  tinyxml2::XMLDocument doc{};
  if (doc.Parse(formData.getEnvironment().getPostData().c_str()) !=
      tinyxml2::XML_SUCCESS)
    throw std::runtime_error{std::string{"Parse xml string failed in "} +
                             __func__};
  auto root = doc.RootElement();
  ID = std::stoi(root->GetText());
  process(doc);
}

void runCGI() {

  cgicc::Cgicc formData;
  std::cout << preHTMLString;

  auto post = formData.getEnvironment().getPostData();

  tinyxml2::XMLDocument doc{};
  if (doc.Parse(post.c_str()) != tinyxml2::XML_SUCCESS) {
    xmlParseError(post);
    return;
  }

  if (doc.RootElement()->FirstChildElement()->Attribute("save", "true"))
    formData.save(envFile);
  auto root = doc.RootElement();
  ID = std::stoi(root->GetText());

  process(doc);
  std::cout << postHTMLString;

  // #endif // DEBUG
}

std::string process(tinyxml2::XMLDocument const &doc) {
  std::function<int(tinyxml2::XMLDocument const &doc, int const sz,
                    int const szID, double const d, int const am)>
      ch = choice;
  auto data = parseDDT(doc, ch);

  // NB data is only here, subsequent code uses const & to access
  // printData(data);
  mlk mlkSmall{data[0], data[1], smallAmounts[smallAmounts.size() - 1]};
  mlk mlkLarge{data[2], data[3], largeAmounts[largeAmounts.size() - 1]};

  lsk const &lsqkSmall = static_cast<lsk const &>(mlkSmall);
  lsk const &lsqkLarge = static_cast<lsk const &>(mlkLarge); // slicing upcast

  /**
   * @brief bool OKjb johnson and bickel indifference point inclusion criteria
   */
  bool OKjb = (lsqkSmall.getInclusionCheck() && lsqkLarge.getInclusionCheck());

  /**
   * @brief bool OKek exponential k inclusion criterion kes > kel
   */
  auto c1 = mlkSmall(0);
  auto c2 = mlkLarge(0);
  bool OKek = c1 > 0 && c2 > 0 && c1 > c2;

  mysqlData packet{ID};
  if (OKjb) {
    packet.setkS(lsqkSmall.getksAndSSDs(), lsqkLarge.getksAndSSDs());
    packet.setKSML(mlkSmall.getksAndMLs(), mlkLarge.getksAndMLs());
    packet.setGS(mlkSmall.getGs(), mlkLarge.getGs());
  }

  packet.setIDPs(lsqkSmall.getIndifferencePoints(),
                 lsqkLarge.getIndifferencePoints());
  tinyxml2::XMLPrinter printer{};
  doc.Print(&printer);
  packet.setXML(printer.CStr(), printer.CStrSize());
  packet.setCrit(OKjb, OKek);

#if CHKABC == true
  /*
  data from e178.r pltPRZsab params D from plot (could be got inside eqPRZPlt)
  ID 407
  hyp d=166 D=262 k=0.022 K=0.016 a=742 ab=12367.7
  exp d=121 D=154 k=0.017 K=0.012 a=900 ab=9010.2

  ID=558
  hyp d=63 D=99 k=0.059 K=0.042 a=746 ab=12368
  exp d=72 D=91 k=0.028 K=0.02 a=900 ab=9010.2

  ID=587
  hyp d=180 D=389 k=0.00475172 K=0.003394087 a=708 ab=6062
  exp d=180 D=252 k=0.00413467 K=0.002953336 a=900 ab=4354

  ID=471
  hyp d=11.6 D=18.4 k=0.3238239 K=0.2313027795 a=738 ab=12366.1
  exp d=6.09 D=7.8 k=0.3319061 K=0.2370758 a=900 ab=9010.1
  */
  struct indat {
    double const m; //
    double const A;
    double const x;
    double const k;
    double const K;
    double const abc;
    bool const exp;
    int const id;
    double const d;
    indat(double x, double K, int id, double abc, double d, bool exp = false,
          double m = 1.4)
        : x{x}, K{K}, id{id}, abc{abc}, d{d}, k{m * K}, exp{exp}, m{m},
          A{1000} {}
  };
  std::vector<indat> testABCData;
  testABCData.push_back(indat{0.742, 0.016, 407, 12367.7, 166});
  testABCData.push_back(indat{0.9, 0.012, 407, 9010.2, 121, true});
  testABCData.push_back(indat{0.746, 0.042, 558, 12368, 63});
  testABCData.push_back(indat{0.9, 0.02, 558, 9010.2, 72, true});
  testABCData.push_back(indat{0.708, 0.003394087, 587, 6062, 180});
  testABCData.push_back(indat{0.9, 0.002953336, 587, 4354, 180, true});
  testABCData.push_back(indat{0.738, 0.2313027795, 471, 12366.1, 11.6});
  testABCData.push_back(indat{0.9, 0.2370758, 471, 9010.1, 6.1, true});

  for (auto &indat : testABCData) {
    auto LD = indat.exp ? &LDe : &LDh;
    auto UD = indat.exp ? &UDe : &UDh;
    auto ddf = indat.exp ? &sedd : &shdd;
    auto mod = indat.exp ? "exp " : "hyp ";
    abcritOptBoost abh{indat.K,           LD,     UD, ddf, indat.A,
                       indat.k / indat.K, indat.x};
    std::cout << mod << " k= " << indat.k << " K= " << indat.K
              << " (m= " << indat.k / indat.K << " ID= " << indat.id << ")"
              << std::endl;
    for (auto d : {5, 10, 45, 80, 150, 180, 225, 300, 400})
      std::cout << "abcrit= " << -abh(d) << " with d= " << d << std::endl;
    abh.maximise();
    std::cout << "Opt d = " << abh.getMaxParam()
              << " abcrit = " << abh.getMaxABCrit()
              << " against r test value d " << indat.d << " and ABC "
              << indat.abc << std::endl
              << "___" << std::endl;
  }

#endif

  if (OKek) {
    double k = mlkSmall();
    double K = mlkLarge();
    double A = mlkLarge.getLLAmount();
    double a = mlkSmall.getLLAmount();
    packet.setS2Data(getStage2Values(k, K, a, A, &LDh, &UDh, &shdd));
    k = mlkSmall(0);
    K = mlkLarge(0);
    packet.setS2Data(getStage2Values(k, K, a, A, &LDe, &UDe, &sedd), false);
  }

  storeData(getCredentials(cgiDir + credentialsFile), "stegzzz", packet);

  sendResponse(packet);

  return "done";
}

std::vector<double> getStage2Values(double const k, double const K,
                                    double const a, double const A,
                                    lfptr const l, ufptr const ub,
                                    ddfptr const d) {
  std::vector<double> result;
  abcritOptBoost ab{K, l, ub, d, A, k / K, a / A};
  ab.maximise();
  result.push_back(ab.getMaxParam());  // d
  result.push_back(ab.getMaxABCrit()); // abc
  result.push_back(
      l(K, a / A, result[static_cast<std::size_t>(s2idx::d)])); // ld
  result.push_back(
      ub(k, K, a / A, result[static_cast<std::size_t>(s2idx::d)])); // ud
  result.push_back((result[static_cast<std::size_t>(s2idx::d)] +
                    result[static_cast<std::size_t>(s2idx::ld)]) /
                   2); // ll1
  result.push_back((result[static_cast<std::size_t>(s2idx::ld)] -
                    result[static_cast<std::size_t>(s2idx::d)]) /
                   2); // ll2
  result.push_back((result[static_cast<std::size_t>(s2idx::ud)] +
                    result[static_cast<std::size_t>(s2idx::ld)]) /
                   2); // ls1
  result.push_back(result[static_cast<std::size_t>(s2idx::ls1)] -
                   result[static_cast<std::size_t>(s2idx::d)]); // ls2
  result.push_back(2 * result[static_cast<std::size_t>(s2idx::ud)] -
                   result[static_cast<std::size_t>(s2idx::ld)]); // ss1
  result.push_back(2 * result[static_cast<std::size_t>(s2idx::ud)] -
                   result[static_cast<std::size_t>(s2idx::ld)] -
                   result[static_cast<std::size_t>(s2idx::d)]); // ss2
#if S2CHCHECK == true
  checkS2Choices(result, k, K, a, A, d);
#endif
  return result;
}

int oldChoice(tinyxml2::XMLDocument const &doc, int const sz, int const szID,
              double const d, int const am) {
  auto eptr = doc.RootElement(); // check for errors
  eptr = eptr->FirstChildElement(sz == 0 ? "small" : "large");
  while (!(eptr->Attribute("id", std::to_string(szID).c_str()))) {
    eptr = eptr->NextSiblingElement(sz == 0 ? "small" : "large");
  }
  eptr = eptr->FirstChildElement("delay");
  while (!compare(eptr->Attribute("id"), d))
    eptr = eptr->NextSiblingElement("delay");
  eptr = eptr->FirstChildElement("amount");
  while (!(eptr->Attribute("am", std::to_string(am).c_str())))
    eptr = eptr->NextSiblingElement("amount");
  eptr = eptr->FirstChildElement("choice");
  auto c = eptr->GetText();
  int result = (*c == 'S' ? 0 : *c == 'L' ? 1 : -1);
  return result;
}

int choice(tinyxml2::XMLDocument const &doc, int const sz, int const szID,
           double const d, int const am) {

  auto tag = getXMLTag(sz, szID, d, am);
  auto eptr = doc.RootElement(); // check for errors
  eptr = eptr->FirstChildElement("data");
  while (strcmp(eptr->Attribute("attr"), tag.c_str())) {
    eptr = eptr->NextSiblingElement();
    if (!eptr)
      throw std::runtime_error{
          std::string{"could not find xml element matching tag " + tag +
                      " in " + __func__}};
  }
  auto c = eptr->GetText();
  int result = (*c == '1' ? 0 : *c == '2' ? 1 : -1);
  return result;
}

std::string getXMLTag(int const sz, int const szID, double const d,
                      int const am) {
  std::ostringstream os("S1", std::ios_base::ate);
  // int idx = (szID - 1) * 2 * nd * na; // series
  int idx = sz * nd * na; // size
  auto it =               // delay, find index of d in delay array
      std::find_if(delays.begin(), delays.end(),
                   [&d](double const target) { return compare(target, d); });
  if (it == delays.end())
    throw std::runtime_error{std::string{
        "Couldn't find delay d= " + std::to_string(d) + " in " + __func__}};
  int didx = it - delays.begin();
  idx += didx * na;

  // amount, find index of am in amount array
  auto &amountArray = sz == 0 ? smallAmounts : largeAmounts;
  auto ita = // amount, find index of am in amount array
      std::find_if(amountArray.begin(), amountArray.end(),
                   [&am](double const target) { return compare(target, am); });
  if (ita == amountArray.end())
    throw std::runtime_error{std::string{
        "Couldn't find amount= " + std::to_string(am) + " in " + __func__}};
  int aidx = ita - amountArray.begin();

  idx += aidx;
  os << std::setw(2) << std::setfill('0') << szID << "_" << std::setw(2)
     << std::setfill('0') << idx + 1;
  return os.str();
}

void printXMLElementInfo(tinyxml2::XMLElement const *ep, int neighbours) {
  if (ep) {
    std::cout << "Name\t" << ep->Name() << "\tValue\t" << ep->Value()
              << std::endl;
    auto att = ep->FirstAttribute();
    if (att)
      std::cout << "AName\t" << att->Name() << "\tAValue\t" << att->Value()
                << std::endl;
  } else
    std::cout << "Null" << std::endl;

  if (neighbours == 1) {
    auto buffEPtr = ep->NextSiblingElement();
    std::cout << "nextSib" << std::endl;
    printXMLElementInfo(buffEPtr, 0);
    buffEPtr = ep->PreviousSiblingElement();
    std::cout << "previousSib" << std::endl;
    printXMLElementInfo(buffEPtr, 0);
    buffEPtr = ep->FirstChildElement();
    std::cout << "child" << std::endl;
    printXMLElementInfo(buffEPtr, 0);
  }
  std::cout << "__" << std::endl;
}

bool compare(char const *val, double const d, double const tol) {
  auto vald = std::stod(val);
  if (std::abs(vald - d) < tol)
    return true;
  return false;
}

bool compare(double const d1, double const d2, double const tol) {
  if (std::abs(d1 - d2) < tol)
    return true;
  return false;
}

void xmlParseError(std::string &xml) {
  auto msgTime =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::ofstream ouf(cgiDir + badXMLFile,
                    std::ofstream::out | std::ofstream::app);
  ouf << xml << std::endl
      << "Error at" << std::endl
      << std::ctime(&msgTime) << std::endl;
  ouf.close();
  std::cout << std::string{"Parse xml string failed in "} + __func__ + xml +
                   "any xml?"
            << postHTMLString << std::endl;
}

std::pair<std::string, std::string> getCredentials(std::string const &f) {
  std::ifstream inf{f};
  if (!inf)
    throw std::runtime_error{
        std::string{"Could not open " + f + " in " + __func__}};
  std::string u, p;
  inf >> u >> p;
  inf.close();
  return std::make_pair(u, p);
}

std::vector<ddtseq>
parseDDT(tinyxml2::XMLDocument const &doc,
         std::function<int(tinyxml2::XMLDocument const &doc, int const sz,
                           int const szID, double const d, int const am)>
             ch) {

  std::vector<ddtseq> result;

  int offset = 0;
  int base = 0;

  for (int s = 0; s < 2; ++s) {   // size small=0, large=1
    for (int i = 1; i < 3; i++) { // series id first=1, second=2
      std::array<ddt, npseq> abuff;
      for (int d = 0; d < nd; d++) {
        auto &amountArray = s == 0 ? smallAmounts : largeAmounts;
        for (int a = 0; a < na; a++) {
          ddt ddtbuff;
          ddtbuff.amount = amountArray[a];
          ddtbuff.delay = delays[d];
          ddtbuff.choice = ch(doc, s, i, delays[d], amountArray[a]);
          abuff[base + offset] = ddtbuff;
          ++offset;
        }
        offset = 0;
        base += na;
      } // for (auto d : delayIDs) {
      result.push_back(abuff);
      base = 0;
    } // for (int i = 1; i < 3; i++) {
  }   // for (int s = 0; s < 2; ++s) {
  return result;
}

void printData(std::vector<ddtseq> const &data) {
  for (auto a : data) {
    for (auto d : a) {
      std::cout << d.amount << "\t" << d.delay << "\t" << d.choice << std::endl;
    }
    std::cout << "__" << std::endl;
  }
}

bool storeData(std::pair<std::string, std::string> const &credentials,
               std::string const &dbname, mysqlData const &data) {
  auto [un, pw] = credentials;
  mysqlx::Session sess("localhost", 33060, un, pw);
  mysqlx::Schema db = sess.getSchema(dbname);
  auto table = db.getTable("ddgem");
  auto xmlTable = db.getTable("ddgemxml");
  sess.startTransaction();
  try {
    auto s = data.getKSS();
    auto l = data.getKSL();
    auto sm = data.getKSSML();
    auto lm = data.getKSLML();
    auto gs = data.getGS();
    auto gl = data.getGL();
    auto [cjb, cek] = data.getIncCriteria();
    // clang-format off
    if(cjb){//only write ks etc if cjb (they will only have been calculated if cjb)
    table.insert("pid","critJB", "critExpK",
                 "kh_S", "ke_S", "kh_L", "ke_L",
                 "kh_Sssd", "ke_Sssd", "kh_Lssd", "ke_Lssd",
                 "khm_S", "kem_S", "khm_L", "kem_L",
                 "kh_SNLL", "ke_SNLL", "kh_LNLL", "ke_LNLL",
                 "ghs", "ges", "ghl","gel"
    ).values(data.getID(),cjb,cek,s[0], s[2], l[0], l[2],
                                  s[1], s[3], l[1], l[3],
                                  sm[0], sm[2], lm[0], lm[2],
                                  sm[1], sm[3], lm[1], lm[3],
                                  gs.first,gs.second,gl.first,gl.second).execute();
    } else {
       table.insert("pid","critJB", "critExpK").values(data.getID(),cjb,cek).execute();
    }

    xmlTable.insert("pid","xml","idps","s2data").values(data.getID(),data.getXML(),data.getIDPS(),data.getS2Data()).execute();
    // clang-format on
    sess.commit();

  } catch (const mysqlx::Error &err) {
    // Rollback the transaction in case of an error
    sess.rollback();
    std::cerr << "Database error: " << err.what() << " in " << __func__
              << std::endl;
    return false;
  }

  return true;
}

std::vector<mysqlData>
readData(std::pair<std::string, std::string> const &credentials,
         std::string const &dbname, std::vector<int> const &ids) {
  auto [un, pw] = credentials;
  mysqlx::Session sess("localhost", 33060, un, pw);
  mysqlx::Schema db = sess.getSchema(dbname);
  auto table = db.getTable("ddgem");
  auto xmlTable = db.getTable("ddgemxml");
  // sess.startTransaction();
  using idx = dbColIdx;
  using idxx = dbColIdxXML;
  std::vector<mysqlData> result;
  try {
    for (auto id : ids) {
      auto myResult =
          table.select("*").where("pid=:id").bind("id", id).execute();

      auto row = myResult.fetchOne();
      mysqlData buff{row[1]}; // initialise buff with id
      buff.setkS({row[static_cast<int>(idx::kh_S)],
                  row[static_cast<int>(idx::kh_Sssd)],
                  row[static_cast<int>(idx::ke_S)],
                  row[static_cast<int>(idx::ke_Sssd)]},
                 {row[static_cast<int>(idx::kh_L)],
                  row[static_cast<int>(idx::kh_Lssd)],
                  row[static_cast<int>(idx::ke_L)],
                  row[static_cast<int>(idx::ke_Lssd)]});

      buff.setKSML({row[static_cast<int>(idx::khm_S)],
                    row[static_cast<int>(idx::kh_SNLL)],
                    row[static_cast<int>(idx::kem_S)],
                    row[static_cast<int>(idx::ke_SNLL)]},
                   {row[static_cast<int>(idx::khm_L)],
                    row[static_cast<int>(idx::kh_LNLL)],
                    row[static_cast<int>(idx::kem_L)],
                    row[static_cast<int>(idx::ke_LNLL)]});

      buff.setGS(std::make_pair(row[static_cast<int>(idx::ghs)],
                                row[static_cast<int>(idx::ges)]),
                 std::make_pair(row[static_cast<int>(idx::ghl)],
                                row[static_cast<int>(idx::gel)]));

      buff.setCrit(row[static_cast<int>(idx::critJB)],
                   row[static_cast<int>(idx::critExpK)]);

      auto rowx = xmlTable.select("*")
                      .where("pid=:id")
                      .bind("id", id)
                      .execute()
                      .fetchOne();

      buff.setIDPs(rowx.getBytes(static_cast<int>(idxx::idps)));
      buff.setS2Data(rowx.getBytes(static_cast<int>(idxx::s2data)));

      result.push_back(mysqlData{std::move(buff)});
    }
  } catch (const mysqlx::Error &err) {
    // Rollback the transaction in case of an error
    //  sess.rollback();
    std::cerr << "Database error: " << err.what() << " in " << __func__
              << std::endl;
  }
  return result;
}

std::vector<std::string> readFile(const std::string &f) {
  std::vector<std::string> result;
  std::ifstream inf(f);
  if (!inf)
    throw std::runtime_error{
        std::string{"Could not open file " + f + " "}.append(__func__)};
  std::string buff;
  while (std::getline(inf, buff))
    result.push_back(buff);
  inf.close();
  return result;
} // std::vector<std::string> readFile(const std::string& f)

void generateTestXML() {
  try {
    TestXMLGenerator txg(
        k_test, g_test, std::tie(delays, smallAmounts, largeAmounts),
        exponentialModel_test, generatedXMLID, xmlTestDataOutputFile);
    txg.saveXMLData();
    std::cout << "generated xml written to " << xmlTestDataOutputFile
              << std::endl;
    if (randomXML) {

      txg.randomisePreXMLData();
      auto rfn = xmlTestDataOutputFile;
      rfn.insert(rfn.find("."), "_rnd");
      txg.refreshXML();
      txg.saveXMLData(rfn);
      std::cout << "randomised generated xml written to " << rfn << std::endl;
    }
    /* std::cout << "original order" << std::endl;
    txg.printPreXMLData();
    txg.randomisePreXMLData();
    std::cout << "randomised order" << std::endl;
    txg.printPreXMLData();
    txg.orderPreXMLData();
    std::cout << "reordered order" << std::endl;
    txg.printPreXMLData(); */
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    throw;
  }
}

void generatePrintXMLTags() {
  int n = 0;
  for (int i = 1; i < 3; i++) {   // series id first=1, second=2
    for (int s = 0; s < 2; ++s) { // size small=0, large=1
      for (int d = 0; d < nd; d++) {
        auto &amountArray = s == 0 ? smallAmounts : largeAmounts;
        for (int a = 0; a < na; a++) {
          std::cout << ++n << ")\tseries " << i << "\tnow " << std::setw(5)
                    << amountArray[a] << "\tlater sz " << std::setw(5)
                    << amountArray[na - 1] << "\tdelay " << std::setw(5)
                    << delays[d] << "\t"
                    << getXMLTag(s, i, delays[d], amountArray[a]) << std::endl;
        }
      } // for (auto d : delayIDs) {
    }   // for (int i = 1; i < 3; i++) {
  }     // for (int s = 0; s < 2; ++s) {
} // void generatePrintXMLTags() {

void abcritCalcs() {
  lfptr lb = exponentialModel_test ? LDe : LDh;
  ufptr ub = exponentialModel_test ? UDe : UDh;
  ddfptr dd = exponentialModel_test ? sedd : shdd;
  abcrit abc(K_test, lb, ub, dd, A_test, k_test / K_test);
  std::vector<double> ds = {5, 10, 20, 50, 100};
  for (auto d : ds) {
    std::cout << "ab= " << abc(0.82, d) << " with d= " << d << " K= " << K_test
              << " k= " << k_test << std::endl;
  }
}

mysqlData::mysqlData(int pid) : id{pid} {}

void mysqlData::setkS(kAndFit &&small, kAndFit &&large) {
  kss = std::move(small);
  ksl = std::move(large);
  return;
}

void mysqlData::setKSML(kAndFit &&small, kAndFit &&large) {
  kssml = std::move(small);
  kslml = std::move(large);
  return;
}

void mysqlData::setGS(std::pair<double, double> &&small,
                      std::pair<double, double> &&large) {
  gs = std::move(small);
  gl = std::move(large);
}

void mysqlData::setIDPs(std::vector<idpValues> &&small,
                        std::vector<idpValues> &&large) {
  idpsS = std::move(small);
  idpsL = std::move(large);
  setIDPBytes();
  idpPtr = reinterpret_cast<unsigned char const *>(idpBytes.data());
}

void mysqlData::setIDPs(mysqlx::bytes const &idpb) {
  auto dp = reinterpret_cast<double const *>(idpb.begin());
  auto lambda = [&dp]() {
    std::vector<idpValues> result;
    for (int i = 0; i < 3; ++i) {
      idpValues buff;
      for (auto &d : buff)
        d = *dp++;
      result.push_back(buff);
    }
    return result;
  };
  auto small = lambda(); // the order of these calls is critical as is the
                         // lambda incrementing of dp
  auto large = lambda();
  setIDPs(std::move(small), std::move(large));
}

void mysqlData::setS2Data(std::vector<double> &&d, bool h) {
  if (h)
    s2h = std::move(d);
  else
    s2e = std::move(d);
  setS2Bytes(h);
  s2Ptr = reinterpret_cast<unsigned char const *>(s2Bytes.data());
}

void mysqlData::setS2Data(mysqlx::bytes const &s2dat) {
  if (s2dat.begin() == s2dat.end()) { // there is no s2 data
    setS2Data(std::vector<double>(10, -99), true);
    setS2Data(std::vector<double>(10, -99), false);
    return;
  }
  auto dp = reinterpret_cast<double const *>(s2dat.begin());
  auto lambda = [&dp]() {
    std::vector<double> result;
    for (int i = 0; i < 10; ++i)
      result.push_back(*dp++);
    return result;
  };
  auto hyp = lambda(); // the order of these calls is critical as is the
                       // lambda incrementing of dp
  auto exp = lambda();
  setS2Data(std::move(hyp), true);
  setS2Data(std::move(exp), false);
}

void mysqlData::setIDPBytes() {
  for (auto &a : idpsS)
    for (auto d : a)
      idpBytes.push_back(d);
  for (auto &a : idpsL)
    for (auto d : a)
      idpBytes.push_back(d);
}

void mysqlData::setS2Bytes(bool h) {
  if (h) {
    for (auto d : s2h)
      s2Bytes.push_back(d);
  } else {
    for (auto d : s2e)
      s2Bytes.push_back(d);
  }
}

void mysqlData::setXML(char const *xp, int sz) {
  XMLPtr = reinterpret_cast<unsigned char const *>(xp);
  XMLSz = sz;
  return;
}

void mysqlData::setCrit(int jb, int ek) {
  critJB = jb;
  critExpK = ek;
  return;
}

std::string mysqlData::getDataStringS2() {
  std::string result;
  result.reserve(512);
  std::ostringstream os{result};
  os << id << "\t" << critJB << "\t" << critExpK << "\t";
  for (auto d : s2h)
    os << d << "\t";
  for (auto d : s2e)
    os << d << "\t";
  return os.str();
}

std::string mysqlData::getDataString() {
  std::string result;
  result.reserve(512);
  std::ostringstream os{result};
  os << id << "\t" << critJB << "\t" << critExpK << "\t";
  auto pkss = [&os](spgxyz::kAndFit const &kaf) {
    for (auto &v : kaf)
      os << v << "\t";
  };
  pkss(kss);
  pkss(ksl);
  pkss(kssml);
  pkss(kslml);
  os << gs.first << "\t" << gs.second << "\t" << gl.first << "\t" << gl.second
     << "\t";
  for (int i = 0; i < 5; ++i)
    os << idpBytes[i + 10] << "\t"; // average small indifference points
  for (int i = 15; i < 20; ++i)
    os << idpBytes[i + 10] << "\t"; // average large indifference points

  return os.str();
}

void mysqlData::printData() {
  auto [jb, ek] = getIncCriteria();
  std::cout << "\n"
            << getID() << "\tjb crit= " << jb << "\tek crit= " << ek << "\n";
  auto pkss = [](spgxyz::kAndFit const &kaf) { // print k and fit values
    for (auto &v : kaf)
      std::cout << v << "\t";
    std::cout << std::endl;
  };
  std::cout << "kss:\n";
  pkss(getKSS());
  std::cout << "ksl:\n";
  pkss(getKSL());
  std::cout << "kssml:\n";
  pkss(getKSSML());
  std::cout << "kslml:\n";
  pkss(getKSLML());
  auto pgss = [](std::pair<double, double> const &gs) { // print gs
    std::cout << gs.first << "\t" << gs.second << std::endl;
  };
  std::cout << "gs:\n";
  pgss(getGS());
  std::cout << "gl:\n";
  pgss(getGL());
  std::cout << "indifference points small:\n";
  for (int i = 0; i < 5; ++i) {
    std::cout << idpBytes[i] << "\t" << idpBytes[i + 5] << "\t"
              << idpBytes[i + 10] << "\n";
  }
  std::cout << "indifference points large:\n";
  for (int i = 15; i < 20; ++i) {
    std::cout << idpBytes[i] << "\t" << idpBytes[i + 5] << "\t"
              << idpBytes[i + 10] << "\n";
  }
  std::cout << "s2data hyperbolic -- exponential:\n";
  auto currentLabel = spgxyz::s2idx::d;
  for (int i = 0; i < s2h.size(); ++i) {
    std::cout << s2idxToString(currentLabel) << ":\t" << s2h[i] << "\t--\t"
              << s2e[i] << "\n";
    ++currentLabel;
  }
  std::cout.flush();
} // void mysqlData::printData() {

void sendResponse(mysqlData const &my) {
  auto [jb, ek] = my.getIncCriteria();
  std::string status{(jb && ek) ? "OK" : "STOP"};
  auto hdat = my.getS2Data(true);
  if (!(jb && ek)) {
    std::cout << status;
    for (int i = 0; i < 14; ++i)
      std::cout << "\t-99";
    std::cout << std::flush;
    return;
  }
  std::cout << status << "\t" << hdat[static_cast<int>(s2idx::d)] << "\t";
  for (int i = static_cast<int>(s2idx::ll1); i <= static_cast<int>(s2idx::ss2);
       i++)
    std::cout << hdat[i] << "\t";
  auto edat = my.getS2Data(false);
  std::cout << edat[static_cast<int>(s2idx::d)] << "\t";
  for (int i = static_cast<int>(s2idx::ll1); i <= static_cast<int>(s2idx::ss2);
       i++)
    if (i < static_cast<int>(s2idx::ss2))
      std::cout << edat[i] << "\t";
    else
      std::cout << edat[i] << std::flush;
}

void checkS2Choices(std::vector<double> const &s2data, double const k,
                    double const K, double const a, double const A,
                    ddfptr const d) {
  if (d == &shdd)
    std::cout << "checking S2 choice data, hyperbolic model" << std::endl;
  if (d == &sedd)
    std::cout << "checking S2 choice data, exponential model" << std::endl;

  auto f = [](double ss, double ll, bool ok) {
    std::cout << "VSS= " << ss << "\tVLL= " << ll << (ok ? " OK" : " FAIL!")
              << std::endl;
  };
  // LL1
  auto const ss = d(a, k, s2data[static_cast<std::size_t>(s2idx::d)]);
  auto ll = d(A, K, s2data[static_cast<std::size_t>(s2idx::ll1)]);
  std::cout << "LL1" << std::endl;
  f(ss, ll, ll > ss);
  // LL2
  ll = d(A, K, s2data[static_cast<std::size_t>(s2idx::ll2)]);
  std::cout << "LL2" << std::endl;
  f(a, ll, ll > a);
  // LS1
  ll = d(A, K, s2data[static_cast<std::size_t>(s2idx::ls1)]);
  std::cout << "LS1" << std::endl;
  f(ss, ll, ll > ss);
  // LS2
  ll = d(A, K, s2data[static_cast<std::size_t>(s2idx::ls2)]);
  std::cout << "LS2" << std::endl;
  f(a, ll, a > ll);
  // SS1
  ll = d(A, K, s2data[static_cast<std::size_t>(s2idx::ss1)]);
  std::cout << "SS1" << std::endl;
  f(ss, ll, ss > ll);
  // SS2
  ll = d(A, K, s2data[static_cast<std::size_t>(s2idx::ss2)]);
  std::cout << "SS2" << std::endl;
  f(a, ll, a > ll);
}

void readDBBinary(std::string f, spgxyz::binFileType type) {
  if (f == "")
    throw std::runtime_error(std::string{} +
                             "file name required for option 6 in " + __func__);
  assert(getEnumValue(spgxyz::bft) >= 0 && getEnumValue(spgxyz::bft) < 3 &&
         "binary file type out of range");
  std::cout << "Reading binary file " << f << std::endl;
  std::ifstream inf{f, std::ios_base::binary};
  std::vector<char> data(std::istreambuf_iterator<char>(inf), {});
  inf.close();

  switch (type) {
  case spgxyz::binFileType::xml: {
    std::cout << "xml follows\n";
    std::string str(data.data(), data.size());

    tinyxml2::XMLDocument doc{};
    if (doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS)
      throw std::runtime_error{std::string{"Parse xml string failed in "} +
                               __func__};

    tinyxml2::XMLPrinter printer{};
    doc.Print(&printer);
    std::cout << std::string{printer.CStr()};
    std::cout << "\nxml ends\n"
              << " now check the S1 questions\n";
    int n = 0;
    for (int i = 1; i < 3; i++) {   // series id first=1, second=2
      for (int s = 0; s < 2; ++s) { // size small=0, large=1
        for (int d = 0; d < nd; d++) {
          auto &amountArray = s == 0 ? smallAmounts : largeAmounts;
          for (int a = 0; a < na; a++) {
            std::cout << ++n << ")\tseries " << i << "\tnow " << std::setw(5)
                      << amountArray[a] << "\tlater sz " << std::setw(5)
                      << amountArray[na - 1] << "\tdelay " << std::setw(5)
                      << delays[d] << "\tchoice "
                      << choice(doc, s, i, delays[d], amountArray[a]) << "\t"
                      << getXMLTag(s, i, delays[d], amountArray[a]) << "\n";
          }
        } // for (auto d : delayIDs) {
      }   // for (int i = 1; i < 3; i++) {
    }     // for (int s = 0; s < 2; ++s) {
    std::cout << std::endl;
  } break;
  case spgxyz::binFileType::idps: {
    std::cout << "idps follows\n";
    std::vector<std::string> series{"S1small", "S2small", "Avgsmall",
                                    "S1large", "S2large", "Avglarge"};
    auto currentSeriesLabel = series.begin();
    auto itDelay = delays.begin();
    auto itDelayEnd = delays.end();
    auto it = data.data();
    auto ite = it + data.size();
    while (it < ite) {
      std::cout << *(reinterpret_cast<double *>(it)) << "\tdelay: " << *itDelay
                << "\tseries: " << *currentSeriesLabel << "\n";
      it += sizeof(double);
      itDelay++;
      if (itDelay == itDelayEnd) {
        itDelay = delays.begin();
        ++currentSeriesLabel;
      }
    }
    std::cout << std::endl;

  } break;
  case spgxyz::binFileType::s2data: {
    std::cout << "s2data follows\nhyperbolic\n";
    auto currentLabel = spgxyz::s2idx::d;
    auto it = data.data();
    auto ite = it + data.size();
    int hcount = 0;
    while (it < ite) {
      if (hcount++ == 10)
        std::cout << "\nexponential\n";
      std::cout << s2idxToString(currentLabel) << ":\t "
                << *(reinterpret_cast<double *>(it)) << "\n";
      it += sizeof(double);
      ++currentLabel;
    }
    std::cout << std::endl;

  } break;
  } // switch
} // void readDBBinary(std::string f, spgxyz::binFileType type) {

std::string s2idxToString(spgxyz::s2idx enumValue) {

  switch (enumValue) {
  case spgxyz::s2idx::d:
    return "delay";
  case spgxyz::s2idx::abc:
    return "abc";
  case spgxyz::s2idx::ld:
    return "ld";
  case spgxyz::s2idx::ud:
    return "ud";
  case spgxyz::s2idx::ll1:
    return "ll1";
  case spgxyz::s2idx::ll2:
    return "ll2";
  case spgxyz::s2idx::ls1:
    return "ls1";
  case spgxyz::s2idx::ls2:
    return "ls2";
  case spgxyz::s2idx::ss1:
    return "ss1";
  case spgxyz::s2idx::ss2:
    return "ss2";
  default:
    assert(false && "invalid value in s2idxToString");
    break;
  } // switch (enumValue) {
} // std::string s2idxToString(spgxyz::s2idx enumValue){

spgxyz::s2idx &operator++(spgxyz::s2idx &idx) {
  switch (idx) {
  case spgxyz::s2idx::d:
    return idx = spgxyz::s2idx::abc;
  case spgxyz::s2idx::abc:
    return idx = spgxyz::s2idx::ld;
  case spgxyz::s2idx::ld:
    return idx = spgxyz::s2idx::ud;
  case spgxyz::s2idx::ud:
    return idx = spgxyz::s2idx::ll1;
  case spgxyz::s2idx::ll1:
    return idx = spgxyz::s2idx::ll2;
  case spgxyz::s2idx::ll2:
    return idx = spgxyz::s2idx::ls1;
  case spgxyz::s2idx::ls1:
    return idx = spgxyz::s2idx::ls2;
  case spgxyz::s2idx::ls2:
    return idx = spgxyz::s2idx::ss1;
  case spgxyz::s2idx::ss1:
    return idx = spgxyz::s2idx::ss2;
  case spgxyz::s2idx::ss2:
    return idx = spgxyz::s2idx::d; // wrap
  default:
    assert(false && "invalid value in operator++(spgxyz::s2idx &idx)");
    break;
  } // switch (enumValue) {
} // spgxyz::s2idx &operator++(spgxyz::s2idx &idx) {
} // namespace spgxyz