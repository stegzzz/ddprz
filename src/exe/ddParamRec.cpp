#include "ddParamRec.h"

const int nopts = 1;
/* clang-format off */
std::array<std::string, nopts> options = {
    "Option 1 (-o 1 | --opt 1), run data generation and parameter recovery"
    };
/* clang-format on */
int option = -1;
bool ver = false;
int nrun = 1; //!< number of times to run data generation and parameter
              //!< recovery, use with appResult

#ifdef VER
std::string version{VER};
#else
std::string version{"version not set"};
#endif

std::string helpStr{
    "delay discounting, data generation and parameter recovery exploration"};

void listOptions() {
  std::cout << "__\n" << __func__ << std::endl;
  std::cout << "Available options ddParamRec:-" << std::endl;
  for (auto s : options)
    std::cout << "\t" << s << std::endl;
}

void help(po::options_description const &desc) {
  std::cout << helpStr << std::endl;
  std::cout << desc << std::endl;
  listOptions();
}

void hint() {
  std::cout << helpStr << "\n"
            << "use -h for more information" << std::endl;
  listOptions();
}

namespace spgxyz {
void OptParams::write() {
  std::ofstream ouf(parameterFile);
  if (!ouf)
    throw std::runtime_error{
        std::string{"Could not open file " + parameterFile + " "}.append(
            __func__)};
  ouf << "#parameter file for ddParamRec.exe NB format param\\t\\t# "
         "description for each parameter\n";
  ouf << "#optimiser\n"
      << nOptimisations << "\t\t# int number of optimisations\n";
  ouf << nIter << "\t\t# int max n interations\n";
  ouf << algorithm
      << "\t\t# string algorithm one of NELDERMEAD, SUBPLEX, BOBYQA, and "
         "COBYLA\n";
  ouf << "#model\n" << k << "\t\t# double discount rate k for a\n";
  ouf << K << "\t\t# double discount rate K for A\n";
  ouf << g << "\t\t# double sensitivity parameter g\n";
  ouf << "#data generation\n"
      << n << "\t\t# int number of data points to generate\n";
  ouf << a << "\t\t# double a (smaller reward)\n";
  ouf << A << "\t\t# double A (larger reward)\n";
  ouf << as << "\t\t# double as, number of steps below for amounts\n";
  ouf << ds << "\t\t# double ds, number of steps below for delays\n";
  for (auto &d : delays1)
    ouf << d << "\t";
  ouf << "\t# doubles, first set of Ds\n";
  for (auto &d : delays2)
    ouf << d << "\t";
  ouf << "\t# doubles, second set of Ds\n";
  ouf << frontEnd1
      << "\t\t# bool (0/1) for front end delays on S with delays1\n";
  ouf << frontEnd2
      << "\t\t# bool (0/1) for front end delays on S with delays2\n";
  ouf << "#input/output\n"
      << parameterFile << "\t\t# string parameter file to read/write\n";
  ouf << generatedDataFileStem
      << "\t\t# string file stem to write generated responses\n";
  ouf << resultFile << "\t\t# string file to write recovery results\n";
  ouf << saveResponses << "\t\t# bool (0/1) to save generated responses\n";
  ouf << saveResults << "\t\t# bool (0/1) to save recovery results\n";
  ouf << appendResults << "\t\t# bool (0/1) to append recovery results\n";
  ouf << "#misc\n"
      << printDetails << "\t\t# bool (0/1) print some details during run\n";
  ouf << outputOff << "\t\t# bool (0/1) switch off all output during run";

  ouf.close();
}

void OptParams::read() {
  auto lines = filterStrings(readFile(parameterFile), "\t\t#");
  auto nread = lines.size();
  if (nread != nVars)
    throw std::runtime_error{
        std::string{"Number of params read " + std::to_string(nread) +
                    " expecting " + std::to_string(nVars) + " in " + __func__}};
  nOptimisations = std::stoi(spgxyz::splitStr(lines[0], "\\s")[0]);
  nIter = std::stoi(spgxyz::splitStr(lines[1], "\\s")[0]);
  algorithm = spgxyz::splitStr(lines[2], "\\s")[0];
  k = std::stod(spgxyz::splitStr(lines[3], "\\s")[0]);
  K = std::stod(spgxyz::splitStr(lines[4], "\\s")[0]);
  g = std::stod(spgxyz::splitStr(lines[5], "\\s")[0]);
  n = std::stoi(spgxyz::splitStr(lines[6], "\\s")[0]);
  a = std::stoi(spgxyz::splitStr(lines[7], "\\s")[0]);
  A = std::stoi(spgxyz::splitStr(lines[8], "\\s")[0]);
  as = std::stoi(spgxyz::splitStr(lines[9], "\\s")[0]);
  ds = std::stoi(spgxyz::splitStr(lines[10], "\\s")[0]);
  delays1.clear();
  auto buffD = spgxyz::splitStr(spgxyz::splitStr(lines[11], "\t\t#")[0], "\\s");
  for (auto &d : buffD)
    delays1.push_back(std::stod(d));
  delays2.clear();
  buffD = spgxyz::splitStr(spgxyz::splitStr(lines[12], "\t\t#")[0], "\\s");
  for (auto &d : buffD)
    delays2.push_back(std::stod(d));
  int buff = std::stoi(spgxyz::splitStr(lines[13], "\\s")[0]);
  frontEnd1 = buff == 0 ? false
              : buff == 1
                  ? true
                  : throw std::range_error{std::string{
                        "Got " + std::to_string(buff) +
                        " frontEnd1 expected 0 or 1 bool in " + __func__}};
  buff = std::stoi(spgxyz::splitStr(lines[14], "\\s")[0]);
  frontEnd2 = buff == 0 ? false
              : buff == 1
                  ? true
                  : throw std::range_error{std::string{
                        "Got " + std::to_string(buff) +
                        " frontEnd2 expected 0 or 1 bool in " + __func__}};

  parameterFile = spgxyz::splitStr(lines[15], "\\s")[0];
  generatedDataFileStem = spgxyz::splitStr(lines[16], "\\s")[0];
  resultFile = spgxyz::splitStr(lines[17], "\\s")[0];
  buff = std::stoi(spgxyz::splitStr(lines[18], "\\s")[0]);
  saveResponses =
      buff == 0   ? false
      : buff == 1 ? true
                  : throw std::range_error{std::string{
                        "Got " + std::to_string(buff) +
                        " saveResponses expected 0 or 1 bool in " + __func__}};
  buff = std::stoi(spgxyz::splitStr(lines[19], "\\s")[0]);
  saveResults = buff == 0 ? false
                : buff == 1
                    ? true
                    : throw std::range_error{std::string{
                          "Got " + std::to_string(buff) +
                          " saveResults expected 0 or 1 bool in " + __func__}};
  buff = std::stoi(spgxyz::splitStr(lines[20], "\\s")[0]);
  appendResults =
      buff == 0   ? false
      : buff == 1 ? true
                  : throw std::range_error{std::string{
                        "Got " + std::to_string(buff) +
                        " appendResults expected 0 or 1 bool in " + __func__}};
  buff = std::stoi(spgxyz::splitStr(lines[21], "\\s")[0]);
  printDetails =
      buff == 0   ? false
      : buff == 1 ? true
                  : throw std::range_error{std::string{
                        "Got " + std::to_string(buff) +
                        " printDetails expected 0 or 1 bool in " + __func__}};
  buff = std::stoi(spgxyz::splitStr(lines[22], "\\s")[0]);
  outputOff = buff == 0 ? false
              : buff == 1
                  ? true
                  : throw std::range_error{std::string{
                        "Got " + std::to_string(buff) +
                        " outputOff expected 0 or 1 bool in " + __func__}};
}

OptParams setupParameters(OptParams const &inits, po::variables_map vm) {
  OptParams result{};
  result.parameterFile = inits.parameterFile;
  result.read(); // we now have parameters from the default file or from a file
                 // entered on command line
  auto it = vm.begin();
  auto ite = vm.end();
  while (it != ite) { // now we iterate over vm and for each parameter item set
                      // on command line set in result via inits
    if ((*it).first == "nopt")
      result.nOptimisations = inits.nOptimisations;
    if ((*it).first == "niter")
      result.nIter = inits.nIter;
    if ((*it).first == "optAlg")
      result.algorithm = inits.algorithm;
    if ((*it).first == "kSmall")
      result.k = inits.k;
    if ((*it).first == "kLarge")
      result.K = inits.K;
    if ((*it).first == "gSensitivity")
      result.g = inits.g;
    if ((*it).first == "nData")
      result.n = inits.n;
    if ((*it).first == "smallAmount")
      result.a = inits.a;
    if ((*it).first == "largeAmount")
      result.A = inits.A;
    if ((*it).first == "aSteps")
      result.as = inits.as;
    if ((*it).first == "dSteps")
      result.ds = inits.ds;
    if ((*it).first == "D1") {
      result.delays1 = inits.delays1;
      std::sort(result.delays1.begin(), result.delays1.end(),
                std::greater<double>());
    }
    if ((*it).first == "D2") {
      result.delays2 = inits.delays2;
      std::sort(result.delays2.begin(), result.delays2.end(),
                std::greater<double>());
    }
    if ((*it).first == "frontEnd1")
      result.frontEnd1 = inits.frontEnd1;
    if ((*it).first == "frontEnd2")
      result.frontEnd2 = inits.frontEnd2;
    if ((*it).first == "generatedDataFileStem")
      result.generatedDataFileStem = inits.generatedDataFileStem;
    if ((*it).first == "resultFile")
      result.resultFile = inits.resultFile;
    if ((*it).first == "print")
      result.printDetails = inits.printDetails;
    if ((*it).first == "outputOff")
      result.outputOff = inits.outputOff;
    ++it;
  }
  if (vm["appResult"].as<bool>())
    result.appendResults = true;

  if (vm["deleteResultsFirst"].as<bool>()) {
    spgxyz::deleteFile(result.resultFile);
  }

  if (vm["save"].as<bool>()) {
    std::cout << "save true" << std::endl;
    result.saveResponses = true;
    result.saveResults = true;
    result.write();
  }
  return result;
}
DDDataGenerator::DDDataGenerator(mparams params, vfnc f, std::string o, bool p,
                                 bool oOff)
    : modelParams(params), val(f), rng(myrng.getRNG()), fileOutput(o),
      printDetails(p), outputOn(!oOff) {}

double DDDataGenerator::getValueBelow(double max, int nstep, bool incZero) {
  dice.param(std::uniform_int<>::param_type(1, nstep));
  auto idx = dice(rng);
  return incZero ? max - idx * (max / nstep)
                 : max - idx * (max / nstep) + max / nstep;
}

int DDDataGenerator::response(double S, double L, double D, double d) {
  auto [k, K, g] = modelParams;
  auto pss = 0.0;
  auto vl = L;
  auto vs = S;
  if (d == 0) {
    auto lk = L == std::get<to_underlying(dpIdx::a)>(dataParams) ? k : K;
    vl = val(L, lk, D);
  } else {
    vl = val(L, K, D);
    vs = val(S, k, d);
  }
  pss = 1 / (1 + std::exp(g * (vl - vs)));
  if (printDetails)
    std::cout << __func__ << "\tpss= " << pss << "\tD,L,vl,K= " << D << "," << L
              << "," << vl << "," << K << "\td,S,vs,k= " << d << "," << S << ","
              << vs << "," << k << "\tg=" << g << "\n";
  biasedCoin.param(std::bernoulli_distribution::param_type(pss));
  return biasedCoin(rng) ? 0 : 1;
}

void DDDataGenerator::generateResponses(dparams params, Delays &Ds,
                                        bool frontEnd) {
  dataParams = params;
  auto n = std::get<to_underlying(dpIdx::n)>(dataParams);
  samples.reserve(n);
  samples.clear();

  for (int i = 0; i < n; ++i) {
    dice.param(std::uniform_int<>::param_type(0, Ds.size() - 1));
    auto idx = dice(rng);
    auto D = Ds[idx]; //!< pick D
    auto d = 0.0;
    auto L = 0.0;
    auto S = 0.0;
    auto rsp = 0;
    if (frontEnd) {
      L = std::get<to_underlying(dpIdx::A)>(dataParams); //!< LL=A
      S = std::get<to_underlying(dpIdx::a)>(dataParams); //!< SS=a
      d = getValueBelow(
          D, std::get<to_underlying(dpIdx::ds)>(
                 dataParams)); //!< pick d NB can include 0 but d less than D
      auto dZero = approxEqual(0, d, 0.0001);
      if (dZero)
        S = getValueBelow(
            L, std::get<to_underlying(dpIdx::as)>(dataParams),
            false); //!< pick the small amount NB we don't want zero and S=L OK
      rsp = dZero ? response(S, L, D) : response(S, L, D, d);
    } else {
      L = coin(rng) ? std::get<to_underlying(dpIdx::a)>(dataParams)
                    : std::get<to_underlying(dpIdx::A)>(
                          dataParams); //!< pick the large amount 50/50 L=a, L=A
      S = getValueBelow(
          L, std::get<to_underlying(dpIdx::as)>(dataParams),
          false); //!< pick the small amount NB we don't want zero and S=L OK
      rsp = response(S, L, D);
    }
    if (printDetails) {
      auto rcode = rsp == 0 ? "S" : rsp == 1 ? "L" : "error";
      std::cout << "Choice " << rcode << "\tL= " << L << " in " << D
                << " or\t S= " << S << " in " << d << std::endl;
    }
    samples.push_back(std::make_tuple(rsp, S, L, d, D));
  } // for (int i = 0; i < n; ++i) {
}

void DDDataGenerator::storeData() {
  std::ofstream ouf(fileOutput);
  if (!ouf)
    throw std::runtime_error{
        std::string{"Could not open file " + fileOutput + " "}.append(
            __func__)};
  std::for_each(samples.begin(), samples.end(), [&ouf](data &dt) {
    auto [r, S, L, d, D] = dt;
    ouf << r << "\t" << S << "\t" << L << "\t" << d << "\t" << D << "\n";
  });
  ouf.close();
}

// NLOPT
double ddParamRec::operator()(std::vector<double> const &x) {
  return (valueML(x[0], x[1], x[2]));
}

double ddParamRec::wrap(std::vector<double> const &x, std::vector<double> &grad,
                        void *data) {
  assert(grad.empty() &&
         "error in ddParamRec::wrap. operator called with !grad.empty()");
  return (*reinterpret_cast<ddParamRec *>(data))(x);
}

double ddParamRec::valueML(double k, double K, double g) {
  double result = 0;
  for (auto s : samples) {
    auto VA = val(std::get<to_underlying(dataIdx::A)>(s), K,
                  std::get<to_underlying(dataIdx::D)>(s));
    auto VS = val(std::get<to_underlying(dataIdx::a)>(s), k,
                  std::get<to_underlying(dataIdx::d)>(s));
    auto pss = 1 / (1 + exp(g * (VA - VS)));
    auto rsp = std::get<to_underlying(dataIdx::rsp)>(s);
    result += rsp == 0   ? -log(pss)
              : rsp == 1 ? -log(1 - pss)
                         : throw std::runtime_error{std::string{
                               "Invalid response: " + std::to_string(rsp) +
                               " in " + __func__}};
  }
  return result;
}

void ddParamRec::perturbInits(double pc) {
  auto dist = std::uniform_real_distribution{};
  for (auto &d : inits) {
    dist.param(
        std::uniform_real_distribution<>::param_type(d - d * pc, d + d * pc));
    d = dist(rng);
  }
}
void ddParamRec::printNLOPTVersion() {
  int major;
  int minor;
  int bugfix;
  nlopt::version(major, minor, bugfix);
  if (outputOn)
    std::cout << "NLOpt version: " << major << ":" << minor << ":" << bugfix
              << std::endl;
};

bool ddParamRec::minimise(int n) {

  nlopt::opt opt(optAlgo, 3);
  // opt.set_xtol_rel(rel_tol);
  opt.set_maxeval(maxiter);
  opt.set_lower_bounds(lb);
  opt.set_upper_bounds(ub);
  opt.set_param("alpha", 1.5);
  opt.set_param("beta", 0.75);
  opt.set_param("gamma", 2.75); //!< see Fan & Zahara 2007

  // std::vector<double> steps{0.01, 0.001};
  // opt.set_initial_step(steps);
  // auto steps = opt.get_initial_step_(x);
  double minf;
  opt.set_min_objective(wrap, this);
  optResult best;
  try {
    for (int i = 0; i < n; ++i) {
      x.clear();
      for (auto d : inits)
        x.push_back(d);
      opt.get_initial_step_(x);
      nlopt::result nlresult = opt.optimize(x, minf);
      if (nlresult > 0) {
        minSuccess = true;
        if (i == 0)
          best = std::make_tuple(x[0], x[1], x[2], minf);
        else if (minf < std::get<to_underlying(resultIdx::ml)>(best)) {
          best = std::make_tuple(x[0], x[1], x[2], minf);
          if (outputOn)
            std::cout << "+";
        }
        // std::cout << "neval: " << opt.get_numevals() << std::endl;
      } else {
        minSuccess = false;
        std::cout << "\n" << nlresult << "\tfail repeat: " << n << std::endl;
      }
      perturbInits(0.2);
      if (outputOn)
        std::cout << ".";
    }
  } catch (std::exception &e) {
    std::cerr << "\nnlopt failed: " << e.what() << std::endl;
    result = std::make_tuple(x[0], x[1], x[2], minf);
    minSuccess = false;
    return false;
  }
  result = best;
  return true;
}
mparams getMP(OptParams const &p) { return std::make_tuple(p.k, p.K, p.g); }

dparams getDGP(OptParams const &p) {
  return std::make_tuple(p.n, p.a, p.A, p.as, p.ds);
}

optimParams getOP(OptParams const &p) {
  return std::make_tuple(p.nOptimisations, p.nIter, p.algorithm,
                         p.printDetails);
}

void ddParamRec::setOptimParams(optimParams p) {
  nopt = std::get<to_underlying(optimIdx::nopt)>(p);
  maxiter = std::get<to_underlying(optimIdx::maxiter)>(p);
  algo = std::get<to_underlying(optimIdx::algo)>(p);
  miscprint = std::get<to_underlying(optimIdx::miscprint)>(p);
  if (algo == "NELDERMEAD")
    optAlgo = nlopt::algorithm::LN_NELDERMEAD;
  if (algo == "SUBPLEX")
    optAlgo = nlopt::algorithm::LN_SBPLX;
  if (algo == "BOBYQA")
    optAlgo = nlopt::algorithm::LN_BOBYQA;
  if (algo == "COBYLA")
    optAlgo = nlopt::algorithm::LN_COBYLA;
}

void run(spgxyz::OptParams const &runParams, int runN, int nrun) {
  auto modelparams = getMP(runParams);
  auto datagenparams = getDGP(runParams);

  std::unique_ptr<std::vector<optResult>> savedResultsPtr = nullptr;

  if (runParams.saveResults)
    savedResultsPtr =
        std::unique_ptr<std::vector<optResult>>{new std::vector<optResult>};

  auto runID = nrun > 1 ? std::to_string(runN) : "";

  std::vector<std::string> files{
      runParams.generatedDataFileStem + "_" + runID + "_H1.txt",
      runParams.generatedDataFileStem + "_" + runID + "_E1.txt",
      runParams.generatedDataFileStem + "_" + runID + "_H2.txt",
      runParams.generatedDataFileStem + "_" + runID + "_E2.txt"};
  std::vector<spgxyz::vfnc> functions{spgxyz::shdd, spgxyz::sedd};
  std::vector<std::vector<double>> delays{runParams.delays1, runParams.delays2};
  int fIdx = 0;
  for (auto &ds : delays)
    for (auto &f : functions) {
      spgxyz::DDDataGenerator dg{modelparams, f, files[fIdx++],
                                 runParams.printDetails, runParams.outputOff};
      if (!runParams.outputOff)
        std::cout << "OK " << files[fIdx - 1] << "\n";

      dg.generateResponses(datagenparams, ds,
                           (fIdx < 3 & runParams.frontEnd1) |
                               (fIdx > 2 & runParams.frontEnd2));
      if (runParams.saveResponses)
        dg.storeData();
      spgxyz::ddParamRec pr{dg.getSamples(), f, runParams.outputOff};
      pr.setOptimParams(getOP(runParams));
      pr.printNLOPTVersion();
      if (pr.minimise(runParams.nOptimisations)) {
        if (savedResultsPtr != nullptr)
          (*savedResultsPtr).push_back(pr.getResult());
        auto [k, K, g, ml] = pr.getResult();
        if (!runParams.outputOff) {
          std::cout << "\nparams recovered: k: " << k << " K: " << K
                    << " g: " << g << " ml: " << ml << std::endl;
          std::cout << "versus k: " << std::get<0>(modelparams)
                    << " K: " << std::get<1>(modelparams)
                    << " g: " << std::get<2>(modelparams) << std::endl;
        }
      } else
        std::cout << "minimisation failed" << std::endl;
    }
  if (savedResultsPtr != nullptr) { // save results
    std::ofstream ouf{runParams.resultFile, runParams.appendResults
                                                ? std::ios_base::app
                                                : std::ios_base::trunc};
    if (!ouf)
      throw std::runtime_error{
          std::string{"Could not open file " + runParams.resultFile + " "}
              .append(__func__)};
    auto sz = (*savedResultsPtr).size();
    for (int i = 0; i < sz; ++i) {
      auto [k, K, g, ml] = (*savedResultsPtr)[i];
      auto sep = (i < (sz - 1)) ? "\t" : "\n";
      ouf << k << "\t" << K << "\t" << g << "\t" << ml << sep;
    }
  }
}

} // namespace spgxyz
int main(int argc, char **argv) {
  std::cout << "hello world again" << std::endl;
  po::variables_map vm;
  po::options_description desc{"Options"};
  std::string no{"Option IDs valid values 1.."};
  no.append(std::to_string(nopts));
  spgxyz::OptParams initialParams;

  auto paramMsg = std::string{"restore default parameter file: "} +
                  initialParams.defaultParameterFile + " and exit";

  try {

    // clang-format off
    desc.add_options()
    ("help,h", "print help and list opt ID descriptions")
    ("version,v",po::bool_switch(&ver), "print version information")
    ("opt,o", po::value<int>(&option), no.c_str())
    ("paramFile,f", po::value<std::string>(&initialParams.parameterFile), "use specified parameter file")
    ("save,s", po::bool_switch(), "save everything and run -- loaded parameters NB as modified by command line, generated responses, and results")
    ("appResult", po::bool_switch(&initialParams.appendResults), "append results to results file")
    ("deleteResultsFirst,d", po::bool_switch(), "delete results file before run, use with append results")
    ("nrun", po::value<int>(&nrun), "n runs of data gen and parameter rec, use with append results")
    ("createFreshDefaultParams", po::bool_switch(), paramMsg.c_str())
    ("nopt", po::value<int>(&initialParams.nOptimisations), "number of optimisations to run")
    ("niter", po::value<int>(&initialParams.nIter), "max iterations for each optimisation")
    ("optAlg", po::value<std::string>(&initialParams.algorithm), "optimisation algorithm -- one of N (NELDERMEAD), S (SUBPLEX), B (BOBYQA), and C (COBYLA)")
    ("kSmall,k", po::value<double>(&initialParams.k), "discount parameter k for small amount")
    ("kLarge,K", po::value<double>(&initialParams.K), "discount parameter K for large amount")
    ("gSensitivity,g", po::value<double>(&initialParams.g), "sensitivity parameter g")
    ("nData,n", po::value<int>(&initialParams.n), "n data point to generate")
    ("smallAmount,a", po::value<double>(&initialParams.a), "small amount")
    ("largeAmount,A", po::value<double>(&initialParams.A), "large amount")
    ("aSteps", po::value<int>(&initialParams.as), "steps for amounts")
    ("dSteps", po::value<int>(&initialParams.ds), "steps for delays")
    ("D1", po::value<std::vector<double>>(&initialParams.delays1), "delay values for first delay vector e.g. --D1 120 --D1 100...")
    ("D2", po::value<std::vector<double>>(&initialParams.delays2), "delay values for second delay vector e.g. --D2 240 --D2 200...")
    ("frontEnd1", po::bool_switch(&initialParams.frontEnd1), "use front end delay in first series")
    ("frontEnd2", po::bool_switch(&initialParams.frontEnd2), "use front end delay in second series")
    ("generatedDataFileStem", po::value<std::string>(&initialParams.generatedDataFileStem), "filename stem for generated data if saving")
    ("resultFile", po::value<std::string>(&initialParams.resultFile), "file for results if saving")
    ("print,p", po::value<bool>(&initialParams.printDetails), "print some details to screen during run")
    ("outputOff", po::value<bool>(&initialParams.outputOff), "print some details to screen during run");

    // clang-format on
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      help(desc);
      return 0;
    }
    if (ver == true) {
      std::cout << "version: " << version << std::endl;
      return true;
    }

    if (vm["createFreshDefaultParams"].as<bool>()) {
      spgxyz::OptParams defaults;
      defaults.write();
      return 0;
    }
  } catch (const po::error &ex) {
    std::cerr << ex.what() << "\n-";
  }

  switch (option) {
  case 1:
    std::cout << options[option - 1] << std::endl;
    try {
      auto currentParams = setupParameters(initialParams, vm);
      for (int i = 0; i < nrun; ++i)
        spgxyz::run(currentParams, i, nrun);
    } catch (std::runtime_error e) {
      std::cerr << "run failed: " << e.what() << std::endl;
    };
    break;
  default:
    hint();
    break;
  }
}

// derived class uses base class getter to return protected member of base
// class