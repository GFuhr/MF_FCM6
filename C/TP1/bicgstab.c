//
// Created by fuhrg on 07/04/2023.
//

#include "bicgstab.h"

static void triband_dot(double const *const restrict band_matrix,
                        double const *const restrict x, double *const restrict result,
                        const size_t matrix_size);

static double single_vec_dot(double const *const x, const size_t vec_size);

static double vec_dot(double const *const restrict x, double const *const restrict y, const size_t vec_size);

static void vec_xmy(double *const restrict res, const double *const restrict x, const double *const restrict y,
                    const size_t vec_size);



static void d_axpbypcz(const size_t vec_size, double const *const restrict x, const double a,
                       double const *const restrict y, const double b,
                       double *const restrict z, const double c);

static void
bicgstab_c(const size_t vec_size, BicgStruct *const restrict bicgs, double const *const restrict rhs,
           double *const restrict x,
           const double tol, unsigned const int max_iter);


void free_bicg(BicgStruct *restrict bicgs) {
    free(bicgs->mat_band), bicgs->mat_band = NULL;
    free(bicgs->work), bicgs->work = NULL;

}

void init_bicg(BicgStruct *const restrict bicgs, const size_t vec_size,
               int const *const restrict bc, double const *const restrict operator) {
    size_t i = 0;
    double const local_operator[] = {-operator[0], 1 - operator[1], -operator[2]};
    bicgs->vec_size = vec_size - 2;
    bicgs->work = (double *) malloc(7 * bicgs->vec_size * sizeof(*bicgs->work));
    bicgs->mat_band = (double *) malloc(5 * 3 * sizeof(*bicgs->mat_band));

    for (i = 0; i < 7 * bicgs->vec_size; i++)
        bicgs->work[i] = 0.;

    bicgs->mat_band[0] = 0.;
    bicgs->mat_band[1] = 0;
    bicgs->mat_band[2] = local_operator[1];
    // to take into account dirichlet or neumann boundary conditions on the left
    bicgs->mat_band[3] = local_operator[2] + bc[0] * local_operator[0];
    // case where box is periodic on the left side
    bicgs->mat_band[4] = bc[1] * local_operator[0];

    bicgs->mat_band[5] = 0.;
    bicgs->mat_band[6] = local_operator[0];
    bicgs->mat_band[7] = local_operator[1];
    bicgs->mat_band[8] = local_operator[2];
    bicgs->mat_band[9] = 0;

    // case where box is periodic on the right side
    bicgs->mat_band[10] = bc[3] * local_operator[2];
    // to take into account dirichlet or neumann bondary conditions on the right
    bicgs->mat_band[11] = local_operator[0] + bc[2] * local_operator[2];
    bicgs->mat_band[12] = local_operator[1];
    bicgs->mat_band[13] = 0;
    bicgs->mat_band[14] = 0;
}


void solve_matrix(BicgStruct *const restrict bicg, double *const restrict sol, double const *const restrict rhs) {
    return bicgstab_c(bicg->vec_size, bicg, rhs + 1, sol + 1, TOL, MAX_ITER);
}


void triband_dot(double const *const restrict band_matrix,
                 double const *const restrict x, double *const restrict result,
                 const size_t matrix_size) {
    size_t idx;
    double const bmm1 = band_matrix[6];
    double const bm = band_matrix[7];
    double const bmp1 = band_matrix[8];
    double xm1, xi, xp1;

    xi = x[0];
    xp1 = x[1];
    result[0] = band_matrix[2] * xi
                + band_matrix[3] * xp1\
 + band_matrix[4] * x[matrix_size - 1];
    for (idx = 1; idx < matrix_size - 1; idx++) {
        xm1 = xi;
        xi = xp1;
        xp1 = x[idx + 1];

        result[idx] = bmm1 * xm1
                      + bm * xi
                      + bmp1 * xp1;
    }
    idx = matrix_size - 1;
    result[idx] = band_matrix[11] * x[idx - 1]
                  + band_matrix[12] * x[idx]
                  + band_matrix[10] * x[0];
}


double single_vec_dot(double const *const x, const size_t vec_size) {
    double dot = 0.;
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    for (idx = 0; idx < idx_end; idx += 4) {
        dot += x[idx] * x[idx]
               + x[idx + 1] * x[idx + 1]
               + x[idx + 2] * x[idx + 2]
               + x[idx + 3] * x[idx + 3];
    }
    for (idx = idx_end; idx < vec_size; ++idx) {
        dot += x[idx] * x[idx];
    }
    return dot;
}


double vec_dot(double const *const restrict x, double const *const restrict y, const size_t vec_size) {
    double dot = 0.0;
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    for (idx = 0; idx < idx_end; idx += 4) {
        dot += x[idx] * y[idx]
               + x[idx + 1] * y[idx + 1]
               + x[idx + 2] * y[idx + 2]
               + x[idx + 3] * y[idx + 3];
    }
    for (; idx < vec_size; ++idx) {
        dot += x[idx] * y[idx];
    }
    return dot;
}


void vec_xmy(double *const restrict res, const double *const restrict x, const double *const restrict y,
             const size_t vec_size) {
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    for (idx = 0; idx < idx_end; idx += 4) {
        res[idx] = x[idx] - y[idx];
        res[idx + 1] = x[idx + 1] - y[idx + 1];
        res[idx + 2] = x[idx + 2] - y[idx + 2];
        res[idx + 3] = x[idx + 3] - y[idx + 3];
    }
    for (; idx < vec_size; ++idx) {
        res[idx] = x[idx] - y[idx];
    }
};


