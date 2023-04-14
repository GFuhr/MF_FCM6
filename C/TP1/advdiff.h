#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "bicgstab.h"


/* structure contenant l'ensemble des parametres, tableaux intermediaires importants */
typedef struct
{
    unsigned int Nx;
    unsigned int Npas;
    unsigned int Nout;
    int *bc;
    double dx;
    double dt;
    double V;
    double C;
    double A;
    double x0;
    double sigma;
    double *Utmp;
    double *du1;
    double *du2;
    double *du3;
    double *du4;
    double *operator;
    PBicgStruct pbicg;
}sParam, *psParam;

/*pointeurs de fonctions */
typedef void (*pfLinOp)(const double *const , const double *const ,  double *const , const size_t );
typedef int (*pfFuncOp)     (const double *const , const double *const ,  double *const , const sParam *const ,const double );
typedef int (*pfFuncTime)   (const double *const , const double *const ,  double *const , const sParam *const , pfLinOp );
typedef double (*pfProfInit)(const double        , const double        , const double   , const double);

static void initialize_linear_operator(double *const operator, double const C, double const V, double const dt, double const dx);

/*routine de calcul des differents operateurs */

void compute_linear_rhs(
        double const *const restrict adv_factor,
        double const *const restrict v,
        double *const restrict u,
        size_t const size);

/*routines de creation des champs, calcul des conditions de bords...*/
int boundary(double *const u, int const*const bc,const unsigned int N);
int init_profiles(double *const restrict x, double *const restrict u0, const sParam *const restrict psp, const pfProfInit pfpi);
double sinus(const double, const double, const double, const double);
double gaussienne(const double, const double, const double, const double);
double porte(const double, const double, const double, const double);
double heavyside(const double, const double, const double, const double);
double gate(const double, const double, const double, const double);

int init_parameters(psParam const psp);
int free_parameters(const psParam psp);
int print_parameters(psParam const psp);
int print_parameters_as_json(psParam const psp);

int swap_fields(double **f1, double **f2);

/*schemas temporels */
int resol_rk4   (const double *const x, const double *const un, double *const unp1, const sParam *const psp, pfLinOp pfop);
int resol_euler_expl (const double *const x, const double *const un, double *const unp1, const sParam *const psp, pfLinOp pfop);
int resol_euler_impl(const double *const x, const double *const un, double *const unp1, const sParam *const psp, pfLinOp pfop);
int resol_semi_impl(const double *const x, const double *const un, double *const unp1, const sParam *const psp,
                    pfLinOp pfop);

/*saisies utilisateurs */
unsigned int get_uint(void);
double get_double(void);

/*routine d'ecriture des profil/gestion des noms de fichiers */
int isFileExist(const char *const cFileName);
int GenerateFileName(const char *const cFilePrefix, char *const cGenerateName);
int writeProfil(const char *const cFileName, const double *const restrict x,
                const double *const restrict u, double const t, const size_t N);