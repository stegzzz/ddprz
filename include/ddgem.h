#pragma once
#include "TestXMLGenerator.h"
#include "abcrit.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPPlainHeader.h"
#include "ddFuncs.h"
#include "lsk.h"
#include "mlk.h"
#include "mysqlx/xdevapi.h"
#include "tinyxml2.h"
#include "types.h"
#include <algorithm>
#include <boost/math/tools/minima.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

/**
 * @mainpage Overview
 *
 * The code in this project is for delay discounting experiment <a
 * href=https://osf.io/qnsz7/>In search of the preference reversal zone</a>
 *
 * The primary usage of this program is as a CGI program which recieves data
 * from stage one of the experiment (in an xml packet), processes that data, and
 * returns data for design of experiment stage 2. The program therefore needs to
 * be installed in an appropriate server directory e.g. Apache24/cgi-bin
 *
 * Starting the program (ddgem.exe) without command line parameters runs in CGI
 * mode and will fail from the command line. The program can, however, be run
 * from the command line for development, debugging and testing purposes. In
 * order to do this it is first necessary to save a CGI environment from which
 * the stage 1 xml packet will be read. Configuration of the CGI environment is
 * described in \subpage cgienv.
 *
 * The program has been built and tested in windows 10 using cl (19.28.29337 for
 * x64) and on debian 8.3.0-6 x86-64 using g++ (8.3.0). Makefile and
 * %Makefile_L.mak build using cl and g++ respectively.
 *
 * Running the program with option -h (i.e. ddgem -h) provides brief help on
 * various options and parameters which are available and were used during
 * development.
 *
 * From main(int argc, char *argv[]) the execution paths start at printPost(),
 * doProcess(), generateTestXML(), generatePrintXMLTags(), or runCGI()
 *
 * Core functions/classes
 *
 *   - parseDDT()
 *   - runCGI()
 *   - process()
 *   - spgxyz::lsk
 *   - spgxyz::mlk
 *   - spgxyz::ddgemModel
 *   - spgxyz::ddgemModelBoost
 *   - spgxyz::ddgemModelNLopt
 *   - spgxyz::abcrit
 *   - spgxyz::mysqlData
 *   - ddFuncs.h
 *
 * ## Dependencies
 * The code in this project uses
 *   - <a href="https://github.com/leethomason/tinyxml2">TinyXML-2</a>
 *   - <a href="https://github.com/stevengj/nlopt">NLopt</a>
 *   - <a href="http://www.boost.org/">Boost</a>
 *   - <a href="https://www.gnu.org/software/cgicc/index.html">GNU cgicc</a>
 *
 *
 * An additional program is available (ddParamRec.exe) for simulation of delay
 * discounting and for a parameter recovery investigation see
 * \subpage ddparamrec for further details.
 *
 * @copyright
 * Copyright (C) 2019-21 Steven Glautier <spgxyz@gmail.com> <br>
 * This work is licensed for non-commercial use as follows: <br>
 * Attribution-NonCommercial-ShareAlike 4.0 International. <br>
 * Applies to all code in this project.
 */

/**
 * @page cgienv Setting-up CGI environment for command line usage
 *
 * ddgem -o 2 starts the program in command line mode simulating CGI mode. In
 * order for this to work there needs to be a file ddgemEnv.txt in the cgi
 * directory. This file is created when running in CGI mode if the stage 1 xml
 * packet contains element <saveEnv save="true"/> as a child element of myData
 * root element.
 *
 * The can be achieved by directing a web browser to the file site/index.html
 * which will provide options to site/submit.php in order to deposit the
 * required xml, with GCI environment, into the cgi directory. Local
 * configuration options will be needed.
 *
 */

/**
 * @page ddparamrec DD data generation and parameter recovery
 *
 * Details for generation of simulated delay-discounting data is given under
 * spgxyz::DDDataGenerator and details for parameter recovery from the generated
 * data is given under spgxyz::ddParamRec.
 *
 * The program ddparamrec.exe can be run with default parameters specified in
 * file ddpr.par using ddparamrec -o 1
 *
 * Use ddParamRec.mak to build.
 *
 * Brief help is available using ddparamrec -h
 */

/**
 * \file ddgem.h
 *
 * A collection of utility functions and spgxyz::lsk class
 * declaration, which is the primary class for data processing and computation.
 */

/**
 * \file chkKsetc.r
 *
 * Tests for lsk, cross reference output against r
 *
 * Tests passed testXMLPacket1.xml, testXMLPacket1_rnd.xml, testXMLPacket2.xml,
 * testXMLPacket2_rnd.xml, 1/3/2021
 */

