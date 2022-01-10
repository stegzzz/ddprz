#include <algorithm>
#include <boost/program_options.hpp>
#include <ddFuncs.h>
#include <functional>
#include <iostream>
#include <memory>
#include <mutils.h>
#include <nlopt.hpp>
#include <tuple>
#include <utils.h>
#include <vector>

namespace po = boost::program_options;

/** @file ddParamsRec.h
 * @brief for dd project, generate some responses for hyperbolic or exponential
 * models with specified parameters. Then model those responses. Can the
 * modelling recover the parameters used to generate the data?
 *
 * More to the point above, the interest here is whether or not the delays used
 * to generate the data for modelling will influence the recovered parameters.
 *
 * See epConclusion.r for example usage
 *
 */

void listOptions();
void help(boost::program_options::options_description const &desc);
void hint();

namespace spgxyz {

/**
 * @brief parameters for data generation and parameter recovery, set to default
 * values that can be changed on command line or restored to default file
 * ddpr.par
 *
 */
struct OptParams {
  static constexpr char defaultParameterFile[] = "ddpr.par";
  static constexpr int nVars = 23; //!< number of fields in this structure
  // optimiser
  int nOptimisations = 10;
  int nIter = 200;
  std::string algorithm{"NELDERMEAD"};
  // model
  double k = 0.03;
  double K = 0.015;
  double g = 0.1;
  // data generation
  int n = 1000;
  double a = 820;
  double A = 1000;
  int as = 5;
  int ds = 5;
  std::vector<double> delays1{24, 12, 6, 3, 1, 0.5, 0.25};
  std::vector<double> delays2{2400, 1200, 600, 300, 100, 50, 25};
  bool frontEnd1 = false;
  bool frontEnd2 = false;
  // in/output
  std::string parameterFile{defaultParameterFile};
  std::string generatedDataFileStem{"data"};
  std::string resultFile{"ddpr.txt"};
  bool saveResponses = true;
  bool saveResults = true;
  bool appendResults = false;
  // misc
  bool printDetails = false;
  bool outputOff = false;

  void write();
  void read();
};
/**
 * @brief Set the parameters
 *
 * @param inits values from defaults with any overrides from command line
 * @return OptParams
 */
OptParams setupParameters(OptParams const &inits, po::variables_map vm);

/**
 * @brief model parameters k, K, g
 *
 */
using mparams = std::tuple<double, double, double>;

enum class mpIdx { k, K, g }; //!< indices for mparams

/**
 * @brief data generation parameters n, a, A, as, and ds
 *
 * n -- number of responses to be generated<br>
 * a, A -- small, large amount<br>
 * as, ds -- amount, delay steps<br>
 *
 * a and A are the upper bound on a series of small amounts, large amounts. The
 * number of values generated below or equalt to the upper bound is defined by
 * as, see DDDataGenerator::generateResponses(dparams params, Delays &Ds, bool
 * frontEnd = false) for detaisl. ds performs the same function for the Ds.
 * getValueBelow chooses for each trial the value below or equal to the upper
 * bound to be used in that trial.
 */
using dparams = std::tuple<int, double, double, int, int>;

enum class dpIdx { n, a, A, as, ds }; //!< indices for dparams

/**
 * @brief data point generated by model rsp, a, A, d, D
 *
 * rsp -- 0 model picked S (a at d) over L (A at D)
 *     -- 1 model picked L over S
 * a, A -- small, large amount
 * d, D -- delay to a, delay to D
 */
using data = std::tuple<int, double, double, double, double>;

enum class dataIdx { rsp, a, A, d, D }; //!< indices for data

using Delays = std::vector<double> const;
using vfnc = std::function<double(double, double, double)>;
using optResult = std::tuple<double, double, double, double>;
enum class resultIdx { k, K, g, ml }; //!< indices for optResult
/**
 * @brief optimiser parameters
 *
 * nopt -- number of optimisationa
 * maxiter -- maximum number of iterations
 * algo -- algorithm one of NELDERMEAD, SUBPLEX, BOBYQA, and COBYLA
 * miscprint -- print misc during run
 *
 */
using optimParams = std::tuple<int, int, std::string, bool>;
enum class optimIdx { nopt, maxiter, algo, miscprint }; //!<

mparams getMP(OptParams const &p);
dparams getDGP(OptParams const &p);
optimParams getOP(OptParams const &p);

/**
 * @brief generate some delay discounting data according to model and data
 * generation parameters
 *
 * DDDataGenerator::generateResponses(dparams params, Delays &Ds,bool
 * frontEnd = false) is the main function used directly after construction to
 * produce simulated responses according to spgxyz::mparams and spgxyz::dparams
 *
 */
class DDDataGenerator {

  mparams modelParams;
  dparams dataParams;
  std::vector<data> samples;
  vfnc val;
  MyMTRNG myrng;
  std::mt19937 rng;
  std::bernoulli_distribution coin;       //!< 50/50 true/false
  std::bernoulli_distribution biasedCoin; //!< going to vary p(true)
  std::uniform_int_distribution<> dice;   //!< integer values

  std::string fileOutput;
  bool printDetails;
  bool outputOn = true;

  /**
   * @brief get a value below max, the number of values below the max is defined
   * by nstep
   *
   * one of nstep values is returned at random. If incZero==true the values are
   * chosen from the range max-i*szstep with i in [1, 2, ..nstep]. If
   * incZero==false the range is max-i*szstep + max/nstep with i in [1, 2,
   * ..nstep]. szstep=max/nstep in both cases.<br><br>
   * For example, with incZero==true, max=50 and nstep=5 gives
   * possible values 0, 10, 20, 30, and 40 formed by 50-1*szstep, 50-2*szstep,
   * .. 50-5*szstep and szstep=50/5. With incZero==false we have 10, 20, 30,
   * 40, and 50 as possible values.
   *
   * uses dice
   *
   * @param max
   * @param nstep
   * @return double
   */
  double getValueBelow(double max, int nstep, bool incZero = true);

