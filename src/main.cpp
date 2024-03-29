// TODO: maybe print all the velocities etc.
//  but maybe just fuck it.

#include <iostream>
#include "simlib.h"
#include <assert.h>
#include "argparser.h"
#include <math.h> // pow, exp, log

constexpr double MU = 1e-6;
double _5mthf = 5.2 * MU;

constexpr double HOUR = 3600;
constexpr double V_mat1_max = (561 * MU) / HOUR;
constexpr double K_mat1_m = 41 * MU;
constexpr double K_mat1_i = 50 * MU;

constexpr double V_mat3_max = (22870 * MU) / HOUR;
constexpr double K_mat3_m2 = 21.1 * MU;

constexpr double V_gnmt_max = (10600 * MU) / HOUR;
constexpr double K_gnmt_m = 4500 * MU;
constexpr double K_gnmt_i = 20 * MU;

constexpr double V_meth_max = (4521 * MU) / HOUR;
constexpr double K_meth_m2__A = 10 * MU;

const double alpha_1 = 100 / HOUR;
const double alpha_2 = 10 / HOUR;

const double beta_1 = 1.7 / HOUR;
const double beta_2 = 30 / HOUR;

constexpr double V_ms_max = (500 * MU) / HOUR;
constexpr double K_ms_m_hcy = 0.1 * MU;
constexpr double K_ms_m5mthf = 25 * MU;
constexpr double K_ms_d = 1 * MU;

constexpr double V_bhmt_max = (2500 * MU) / HOUR; // velocity. µ / h
constexpr double K_bhmt_m = 12 * MU;


class Function3 : public aContiBlock3 {
    Function3(const Function3&) = delete;
    Function3&operator=(const Function3&) = delete;
  double (*f)(double,double,double); // pointer to function
 public:
  Function3(Input i1, Input i2, Input i3, double (*pf)(double,double,double));
  virtual double Value() override;
  //virtual const char *Name() const;
};

Function3::Function3(Input i1, Input i2, Input i3, double (*pf)(double,double,double))

  : aContiBlock3(i1,i2,i3), f(pf) {}

double Function3::Value() {
  /* AlgLoopDetector _(this); */
  this->isEvaluated = true; // haha fuck you simlib
  double ret = f(Input1Value(), Input2Value(), Input3Value());
  return ret;
}

inline double V_ms(const double hcy) {
  // Michaelis-Menten equation implementation
  double numerator = V_ms_max * _5mthf * hcy;

  double denominator1 = K_ms_m_hcy * (K_ms_d + _5mthf);
  double denominator2 = hcy * (K_ms_m5mthf + _5mthf);
  double denominator = denominator1 + denominator2;

  return numerator / denominator;
}

inline double V_meth(const double adoMeth) {
  const double K_meth_m1__adoMet = (1 + adoMeth / 4) / adoMeth; // K_meth_m1 / adoMeth
  // liteally 11 + 11K_meth_m1__adoMet :clown_face:
  const double denominator =
      (K_meth_m2__A + 1) + K_meth_m1__adoMet * (1 + K_meth_m2__A);

  return V_meth_max / denominator;
}

inline double K_mat3_m1(const double adoMet) {
  const double adoMet_exp = adoMet / (adoMet + 600);

  return 20000 / (1 + 5.7 * (adoMet_exp * adoMet_exp));
}

inline double V_mat1(const double met, const double adoMet) {
  const double denominator1 = (K_mat1_m / met) * (1 + adoMet / K_mat1_i);
  const double denominator = 1 + denominator1;

  assert(denominator != 0 && "Denominiator must not be zero");

  return V_mat1_max / denominator;
}

uint c = 0;
inline double V_mat3(const double met, const double adoMet) {
  const double denominator = 1 + (K_mat3_m1(adoMet) * K_mat3_m2) / (met * (met + K_mat3_m2));

  return V_mat3_max / denominator;
}

inline double V_gnmt(const double adoMet, const double adoHey) {
  const double denominator1 = 1 + pow(K_gnmt_m / adoMet, 2.3);
  const double denominator2 = 1 + adoHey / K_gnmt_i;

  return V_gnmt_max / (denominator1 * denominator2);
}

inline double V_ah(const double adoHcy, const double hcy) {
  return alpha_1 * (adoHcy - alpha_2 * hcy);
}

inline double V_cbs(const double adoMet, const double adoHcy, const double hcy) {
  // from Finkelstein and Martin, 1984
  /* return 1.7 * (adoMet + adoHcy) - 30; */
  return (beta_1 * (adoMet + adoHcy) - beta_2) * hcy;
}

inline double V_bhmt(const double adoMet, const double adoHcy, const double hcy) {
  const double exp1 = 0.7 - 0.025 * (adoMet + adoHcy - 150);
  const double exp2 = (V_bhmt_max * hcy) / (K_bhmt_m + hcy);

  return exp1 * exp2;
}

