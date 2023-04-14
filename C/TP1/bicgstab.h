//
// Created by fuhrg on 07/04/2023.
//

#ifndef C_BICGSTAB_H
#define C_BICGSTAB_H

#define TOL 1e-6
#define MAX_ITER 1000

#include "math.h"
#include "malloc.h"
#include <stdio.h>


typedef struct _BicgStruct {
    double * restrict work;
    double * restrict mat_band;
    size_t vec_size;
} BicgStruct, *PBicgStruct;

void init_bicg(BicgStruct *const restrict bicgs, const size_t vec_size,
               int const *const restrict bc, double const *const restrict operator);
void free_bicg(BicgStruct * restrict bicgs);

void solve_matrix( BicgStruct *const restrict bicg,
                  double *const restrict sol, double const*const restrict rhs);


void dcopy(const size_t vec_size, const double *const restrict src, double *const restrict dest);
void icopy(const size_t vec_size, const int *const restrict src, int *const restrict dest);
void dset(const size_t vec_size, double  *const restrict dest, double const val);

void vec_xpby(double *const restrict res, const double *const restrict x, const double *const restrict y,
              const double factor, const size_t vec_size);

#endif //C_BICGSTAB_H