namespace spgxyz {
// run ABCRIT checks
#define CHKABC 0
// run S2Choice checks
#define S2CHCHECK 0
// checks on ddgem program, see also rwork/e178/e178.r,
// latex/work/ddgem/expDD.xlsx, build/S2DataSets.txt, S2CHCHECK defined in
//#ddgem.h

void help(boost::program_options::options_description const &desc);
void listOptions();

/**
 * @brief prints the posted xmlpacket restored from envFile.
 * envFile written via a browser request sent to localhost/ddgem
 * (dd/site/index.html). /sa void runCGI().
 */
void printPost();

/**
 * @brief process saved data in cgi environment for running in command line mode
 */
void doProcess();

/**
 * @brief prints the data returned by parseDDT, a vector of ddtseq arrays. the
 * ddtse arrays contain the amount, delay, and choices for the small (x2) and
 * large (x2) stage 1 sequences
 *
 * @sa std::vector<ddtseq> parseDDT(tinyxml2::XMLDocument const &doc,
 * std::function<int(tinyxml2::XMLDocument const &doc, int const sz, int const
 * szID, double const d, int const am)> ch);
 *
 * @sa int choice(tinyxml2::XMLDocument const &doc, int const sz, int const
 * szID, double const d, int const am);
 *
 */
void printData(std::vector<ddtseq> const &data);

/**
 * @brief generate a test xml packet, command line mode option 3, choose
 * parameters
 *
 */
void generateTestXML();

/**
 * @brief generate and print the xml tags used for extracting data from xml.
 * command line mode option 4
 */
void generatePrintXMLTags();

/**
 * @brief runs if no options given or option=0. From the command line this will
 * fail because a cgi environment is required. If called by Apache there will be
 * a CGI environment. Visiting localhost/ddgem (dd/site/index.html) will cause
 * Apache to start ddgem with the xmlpacket in dd/site/ddPack.xml posted, thus
 * there will be a cgi environment present for runCGI get. When starting in this
 * way there is an option to save the cgi environment to envFile so that
 * functions requiring the same cgi environment as runCGI, including the
 * xmlpacket, can run e.g. /sa void printPost().
 *
 * If the xmlpacket in cgi environment contains element <saveEnv save="true"/>
 * then the environment will be saved in envFile when runCGI executes.
 *
 */
void runCGI();

/**
 * @brief process an xmlPacket. This is the core
 * launch for all data processing and analysis. Called from doProcess() and
 * runCGI(). Creates spgxyz::mlk object, extracts least squares and maximum
 * likelihood ks for exponential and hyperbolic models, uses ks to compute and
 * return the stage 2 parameters.
 *
 * During mlk creation the least squares parameters are created during
 * initialisation of the mlk base class lsk. To create maximum likelihood
 * parameters mlk constructor creates a ddgemModelNLopt object. This object
 * takes a pointer to the mlk object for it's calculations of ml using NLOpt.
 */
std::string process(tinyxml2::XMLDocument const &doc);

/**
 * @brief find choice, my structured xml. will need another with same type to
 * process hedwig format \sa int choice(tinyxml2::XMLDocument const &doc, int
 const sz, int const szID, double const d, int const am);
 *
 * @param int const sz    --0,1 -> small,large
 * @param int const szID  --0,1 -> first,second series
 * @param double const d  --delay
 * @param int const am    --amount
 *
 * @return choice         -- -1,0,1  --> error not found, small choice, large
 * choice
 */
int oldChoice(tinyxml2::XMLDocument const &doc, int const sz, int const szID,
              double const d, int const am);

/**
 * @brief choice for Hedwig's "flat xml", we are using this for the experiment
 *
 * @sa int oldChoice(tinyxml2::XMLDocument const &doc, int const sz, int const
 * szID, double const d, int const am);
 *
 * @return int - 0 small choice, 1 large choice
 */
int choice(tinyxml2::XMLDocument const &doc, int const sz, int const szID,
           double const d, int const am);

/**
 * @brief helper for choice. get xml tag e.g. S101_01 or S101_26 based on inputs
 * so response can be extracted from hedwig's flat xml xml
 *
 * see perl upscripts/indx.pl for quick code to reproduce pattern in
 * site/xmlTagValueMap.txt and see TestXMLGenerator -- ddgem test option 4
 *
 * @param int const sz      --0,1 -> small,large
 * @param int const szID    --0,1 -> first,second series
 * @param double const d    --delay
 * @param int const am      --amount
 */
std::string getXMLTag(int const sz, int const szID, double const d,
                      int const am);
/**
 * @brief debug helper, print some useful xml information. pass XMLElement ptr.
 * Do not use neighbours without thinking about it. Used internally to limit the
 * depth, so when called with just and XMLElement ptr it is called recursively
 * to just print the current element, next sib, previous sib, and child only.
 *
 * @param tinyxml2::XMLElement const *ep
 * @param int neighbours = 1
 */
void printXMLElementInfo(tinyxml2::XMLElement const *ep, int neighbours = 1);

/**
 * @brief compare a double to char * representation of a double. Needs only to
 * match with tol, otherwise problems with small decimal errors
 *
 * @sa bool compare(double const d1, double const d2, double const tol = 0.0001)
 * @param char const *val
 * @param double const d
 * @param double const tol = 0.0001
 */
bool compare(char const *val, double const d, double const tol = 0.0001);

/**
 * @brief compare double to double with tol
 * @sa bool compare(char const *val, double const d, double const tol = 0.0001);
 */
bool compare(double const d1, double const d2, double const tol = 0.0001);

/**
 * @brief save the faulty xml and throw
 *
 */
void xmlParseError(std::string &xml);

/**
 * @brief read from f required credentials
 *
 */
std::pair<std::string, std::string> getCredentials(std::string const &f);

/**
 * @brief gets the stage 1 data from xml. Uses choice(tinyxml2::XMLDocument
 const &doc, int const sz, int const szID, double const d, int const am). This
 is a core function.
 *
 * @param tinyxml2::XMLDocument const &doc - xml to process
 * @param std::function<int (tinyxml2::XMLDocument const &doc, int const sz, int
 * const szID, double const d, int const am)> ch - function to do the processing
 *
 * @return std::vector<ddtseq> - vector of ddtseq. ddtseq is array of ddt.
 *
 * ddt struct {amount, delay, choice}. vector[0]=small1, [1]=small2, [3]=large1,
 * [4]=large2. Each vector length = nd*na
 *
 * within array e.g. small[0] the ddts are ordered delay[0], amount[0];
 * delay[0],amount[1]; delay[0],amount[2]; .. delay[nd-1],amount[na-1]
 */
std::vector<ddtseq>
parseDDT(tinyxml2::XMLDocument const &doc,
         std::function<int(tinyxml2::XMLDocument const &doc, int const sz,
                           int const szID, double const d, int const am)>
             ch);
/**
 * @brief some test/development calculations for abcrit
 */
void abcritCalcs();

/**
 * @brief get values needed for stage 2
 *
 * The D values for each question are given below, d is fixed.
 * ll1 - S in d or L in (d+LD)/2
 * ll2 - S now or L  in (LD-d)/2
 * ls1 - S in d or L in d+(UD-LD)/2
 * ls2 - S now or L  in (UD-LD)/2
 * ss1 - S in d or L in 2UD-LD
 * ss2 - S now or L  in 2UD -LD -d
 *
 * @return std::vector<double> indexed by scoped enumeration s2idx
 */
std::vector<double> getStage2Values(double const k, double const K,
                                    double const a, double const A,
                                    lfptr const l, ufptr const ub,
                                    ddfptr const d);

enum class dbColIdx {
  id,
  pid,
  dts,
  critJB,
  critExpK,
  kh_S,
  ke_S,
  kh_L,
  ke_L,
  kh_Sssd,
  ke_Sssd,
  kh_Lssd,
  ke_Lssd,
  khm_S,
  kem_S,
  khm_L,
  kem_L,
  kh_SNLL,
  ke_SNLL,
  kh_LNLL,
  ke_LNLL,
  ghs,
  ges,
  ghl,
  gel
};

enum class dbColIdxXML { id, pid, dts, xml, idps, s2data };

/**
 * @brief data to and from mysql database
 *
 */
class mysqlData {
  int id;
  kAndFit kss;
  kAndFit ksl;
  kAndFit kssml;
  kAndFit kslml;
  std::pair<double, double> gs;
  std::pair<double, double> gl;
  unsigned char const *XMLPtr;
  std::size_t XMLSz;
  int critJB;
  int critExpK;
  std::vector<idpValues> idpsS;
  std::vector<idpValues> idpsL;

