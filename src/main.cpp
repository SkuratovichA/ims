#include "simlib.h"
#include <assert.h>
#include <cstdlib>


constexpr double MU_MOL = 1e-6;
constexpr double _5mthf = 5.2 * MU_MOL;

constexpr double V_mat1_max = 561 * MU_MOL;
constexpr double K_mat1_m = 41 * MU_MOL;
constexpr double K_mat1_i = 50 * MU_MOL;

constexpr double V_mat3_max = 22870 * MU_MOL;
constexpr double K_mat3_m2 = 21.1 * MU_MOL;

constexpr double V_gnmt_max = 10600 * MU_MOL;
constexpr double K_gnmt_m = 4500 * MU_MOL;
constexpr double K_gnmt_i = 20 * MU_MOL;

constexpr double V_meth_max = 4521 * MU_MOL;
constexpr double K_meth_m2__A = 10 * MU_MOL; // shitty name :)

const double alpha_1 = 100;
const double alpha_2 = 10;

const double beta_1 = 1.7; // Assuming this is the correct value
const double beta_2 = 30;  // Assuming this is the correct value

constexpr double V_ms_max = 500 * MU_MOL;
constexpr double K_ms_m_hcy = 0.1 * MU_MOL;
constexpr double K_ms_m5mthf = 25 * MU_MOL;
constexpr double K_ms_d = 1 * MU_MOL;

constexpr double V_bhmt_max = 2500 * MU_MOL; // velocity. µ / h
constexpr double K_bhmt_m = 12 * MU_MOL;

constexpr double DEFAULT_T1 = 30.0;


inline double V_ms(const double hcy) {
  // Michaelis-Menten equation implementation
  double numerator = V_ms_max * _5mthf * hcy;

  double denominator1 = K_ms_m_hcy * (K_ms_d + _5mthf);
  double denominator2 = hcy * (K_ms_m5mthf + _5mthf);
  double denominator = denominator1 + denominator2;

  return numerator / denominator;
}

inline double V_mat1(const double met, const double adoMet) {
  const double denominator1 = (K_mat1_m / met) * (1 + adoMet / K_mat1_i);
  const double denominator = 1 + denominator1;

  assert(denominator != 0 && "Denominiator must not be zero");

  return V_mat1_max / denominator;
}

inline double K_mat3_m1(const double adoMet) {
  const double adoMet_exp = adoMet / (adoMet + 600);

  return 2000 / (1 + 5.7 * (adoMet_exp * adoMet_exp));
}

inline double V_mat3(const double met, const double adoMet) {
  const double denominator = 1 + (K_mat3_m1(adoMet) * K_mat3_m2) / (met * (met + K_mat3_m2));

  return V_mat3_max / denominator;
}

inline double V_gnmt(const double adoMet, const double adoHey) {
  const double denominator1 = 1 + pow(K_gnmt_m / adoMet, 2.3);
  const double denominator2 = 1 + adoHey / K_gnmt_i;

  return V_gnmt_max / (denominator1 * denominator2);
}

inline double K_meth_m1(const double adoMeth) {
  return 1 + adoMeth / 4;
}

inline double V_meth(const double adoMeth) {
  const double K_meth_m1__adoMet = K_meth_m1(adoMeth) / adoMeth;
  // liteally 11 + 11K_meth_m1__adoMet :clown_face:
  const double denominator =
      (K_meth_m2__A + 1) + K_meth_m1__adoMet * (1 + K_meth_m2__A);

  return V_meth_max / denominator;
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

// ---
// TODO: rewrite
//
// Model
struct Lorenz {
  /* Integrator x1, x2, x3; */
  /* Lorenz(double sigma, double lambda, double b) */
  /*     :                           // blocks: */
  /*       x1(sigma * (x2 - x1), 1), // dx1/dt = sigma * (x2 - x1) */
  /*       x2((1 + lambda - x3) * x1 - x2, */
  /*          1),                     // dx2/dt = (1 + lambda - x3) * x1 - x2 */
  /*       x3(x1 * x2 - b * x3, 1) {} // dx3/dt = x1 * x2 - b * x3 */
};

/* Lorenz L(10, 24, 2); // Create instance of model */

/**/
// Output sampling
/* void Sample() { */
/*   Print("%6.2f %.5g %.5g %.5g\n", T.Value(), L.x1.Value(), L.x2.Value(), */
/*         L.x3.Value()); */
/* } */
/* Sampler S(Sample, 0.01); // Output step */

int main(int argc, char *argv[]) {
  double maxtime = DEFAULT_T1;

  // TODO:
  // 1. validate this
  // 2. check the expressions at the end of the function
  if (argc > 1) {
    maxtime = std::atof(argv[1]);
  } else {
    _Print("\nUsage:  %s  [maxtime>=1,default=%g] \n\n", argv[0], DEFAULT_T1);
  }

  if (maxtime < 1.0) {
    _Print("\nUsage:  %s  [maxtime>=1,default=%g] \n\n", argv[0], DEFAULT_T1);
    return 1;
  }

  /* SetOutput("lorenz.dat"); // Redirect output to file */
  /* Print("# Lorenz equation output (maxtime=%g) \n", maxtime); */
  /* Print("# Time x1 x2 \n"); */
  /* Init(0, maxtime);           // Initialize siMU_MOLlator */
  /* SetAccuracy(1e-8);          // Required accuracy */
  /* Run();                      // SiMU_MOLlate */
  /* SIMLIB_statistics.Output(); // Print siMU_MOLlation run statistics */
}
