#include "advdiff.h"
#include <time.h>

int main(int argc, char **argv) {
    double  *Un = NULL, *Unp1 = NULL, *Xval = NULL, *Utmp = NULL;
    unsigned int choix = 0;
    unsigned int choix_discret = 0;
    int bc[4] = {0, 0, 0, 0};
    double operator[3] = {-999, -999, -999};
    char cBuffer[256] = {0};
    unsigned int i = 0, j = 0;
    psParam psp = NULL;
    FILE *file;
    pfFuncTime pfTime = NULL;
    pfProfInit pfFuncU0 = NULL;
    pfLinOp pf_linear_op = compute_linear_rhs;
    clock_t startt = clock();
    clock_t endt = clock();
    double etime = 0.;
    double current_time =0.;
    double correction_factor = 1;

    /* allocation de la structure */
    psp = (sParam *) malloc(sizeof(*psp));

    /* initialisation de la structure et des parametres */
    fprintf(stdout, "***Parameters***\n");
    init_parameters(psp);

    fprintf(stdout, "***discretization in space ***\n");
    fprintf(stdout, "1-Forward scheme\n");
    fprintf(stdout, "2-Backward scheme\n");
    fprintf(stdout, "3-Centered scheme\n");
    choix_discret = get_uint();
    switch (choix_discret) {
        case 1:
            operator[0] = 0., operator[1] = -1., operator[2] = 1.;
            break;
        case 2:
            operator[0] = -1., operator[1] = 1., operator[2] = 0.;
            break;
        case 3:
            operator[0] = -.5, operator[1] = 0., operator[2] = .5;
            break;
        default:
            printf("choice not valid");
            break;
    }

    fprintf(stdout, "***Boundary Conditions***\n");
    fprintf(stdout, "1-Dirichlet\n");
    fprintf(stdout, "2-Neumann\n");
    fprintf(stdout, "3-Periodic\n");
    choix = get_uint();

    switch (choix) {
        case 1:
            bc[0] = -1, bc[1] = 0, bc[2] = -1, bc[3] = 0;
            break;
        case 2:
            bc[0] = 1, bc[1] = 0, bc[2] = 1, bc[3] = 0;
            break;
        case 3:
            bc[0] = 0, bc[1] = 1, bc[2] = 0, bc[3] = 1;
            break;
        default:
            break;
    }


    fprintf(stdout, "***time scheme***\n");
    fprintf(stdout, "1-Euler explicite\n");
    fprintf(stdout, "2-Euler implicite\n");
    fprintf(stdout, "3-Runge-Kutta 4\n");
    fprintf(stdout, "4-Crank-Nicolson\n");
    choix = get_uint();

    switch (choix) {
        case 1:
            pfTime = resol_euler_expl;
            break;
        case 2:
            pfTime = resol_euler_impl;
            break;
        case 3:
            pfTime = resol_rk4;
            break;
        case 4:
            pfTime = resol_semi_impl;
            correction_factor=.5;
        default:
            break;
    }


    fprintf(stdout, "***initial condition***\n");
    fprintf(stdout, "1-Sinus\n");
    fprintf(stdout, "2-Gaussienne\n");
    fprintf(stdout, "3-Heaviside\n");
    fprintf(stdout, "4-Gate\n");
    choix = get_uint();

    switch (choix) {
        case 1:
            pfFuncU0 = sinus;
            psp->sigma /= ((psp->dx) * (psp->Nx - 3.));
            break;
        case 2:
            pfFuncU0 = gaussienne;
            break;
        case 3:
            pfFuncU0 = heavyside;
            break;
        case 4:
            pfFuncU0 = gate;
            break;
        default:
            break;
    }

    /* allocation memoire pour les tableaux */
    Un = (double *) malloc(sizeof(*Un) * psp->Nx);
    Unp1 = (double *) malloc(sizeof(*Unp1) * psp->Nx);
    Utmp = (double *) malloc(sizeof(*Utmp) * psp->Nx);
    Xval = (double *) malloc(sizeof(*Xval) * psp->Nx);

    /* initialisation des valeurs de x et u0 */
    init_profiles(Xval, Un, psp, pfFuncU0);

    dcopy(psp->Nx, Un, Unp1);

    initialize_linear_operator(operator, correction_factor*psp->C, correction_factor*psp->V, psp->dt, psp->dx);
    init_bicg(psp->pbicg, psp->Nx, bc, operator);
    dcopy(3, operator, psp->operator);
    icopy(4, bc, psp->bc);

    /* ecriture des profils initiaux */
    GenerateFileName("out_", cBuffer);
    file = fopen(cBuffer, "w+");
    fclose(file);
    writeProfil(cBuffer, Xval, Un, current_time, psp->Nx);

    startt = clock();
    /* boucle en temps */
    for (i = 0; i < psp->Npas; i += psp->Nout) {
        for (j = 0; j < psp->Nout; j++) {
            /* avancement en temps */
            pfTime(Xval, Un, Unp1, psp, pf_linear_op);
            /*pour le pas d'apres Unp1 va devenir le nouveau Un */
            swap_fields(&Un, &Unp1);
            current_time+=psp->dt;
        }
        /*ecriture des profils tout les Nout iterations */
        GenerateFileName("out_", cBuffer);
        writeProfil(cBuffer, Xval, Un,current_time, psp->Nx);
    }
    endt = clock();
    etime = (float) (endt - startt) / CLOCKS_PER_SEC;

    print_parameters_as_json(psp);
    fprintf(stdout, "Elapsed time %f s\n", etime);

    /* liberation de la memoire */
    free_parameters(psp);
    free(psp), psp = NULL;
    free(Xval), Xval = NULL;
    free(Unp1), Unp1 = NULL;
    free(Un), Un = NULL;
    free(Utmp), Utmp = NULL;
    return 0;
}