void vec_xpby(double *const restrict res, const double *const restrict x, const double *const restrict y,
              const double factor, const size_t vec_size) {
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    for (idx = 0; idx < idx_end; idx += 4) {
        res[idx] = x[idx] + factor * y[idx];
        res[idx + 1] = x[idx + 1] + factor * y[idx + 1];
        res[idx + 2] = x[idx + 2] + factor * y[idx + 2];
        res[idx + 3] = x[idx + 3] + factor * y[idx + 3];
    }
    for (; idx < vec_size; ++idx) {
        res[idx] = x[idx] + factor * y[idx];
    }
};

void dset(const size_t vec_size, double  *const restrict dest, double const val) {
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    if (vec_size <= 4) {
        for (idx = 0; idx < vec_size; ++idx) {
            dest[idx] = val;
        }
    } else {
        for (idx = 0; idx < idx_end; idx += 4) {
            dest[idx] = val;
            dest[idx + 1] = val;
            dest[idx + 2] = val;
            dest[idx + 3] = val;
        }
        for (; idx < vec_size; ++idx) {
            dest[idx] = val;
        }
    }
};

void dcopy(const size_t vec_size, const double *const restrict src, double *const restrict dest) {
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    if (vec_size <= 4) {
        for (idx = 0; idx < vec_size; ++idx) {
            dest[idx] = src[idx];
        }
    } else {
        for (idx = 0; idx < idx_end; idx += 4) {
            dest[idx] = src[idx];
            dest[idx + 1] = src[idx + 1];
            dest[idx + 2] = src[idx + 2];
            dest[idx + 3] = src[idx + 3];
        }
        for (; idx < vec_size; ++idx) {
            dest[idx] = src[idx];
        }
    }
};


void icopy(const size_t vec_size, const int *const restrict src, int *const restrict dest) {
    size_t idx = 0;
    size_t idx_end = vec_size - vec_size % 4;

    if (vec_size <= 4) {
        for (idx = 0; idx < vec_size; ++idx) {
            dest[idx] = src[idx];
        }
    } else {
        for (idx = 0; idx < idx_end; idx += 4) {
            dest[idx] = src[idx];
            dest[idx + 1] = src[idx + 1];
            dest[idx + 2] = src[idx + 2];
            dest[idx + 3] = src[idx + 3];
        }
        for (; idx < vec_size; ++idx) {
            dest[idx] = src[idx];
        }
    }
}


void d_axpbypcz(const size_t vec_size, double const *const restrict x, const double a,
                double const *const restrict y, const double b,
                double *const restrict z, const double c) {
    size_t idx;
    size_t idx_end = vec_size - vec_size % 4; // align size to a multiple of 4

    for (idx = 0; idx < idx_end; idx += 4) {
        z[idx] = a * x[idx] + b * y[idx] + c * z[idx];
        z[idx + 1] = a * x[idx + 1] + b * y[idx + 1] + c * z[idx + 1];
        z[idx + 2] = a * x[idx + 2] + b * y[idx + 2] + c * z[idx + 2];
        z[idx + 3] = a * x[idx + 3] + b * y[idx + 3] + c * z[idx + 3];
    }

    for (; idx < vec_size; idx++) {
        z[idx] = a * x[idx] + b * y[idx] + c * z[idx];
    }
}


void
bicgstab_c(const size_t vec_size, BicgStruct *const restrict bicgs, double const *const restrict rhs,
           double *const restrict x,
           const double tol, unsigned const int max_iter) {
    double rho_i, beta, alpha = 0, omega_i = 1, res, rho_im1 = 1;
    double *const work = bicgs->work;
    double *const r_i = work;
    double *const s_i = work + bicgs->vec_size;
    double *const v_i = work + 2 * bicgs->vec_size;
    double *const p_i = work + 3 * bicgs->vec_size;
    double *const t_i = work + 4 * bicgs->vec_size;
    double *const r_hat_0 = work + 5 * bicgs->vec_size;
    double *const dummy = work + 6 * bicgs->vec_size;
    double const *const mat = bicgs->mat_band;
    unsigned  int i;

    triband_dot(mat, x, dummy, vec_size);
    vec_xmy(r_i, rhs, dummy, vec_size);
    dcopy(vec_size, r_i, r_hat_0);

    dset(vec_size, p_i, 0);
    dset(vec_size, t_i, 0);
    dset(vec_size, v_i, 0);

    for (i = 0; i < max_iter; i++) {
        rho_i = vec_dot(r_hat_0, r_i, vec_size);
        beta = (rho_i / rho_im1) * (alpha / omega_i);
        d_axpbypcz(vec_size, r_i, 1, v_i, -beta * omega_i, p_i, beta);

        // v_i =
        triband_dot(mat, p_i, v_i, vec_size);
        alpha = rho_i / vec_dot(r_hat_0, v_i, vec_size);
        //s_i = r_i - alpha * v_i
        vec_xpby(s_i, r_i, v_i, -alpha, vec_size);

        // t_i
        triband_dot(mat, s_i, t_i, vec_size);
        omega_i = vec_dot(t_i, s_i, vec_size) / single_vec_dot(t_i, vec_size);
        d_axpbypcz(vec_size, p_i, alpha, s_i, omega_i, x, 1);

        //r_i = s_i - omega_i * t_i
        vec_xpby(r_i, s_i, t_i, -omega_i, vec_size);
        rho_im1 = rho_i;
        res = single_vec_dot(r_i, vec_size);
        if (fabs(res) < tol) {
            break;
        }
    }
}