double maxTime = 10;

inline double normalizeTime (double currentTime) {
    return currentTime / maxTime;
}

double getTime (double junk) {
    (void) junk;
    return normalizeTime(T.Value());
}

double getTimeExp(double junk) {
    (void) junk;
    double normalizedTime = normalizeTime(T.Value());

    return (exp(normalizedTime) - 1) / (exp(1) - 1);
}

double getTimeLog(double junk) {
    (void) junk;
    double normalizedTime = normalizeTime(T.Value());

    return log1p(normalizedTime) / log1p(1);
}

struct MetabolicModel {
  Integrator Met, AdoMet, AdoHcy, Hcy;
  Expression Metin;

  Function3 fn_V_cbs, fn_V_bhmt;
  Function2 fn_V_mat1, fn_V_mat3, fn_V_gnmt, fn_V_ah;
  Function1 fn_V_meth, fn_K_mat3_m1, fn_V_ms;
  Function1 fn_getTimeLinear, fn_getTimeLog, fn_getTimeExp;

  MetabolicModel(
    double initialMet,
    double initialAdoMet,
    double initialAdoHcy,
    double initialHcy,
    double metinMax
  ) :
    fn_V_cbs(Input(AdoMet), Input(AdoHcy), Input(Hcy), V_cbs),
    fn_V_bhmt(Input(AdoMet), Input(AdoHcy), Input(Hcy), V_bhmt),
    fn_V_mat1(Input(Met), Input(AdoMet), V_mat1),
    fn_V_mat3(Input(Met), Input(AdoMet), V_mat3),
    fn_V_gnmt(Input(AdoMet), Input(AdoHcy), V_gnmt),
    fn_V_ah(Input(AdoHcy), Input(Hcy), V_ah),
    fn_K_mat3_m1(Input(AdoMet), K_mat3_m1),
    fn_V_meth(Input(AdoMet), V_meth),
    fn_V_ms(Input(Hcy), V_ms),
    fn_getTimeLinear(Input(Hcy), getTime),
    fn_getTimeLog(Input(Hcy), getTimeLog),
    fn_getTimeExp(Input(Hcy), getTimeExp),

//    Metin(metinMax * MU * fn_getTimeLog),
//    Metin(metinMax * MU * fn_getTimeExp),
    Metin(metinMax * MU * 3e-4),

      // TODO if `* MU`, then we got weird graphs. FUCK. Nevermind...
    Met(fn_V_ms + fn_V_bhmt + Metin - fn_V_mat1 - fn_V_mat3, initialMet * MU),
    AdoMet(fn_V_mat1 + fn_V_mat3 - fn_V_meth - fn_V_gnmt, initialAdoMet * MU),
    AdoHcy(fn_V_meth + fn_V_gnmt - fn_V_ah, initialAdoHcy * MU),
    Hcy(fn_V_ah - fn_V_cbs - fn_V_ms - fn_V_bhmt, initialHcy * MU) {}
};

MetabolicModel *model = nullptr;

void Sample() {
  if (model != nullptr) {
    Print("%6.2f %.5g %.5g %.5g %.5g %.5g\n", T.Value(), model->Met.Value(), model->AdoMet.Value(), model->AdoHcy.Value(), model->Hcy.Value(), model->Metin.Value());
  }
}

Sampler S(Sample, 0.1);

int main(int argc, char **argv) {

  SimulationConfiguration configuration;
  try {
    configuration = argparser::parseArguments(argc, argv);
  } catch (std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "Usage: " << argv[0] << " " <<  argparser::getUsage() << std::endl;
    return -1;
  }
  const InitialSimulationConfiguration isc = configuration.initialSimulationConfiguration.value_or(DEFAULT_SIMULATION_CONFIGURATION);

  // hate this
  _5mthf = isc.thf_5m.has_value() ? isc.thf_5m.value() * MU: _5mthf;

  static MetabolicModel localModel(
    isc.initialMet.value_or(100),
    isc.initialAdoMet.value_or(100),
    isc.initialAdoHcy.value_or(100),
    isc.initialHcy.value_or(100),
    isc.metinMax.value_or(100)
  );
  model = &localModel;

  SetOutput(configuration.imagePath.value_or(DEFAULT_IMAGE_PATH).c_str());
  Print("# Time Met AdoMet AdoHcy Hcy\n");

  const double startTime = 0;
  maxTime = configuration.endTime.value_or(DEFAULT_END_TIME); // 3600 seconds
  Init(startTime, maxTime);
  SetStep(1e-6, 1);
  SetAccuracy(1e-3, 1e-1);

  Run();

  return 0;
}