/* utilisation securisee de scanf pour saisir un nombre entier */
unsigned int get_uint(void) {
    unsigned int nombre = 0;
    int isOk = 0;

    while (!isOk) {

        int retour = scanf("%u%*[^\n]", &nombre);

        if (!retour) {
            /* erreur de saisie, on vide le flux */
            int c;
            while (((c = getchar()) != '\n') && c != EOF);
        } else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */
            isOk = 1;  /* sort de la boucle */
        }
    }
    return nombre;
}

/* utilisation securisee de scanf pour saisir un nombre decimal */
double get_double(void) {
    double nombre = 0;
    int isOk = 0;

    while (!isOk) {
        int retour = scanf("%lf%*[^\n]", &nombre);
        if (!retour) {
            /* erreur de saisie, on vide le flux */
            int c;
            while (((c = getchar()) != '\n') && c != EOF);
        } else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */
            isOk = 1;  /* sort de la boucle */
        }
    }
    return nombre;
}


void compute_linear_rhs(
        double const *const restrict adv_factor,
        double const *const restrict v,
        double *const restrict u,
        size_t const size) {
    long unsigned int const stop = size - 1;
    long unsigned int i, start = 1;
    double const cvm1 = adv_factor[0];
    double const cv = adv_factor[1];
    double const cvp1 = adv_factor[2];
    double vxm1 = 0., vx = v[0], vxp1 = v[1];

    for (i = start; i < stop; i++) {
        vxm1 = vx;
        vx = vxp1;
        vxp1 = v[i + 1];
        u[i]= cvm1 * vxm1
                + cv * vx
                + cvp1 * vxp1;
    }
}

void initialize_linear_operator(double *const operator, double const C, double const V, double const dt, double const dx) {

    if (fabs(V) > 1e-8) {
        operator[0] *= V * dt / dx;
        operator[1] *= V * dt / dx;
        operator[2] *= V * dt / dx;
    } else {
        operator[0] = 0.;
        operator[1] = 0.;
        operator[2] = 0.;
    }
    if (fabs(C) > 1e-8) {
        operator[0] += C * dt / (dx * dx);
        operator[1] -= 2. * C * dt / (dx * dx);
        operator[2] += C * dt / (dx * dx);
    }
}