  std::vector<double> s2h;
  std::vector<double> s2e;

  std::vector<double> idpBytes;
  unsigned char const *idpPtr;

  std::vector<double> s2Bytes;
  unsigned char const *s2Ptr;

  /**
   * @brief get indifference points small s1, small s2, small avg, large s1,
   * large s2, large avg and set them into idpBytes so can be returned as
   * mysqlx::bytes \sa void setS2Bytes
   */
  void setIDPBytes();

  /**
   * @brief s2 values into s2Bytes to return as mysqlx::bytes, hyperbolic
   * first,exponential second, doubles indexed by s2idx \sa void setIDPBytes
   */
  void setS2Bytes(bool h);

public:
  mysqlData(int pid);
  mysqlData(mysqlData &&lhs) = default; // move construction

  void setkS(kAndFit &&small, kAndFit &&large);
  void setKSML(kAndFit &&small, kAndFit &&large);
  void setGS(std::pair<double, double> &&small,
             std::pair<double, double> &&large);
  void setXML(char const *xp, int sz);
  void setCrit(int jb, int ek);
  void setIDPs(std::vector<idpValues> &&small, std::vector<idpValues> &&large);
  void setIDPs(mysqlx::bytes const &idpb);

  void setS2Data(std::vector<double> &&d,
                 bool h = true); //!< call twice once for hyperbolic, once for
  //!< exponential (set h=false)