  /**
   * @brief d defaults to now
   *
   * @param S
   * @param L
   * @param D
   * @param d
   * @return int
   */
  int response(double S, double L, double D, double d = 0);

public:
  /**
   * @brief Construct a new DDDataGenerator object
   *
   * @param params model parameters
   * @param f value function (h or e)
   * @param o output file
   * @param p print some extra details to screen during generation
   * @param oOff output off
   */
  DDDataGenerator(mparams params, vfnc f, std::string o, bool p = false,
                  bool oOff = false);

  /**
   * @brief generate the responses using params and Ds
   *
   * n "trials" are generated. Each trial is randomly determined to use one of a
   * or A as the larger later outcome. The default values of a and A are 820 and
   * 1000 respectively to mirror the values used in ddgem in search of the PRZ
   * project. After setting the value of LL (to a or A) a value for SS is
   * randomly chosen to be some value \(0 < SS \leq LL\) from among \(as\)
   * evenly spaced values. The delay to the larger reward D is chosen at random
   * from Ds. By default the delay to the smaller reward d is fixed at zero but
   * can include front end delay.
   *
   * If a front end delay is included then LL is always set to A and SS is
   * always set to a. D is chosen randomly from Ds and d is then randomly chosen
   * to be some value \(0 \leq d < D\) from among \(ds\) evenly spaced values.
   *
   * @param params data generation parameters
   * @param Ds delays
   * @param frontEnd false for d=0, true to allow front end delays
   */
  void generateResponses(dparams params, Delays &Ds, bool frontEnd = false);

  /**
   * @brief write the data to file
   *
   */
  void storeData();

  /**
   * @brief access the generated data NB by reference
   *
   * @return std::vector<data> const&
   */
  std::vector<data> const &getSamples() { return samples; };
};

/**
 * @brief finds maximum likelihood parameters for vector of data points for
 * model with objective function type vfnc
 *
 * @todo needs constructor overload to read data points from file
 *
 */
class ddParamRec {
  std::vector<double> x; /*!< nlopt x values */
  std::vector<double> lb{0.000001, 0.000001,
                         0.000001};            /*!< nlopt lower bounds */
  std::vector<double> ub{5, 5, 5};             /*!< nlopt uppper bounds */
  double rel_tol = 1e-6;                       //!< nlopt relative tolerance
  std::vector<double> inits{0.04, 0.02, 0.03}; //!< initial values
  optResult result; //!< result of ml minimisation k, K, g, ml
  bool minSuccess = false;

  double valueML(double k, double K, double g);
  std::vector<data> const &samples;
  vfnc val;
  MyMTRNG myrng;
  std::mt19937 rng;

  // default params, changeable via setOptimParams
  int nopt = 10;
  int maxiter = 200;
  std::string algo{
      "NELDERMEAD"}; //!< passed as parameter, needs convert to
                     //!< unsigned of nlopt::algorithm enum in optAlgo
  bool miscprint = false;
  nlopt::algorithm optAlgo = nlopt::algorithm::LN_NELDERMEAD;
  bool outputOn;

  /**
   * @brief used during repeated optimisations to vary the initial values
   *
   * @param pc
   */
  void perturbInits(double pc);

public:
  ddParamRec(std::vector<data> const &d, vfnc f, bool oOff = false)
      : samples(d), val(f), rng(myrng.getRNG()), outputOn(!oOff) {}

  /**
   * @brief number of times to repeat optimisation selecting best ml result
   *
   * @param n
   * @return true
   * @return false
   */
  bool minimise(int n = 1);

  /**
   * @brief print out initial values
   *
   */
  void printInits() {
    for (auto &d : inits)
      std::cout << d << std::endl;
  }

  optResult getResult() { return result; }

  /**
   * @brief change from defaults
   *
   * @param p
   */
  void setOptimParams(optimParams p);

  void printNLOPTVersion();

  // NLOPT
  /**
   * @brief calls valueML
   *
   * @param x
   * @return double
   */
  double operator()(std::vector<double> const &x); /*!< for nlopt */

  /**
   * @brief this function is passed to nlopt::opt::set_min_objective along with
   * a pointer to this
   *
   * wrap calls this, which is a function object, by reinterpret cast to
   * ddParamRec on data parameter passing x
   * @param x
   * @param grad
   * @param data
   * @return double
   */
  static double wrap(std::vector<double> const &x, std::vector<double> &grad,
                     void *data); /*!< for nlopt */
};

/**
 * @brief run main data generation and parameter recovery
 *
 * for each model and for each of the two sequences of Ds specified in runParams
 * generate some data and then try to recover the parameters. If save true
 * parameters (as read from file and modified by any command line values),
 * responses, and results will be saved. Saved results have optResult format k,
 * K, g, and ml and will be written on one row of 16 columns -- result for H1,
 * E1, H2 and E2. If appResult set the results from multiple runs appended --
 * for statistical analysis of multiple runs.
 *
 * @param OptParams const &runParams  -- parameterisation
 * @param int runN=1  -- used for multiple runs
 * @param int nrun    -- number of runs to be run
 *
 */
void run(OptParams const &runParams, int runN, int nrun);

} // namespace spgxyz