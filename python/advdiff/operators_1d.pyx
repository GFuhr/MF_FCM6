#!python
#cython: language_level=3
#cython: boundscheck=False
#cython: wraparound=False
#cython: initializedcheck=False
#cython: nonecheck=False
#cython: cdivision=True
#cython: warn.undeclared=True



import cython
cimport numpy as np
cimport cython
from cython.parallel import prange, parallel
from cython.parallel cimport prange, parallel

from bicgstab cimport MatrixSolver



cdef void d_axpbypcz(const Py_ssize_t vec_size, const double[::1] x, const double  a,
                    const double[::1] y, const double b,
                    double[::1] z, const double c
                       ) nogil:
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = vec_size & ~3

    for idx in prange(0, idx_end, 4):
        z[idx] = a*x[idx]+b*y[idx]+c*z[idx]
        z[idx+1] = a*x[idx+1]+b*y[idx+1]+c*z[idx+1]
        z[idx+2] = a*x[idx+2]+b*y[idx+2]+c*z[idx+2]
        z[idx+3] = a*x[idx+3]+b*y[idx+3]+c*z[idx+3]

    for idx in prange(idx_end, vec_size):
        z[idx] = a*x[idx]+b*y[idx]+c*z[idx]


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void op_xpy(
        double[::1] x,
        const double[::1] y) nogil:
    cdef Py_ssize_t m=x.shape[0]
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = m & ~3

    for idx in prange(0, idx_end, 4):
            x[idx] += y[idx]
            x[idx+1] += y[idx+1]
            x[idx+2] += y[idx+2]
            x[idx+3] += y[idx+3]

    for idx in prange(idx_end, m):
        x[idx] += y[idx]

@cython.boundscheck(False)
@cython.wraparound(False)
cdef void op_copy(
        double[::1] dest,
        const double[::1] src) nogil:
    cdef Py_ssize_t m = dest.shape[0]
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = m & ~3

    for idx in prange(0, idx_end, 4):
        dest[idx] = src[idx]
        dest[idx + 1] = src[idx + 1]
        dest[idx + 2] = src[idx + 2]
        dest[idx + 3] = src[idx + 3]

    for idx in prange(idx_end, m):
        dest[idx] = src[idx]

@cython.boundscheck(False)
@cython.wraparound(False)
cdef void op_axpby(
            const double[::1] x,
            const double a,
            double[::1] y,
            const double b) nogil:
    cdef Py_ssize_t i = 0
    cdef Py_ssize_t m=x.shape[0]

    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = m & ~3

    for idx in prange(0, idx_end, 4):
        y[idx] = a*x[idx]+b*y[idx]
        y[idx+1] = a*x[idx+1]+b*y[idx+1]
        y[idx+2] = a*x[idx+2]+b*y[idx+2]
        y[idx+3] = a*x[idx+3]+b*y[idx+3]

    for idx in prange(idx_end, m):
        y[idx] = a*x[idx]+b*y[idx]

@cython.boundscheck(False)
@cython.wraparound(False)
cdef void op_xpby(
           double[::1] x,
           const double[::1] y,
           const double b):
    cdef Py_ssize_t i = 0
    cdef Py_ssize_t m=x.shape[0]
    with nogil, parallel(num_threads=8):
        for i in prange(0, m):
            x[i] += b*y[i]


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void op_zero(
        double[::1] x):
    cdef Py_ssize_t m=x.shape[0]
    cdef Py_ssize_t i = 0
    with nogil, parallel(num_threads=8):
        for i in prange(0, m):
            x[i] = 0.


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void diffusion(const double dx, const double c,
               const double[::1] v,
               double[::1] u):
    cdef Py_ssize_t m=u.shape[0]-1
    cdef Py_ssize_t i
    cdef double hx = c/(dx*dx)

    with nogil, parallel(num_threads=8):
        for i in prange(1, m):
            u[i] += hx*(v[i+1] + v[i-1] - 2*v[i])


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void diffusion_coef(const double dx, const double Lx,
                    const double[::1] chi,
                    const double[::1] dchi,
                    const double[::1] v,
                    double[::1] u):
    cdef Py_ssize_t m = u.shape[0]-1
    cdef Py_ssize_t i
    cdef double hx = 1/dx**2
    cdef double lxm2 = 1./(Lx*Lx)
    with nogil, parallel(num_threads=8):
        for i in prange(1, m):
            u[i] += lxm2*(chi[i]*hx*(v[i+1] + v[i-1] - 2*v[i]) + dchi[i]*(v[i+1] + v[i-1])*.5/dx)



@cython.boundscheck(False)
@cython.wraparound(False)
cdef void advection(double dx, const double[3] adv_factor,
               const double[::1] v,
               double[::1] u):
    cdef Py_ssize_t m = u.shape[0]-1
    cdef Py_ssize_t i, start = 1

    with nogil, parallel(num_threads=4):
        for i in prange(start, m):
                u[i] += (adv_factor[0]*v[i-1] + adv_factor[1]*v[i] + adv_factor[2]*v[i+1])