  void setS2Data(mysqlx::bytes const &s2dat);

  int getID() const { return id; }
  kAndFit const &getKSS() const { return kss; }
  kAndFit const &getKSL() const { return ksl; }
  kAndFit const &getKSSML() const { return kssml; }
  kAndFit const &getKSLML() const { return kslml; }
  std::pair<double, double> const &getGS() const { return gs; }
  std::pair<double, double> const &getGL() const { return gl; }
  idpValues const &getIDPs(bool s) const {
    return s ? idpsS[2] : idpsL[2];
  } //!< return average idp for small or large
  mysqlx::bytes getXML() const { return mysqlx::bytes{XMLPtr, XMLSz}; }
  std::pair<int, int> getIncCriteria() const {
    return std::make_pair(critJB, critExpK);
  }

  /**
   * @brief get indifference points small s1, small s2, small avg, large s1,
   * large s2, large avg as mysqlx::bytes
   */
  mysqlx::bytes getIDPS() const {
    return mysqlx::bytes{idpPtr,
                         static_cast<size_t>(idpBytes.size() * sizeof(double))};
  }

  /**
   * @brief get s2 data as mysqlx::bytes
   */
  mysqlx::bytes getS2Data() const {
    return mysqlx::bytes{s2Ptr,
                         static_cast<size_t>(s2Bytes.size() * sizeof(double))};
  }

  /**
   * @brief access the stage2 data vectors
   */
  std::vector<double> const &getS2Data(bool h) const { return h ? s2h : s2e; }

  /**
   * @brief Get the Data String object for reading into R etc. stage 1 data
   *
   * @return std::string
   */
  std::string getDataString();

  /**
   * @brief Get the Data String object for reading into R etc. stage 2 data
   *
   * @return std::string
   */
  std::string getDataStringS2();

  /**
   * @brief prints data to screen
   *
   */
  void printData();
};

/**
 * @brief writes data to mysql database
 *
 * @param std::pair<std::string, std::string> const &credentials
 * @param std::string const &dbname
 * @param mysqlData const& data
 */
bool storeData(std::pair<std::string, std::string> const &credentials,
               std::string const &dbname, mysqlData const &data);

/**
 * @brief reads data from mysql database
 *
 * @param credentials
 * @param dbname
 * @param ids
 * @return mysqlData
 */
std::vector<mysqlData>
readData(std::pair<std::string, std::string> const &credentials,
         std::string const &dbname, std::vector<int> const &ids);

/**
 * @brief reads the lines in f
 *
 * @param f
 * @return std::vector<std::string>
 */
std::vector<std::string> readFile(const std::string &f);
/**
 * @brief stage 2 data back to SSS server
 */
void sendResponse(mysqlData const &my);

/**
 * @brief print, for checking, the choice pattern produced by the s2 delays
 *
 * @param std::vector<double> const & s2data -- stage 2 data for checking
 * @param bool h -- hyperbolic or exponential model
 */
void checkS2Choices(std::vector<double> const &s2data, double const k,
                    double const K, double const a, double const A,
                    ddfptr const d);

/**
 * @brief read binary files from db -- either xml, indifference points, or
 * s2data
 *
 * @param std::string f - input file
 * @param spgxyz::binFileType type - type of binary DB to read
 */
void readDBBinary(std::string f, spgxyz::binFileType type);

/**
 * @brief get string values for s2 indices
 */
std::string s2idxToString(spgxyz::s2idx enumValue);

/**
 * @brief increment s2idx enum, wrap after reaching end, spgxyz::s2idx::ss2
 *
 * see stroustrup p27
 */
spgxyz::s2idx &operator++(spgxyz::s2idx &idx);

} // namespace spgxyz