/* demande de saisie des parametres utilisateurs */
int init_parameters(psParam const psp) {
    fprintf(stdout, "Nx=?");
    psp->Nx = get_uint();
    fprintf(stdout, "Npas=?");
    psp->Npas = get_uint();
    fprintf(stdout, "Nout=?");
    psp->Nout = get_uint();
    fprintf(stdout, "C=?");
    psp->C = get_double();
    fprintf(stdout, "V=?");
    psp->V = get_double();
    fprintf(stdout, "A=?");
    psp->A = get_double();
    fprintf(stdout, "x0=?");
    psp->x0 = get_double();
    fprintf(stdout, "sigma=?");
    psp->sigma = get_double();
    fprintf(stdout, "Dx=?");
    psp->dx = get_double();
    fprintf(stdout, "Dt=?");
    psp->dt = get_double();

    psp->du1 = (double *) malloc(sizeof(*psp->du1) * psp->Nx);
    psp->du2 = (double *) malloc(sizeof(*psp->du2) * psp->Nx);
    psp->du3 = (double *) malloc(sizeof(*psp->du3) * psp->Nx);
    psp->du4 = (double *) malloc(sizeof(*psp->du4) * psp->Nx);
    psp->Utmp = (double *) malloc(sizeof(*psp->Utmp) * psp->Nx);
    psp->operator = (double *) malloc(sizeof(*psp->operator) * 3);
    psp->bc = (int *) malloc(sizeof(*psp->bc) * 4);
    psp->pbicg = (BicgStruct *) malloc(sizeof(*psp->pbicg));
    return 0;
}

int print_parameters_as_json(psParam const psp) {
    fprintf(stdout, "{params:\n {\n");
    fprintf(stdout, "Nx: \"%d\",\n", psp->Nx);
    fprintf(stdout, "Npas: \"%d\",\n", psp->Npas);
    fprintf(stdout, "Nout: \"%d\",\n", psp->Nout);
    fprintf(stdout, "C: \"%f\",\n", psp->C);
    fprintf(stdout, "V: \"%f\",\n", psp->V);
    fprintf(stdout, "A: \"%f\",\n", psp->A);
    fprintf(stdout, "x0: \"%f\",\n", psp->x0);
    fprintf(stdout, "sigma: \"%f\",\n", psp->sigma);
    fprintf(stdout, "Dx: \"%f\",\n", psp->dx);
    fprintf(stdout, "Dt: \"%f\"\n}\n}\n", psp->dt);

    return 0;
}

/* demande de saisie des parametres utilisateurs */
int print_parameters(psParam const psp) {
    fprintf(stdout, "Nx= %d\n", psp->Nx);
    fprintf(stdout, "Npas= %d\n", psp->Npas);
    fprintf(stdout, "Nout= %d\n", psp->Nout);
    fprintf(stdout, "C= %f\n", psp->C);
    fprintf(stdout, "V= %f\n", psp->V);
    fprintf(stdout, "A= %f\n", psp->A);
    fprintf(stdout, "x0= %f\n", psp->x0);
    fprintf(stdout, "sigma= %f\n", psp->sigma);
    fprintf(stdout, "Dx= %f\n", psp->dx);
    fprintf(stdout, "Dt= %f\n", psp->dt);

    return 0;
}

/*liberation de la memoire associee a une structure sParam */
int free_parameters(const psParam psp) {
    free(psp->du1);
    free(psp->du2);
    free(psp->du3);
    free(psp->du4);
    free(psp->Utmp);
    free(psp->bc);
    free(psp->operator);
    free_bicg(psp->pbicg);
    free(psp->pbicg);

    return 0;
}

/*condition de bords avec une precision dx^2*/
int boundary(double *const u, int const *const bc, const unsigned int N) {
    int const bc_size = 4;

    u[0] = bc[0] * u[2] + bc[1] * u[N - 2];
    u[N - 1] = bc[bc_size - 2] * u[N - 3] + bc[bc_size - 1] * u[1];

    return 0;
}


/*creation d'un profil à partir d'une fonction donnee */
int init_profiles(double *const restrict x, double *const restrict u0, const sParam *const restrict psp, const pfProfInit pfpi) {
    unsigned int i;

    for (i = 0; i < psp->Nx; i++) {
        x[i] = ((double) i - 1.) * psp->dx;
        u0[i] = pfpi(psp->x0, x[i], psp->A, psp->sigma);
    }
    return 0;
}

/* ecriture du profil dans un fichier */
int writeProfil(const char *const cFileName, const double *const restrict x,
                const double *const restrict u, double const t, const size_t N) {
    FILE *file = NULL;
    unsigned int i = 0;

    file = fopen(cFileName, "a");
    fprintf(file, "#time:%lf\n", t);
    for (i = 1; i < N - 1; i++)
        fprintf(file, "%lf\t%lf\n", x[i], u[i]);
    fclose(file), file = NULL;
    return 0;
}

/* inversion des tableaux associes aux variables f1 et f2 */
int swap_fields(double **f1, double **f2) {
    double *ptmp;
    ptmp = *f1;
    *f1 = *f2;
    *f2 = ptmp;
    return 0;
}