@cython.boundscheck(False)
@cython.wraparound(False)
cdef void advection_diffusion(
               const double[::1] adv_factor,
               const double[::1] v,
               double[::1] u) nogil:
    cdef Py_ssize_t stop = u.shape[0]-1
    cdef Py_ssize_t i, start = 1
    cdef double cvm = adv_factor[0]
    cdef double cv = adv_factor[1]
    cdef double cvp = adv_factor[2]
    cdef double vxm1, vx, vxp1

    #with nogil:
    vxm1 = v[0]
    vx = v[0]
    vxp1 = v[1]

    for i in range(start, stop):
        vxm1 = vx
        vx = vxp1
        vxp1 = v[i+1]
        u[i] = cvm*vxm1 \
                + cv*vx \
                + cvp*vxp1



@cython.boundscheck(False)
@cython.wraparound(False)
cdef void boundary(
    double[::1] u,
    const long[::1] bc) nogil:
    cdef Py_ssize_t m = u.shape[0]-1
    cdef Py_ssize_t bc_size = bc.shape[0]

    u[0] = bc[0]*u[2] + bc[1]*u[m-1]
    u[m] = bc[bc_size-2]*u[m-2] + bc[bc_size-1]*u[1]

@cython.boundscheck(False)
@cython.wraparound(False)
cdef void time_step(
                double[::1] Field_p,
                double[::1] pp1,
                const double[::1] adv_factor) nogil:

    advection_diffusion(adv_factor, Field_p, pp1 )

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void eule(
        double[::1] rhs,
        double[::1] Field_p,
        const double[::1] adv_factor,
        const long[::1] bc):
    """
        compute explicit euler time step
        Field_w = Field_w + dt* (-V d/dx + C d2/dx2)
        params Field_w :
        params rhs : temporary array to compute RHS of equation
        params kwargs : physical parameters (C, V, dx, dt)
    """

    time_step(Field_p, rhs, adv_factor)
    #op_xpy(Field_p, rhs)
    op_axpby(rhs, 1, Field_p, 1)
    boundary(Field_p, bc)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void euli(MatrixSolver matA,
         double[::1] pp1,
         double[::1] Field_p, const long[::1] bc):


    matA.solve(Field_p, pp1)
    op_copy(Field_p, pp1)

    boundary(Field_p, bc)

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void CranckN(MatrixSolver matA,
            double[::1] rhs,
            double[::1] Field_p,
            const double[::1] adv_factor,
            const long[::1] bc):

    eule(rhs, Field_p, adv_factor, bc)
    euli(matA, rhs, Field_p, bc)


@cython.boundscheck(False)
@cython.wraparound(False)
cdef void RK_step(
        double[::1] Field_p,
        double[::1] ki,
        double[::1] ppi,
        const double[::1] adv_factor,
        const long[::1] bc,
        const double gamma) nogil:

    cdef Py_ssize_t m = Field_p.shape[0]-1
    cdef Py_ssize_t idx_x, start = 1

    time_step(Field_p, ki, adv_factor)
    op_copy(ppi, Field_p)
    op_axpby(ki, gamma, ppi, 1)
   #  for idx_x in prange(start, m):
   #      ppi[idx_x] = Field_p[idx_x] + gamma*ki[idx_x]
    boundary(ppi, bc)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void RK4(
    double[::1] k1,
        double[::1] k2,
        double[::1] k3,
        double[::1] k4,
        double[::1] y1,
        double[::1] y2,
        double[::1] y3,
        double[::1] Field_p,
        const double[::1] adv_factor,
        const long[::1] bc,
        ):

    cdef Py_ssize_t m = Field_p.shape[0]-1
    cdef Py_ssize_t idx_x, idx_y, start = 1
    cdef double gamma = 0.

    gamma = .5
    RK_step(Field_p, k1, y1, adv_factor, bc, gamma)
    gamma = .5
    RK_step(y1, k2, y2, adv_factor, bc, gamma)
    gamma = 1
    RK_step(y2, k3, y3, adv_factor, bc, gamma)
    time_step(y3, k4, adv_factor)

    # 1/3(k2+k3)->k3
    op_axpby(k2, 1./3., k3, 1./3.)
    # 1/6(k1+k4)->k4
    op_axpby(k1, 1. / 6., k4, 1. / 6.)
    # k1+k4->k4
    op_axpby(k1, 1. , k4, 1.)
    # k4+field_p ->field_p
    op_axpby(k4, 1.  , Field_p, 1.)
    # for idx_x in range(start, m):
    #             Field_p[idx_x] = Field_p[idx_x] + \
    #                                 (1./6.)*(k1[idx_x]+2.*(k2[idx_x]+k3[idx_x])+k4[idx_x])
    boundary(Field_p, bc)


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void RK2(
    double[::1] k1,
        double[::1] k2,
        double[::1] y1,
        double[::1] Field_p,
        const double[::1] adv_factor,
        const long[::1] bc,):

    cdef Py_ssize_t m = Field_p.shape[0]-1
    cdef Py_ssize_t idx_x, idx_y, start = 1

    # with nogil, parallel(num_threads=4):
    #     for idx_x in prange(start, m):
    #             k1[idx_x] = 0
    #             k2[idx_x] = 0
    #             y1[idx_x] = 0
    #gamma = .5
    RK_step(Field_p, k1, y1, adv_factor, bc, .5)
    #gamma = 1
    time_step(y1, k2, adv_factor)

    op_axpby(k2,1,Field_p,1)
    boundary(Field_p, bc)