/*algorithme d'Euler */
int resol_euler_expl(const double *const x, const double *const un, double *const unp1, const sParam *const psp,
                     pfLinOp pfop) {
    double *const restrict Utmp = psp->Utmp;

    pfop(psp->operator, un, Utmp, psp->Nx);
    vec_xpby(unp1+1, un+1, Utmp+1, 1, psp->Nx-2);
    boundary(unp1, psp->bc, psp->Nx);
    return 0;
}

/*algorithme d'Euler implicite*/
int resol_euler_impl(const double *const x, const double *const un, double *const unp1, const sParam *const psp,
                     pfLinOp pfop) {
    solve_matrix(psp->pbicg, unp1, un);
    boundary(unp1, psp->bc, psp->Nx);
    return 0;
}


int resol_semi_impl(const double *const x, const double *const un, double *const unp1, const sParam *const psp,
                     pfLinOp pfop) {
    resol_euler_expl(x, un, psp->Utmp, psp, pfop);
    resol_euler_impl(x, psp->Utmp, unp1, psp, pfop);
    return 0;
}


/*algorithme de RK4 */
int
resol_rk4(const double *const x, const double *const un, double *const unp1, const sParam *const psp, pfLinOp pfop) {
    double *const restrict du1 = psp->du1;
    double *const restrict du2 = psp->du2;
    double *const restrict du3 = psp->du3;
    double *const restrict du4 = psp->du4;
    double *const restrict Utmp = psp->Utmp;
    unsigned int i = 0;
    size_t const loc_size = psp->Nx-2;

    /* calcul des termes */
    pfop(psp->operator, un, du1, psp->Nx);
    vec_xpby(Utmp+1, un+1, du1+1, 0.5, loc_size);
    boundary(Utmp, psp->bc, psp->Nx);

    pfop(psp->operator, Utmp, du2, psp->Nx);
    vec_xpby(Utmp+1, un+1, du2+1, 0.5, loc_size);
    boundary(Utmp, psp->bc, psp->Nx);

    pfop(psp->operator, Utmp, du3, psp->Nx);
    vec_xpby(Utmp+1, un+1, du3+1, 1., loc_size);
    boundary(Utmp, psp->bc, psp->Nx);

    pfop(psp->operator, Utmp, du4, psp->Nx);

    for (i = 1; i <= loc_size; i++) {
        unp1[i] = un[i] + (1. / 6.) * (du1[i] + 2. * du2[i] + 2. * du3[i] + du4[i]);
    }
    boundary(unp1, psp->bc, psp->Nx);
    return 0;
}

/* teste si un fichier existe, renvoie 1 si vrai et 0 si faux */
int isFileExist(const char *const filename) {
    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL)
        return 0;

    fclose(file);
    return 1;
}

/* cree un nom de fichier a partir d'un prefixe qui sera incremente a chaque fois */
int GenerateFileName(const char *const cFilePrefix, char *const cGenerateName) {
    char buffer[256] = {0};
    unsigned int i = 0;

    if ((cFilePrefix == NULL) || (cGenerateName == NULL))
        return 1;

    sprintf(buffer, "%s_%04d.dat", cFilePrefix, i);
    while ((i < 10000) && (isFileExist(buffer) == 1)) {
        i++;
        sprintf(buffer, "%s_%04d.dat", cFilePrefix, i);
    }

    if (i == 10000)
        return 1;

    strcpy(cGenerateName, buffer);
    return 0;
}


double sinus(const double x0, const double x, const double A, const double sigma) {
    double const dpi = 4. * atan(1.);
    double const res =  A * sin(dpi * (x - x0) * sigma);
    return res;
}


double gaussienne(const double x0, const double x, const double A, const double sigma) {
    double const res =  A * exp(-((x - x0) * (x - x0)) / (sigma * sigma));
    return res;
}


double porte(const double x0, const double x, const double A, const double sigma) {
    double res = 0.0;

    if ((x > (x0 - sigma * 0.5)) && (x < (x0 + sigma * 0.5)))
        res = A;
    return res;
}


double heavyside(const double x0, const double x, const double A, const double sigma) {
    double res = 0.0;

    if (x >= (x0))
        res = A;
    return res;
}


double gate(const double x0, const double x, const double A, const double sigma) {
    double res = 0.0;

    if (x >= (x0 - sigma) || (x <= (x0 + sigma)))
        res = A;
    return res;
}