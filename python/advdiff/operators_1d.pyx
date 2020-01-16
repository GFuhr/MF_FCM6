#!python
#cython: language_level=3
import numpy as np
import cython
cimport numpy as np
cimport cython
from cython.parallel import prange, parallel
import matrix
cimport matrix

DTYPE = np.double
ctypedef np.double_t DTYPE_t

@cython.boundscheck(False)
@cython.wraparound(False)
cdef diffusion(double dx, double C, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] u):
    cdef int m = u.shape[0]-1
    cdef int i
    cdef double hx = C/dx**2

    with nogil, parallel(num_threads=8):
        for i in prange(1, m):
            u[i] += hx*(v[i+1] + v[i-1] - 2*v[i])


@cython.boundscheck(False)
@cython.wraparound(False)
cdef diffusion_coef(double dx, double Lx, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] chi, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] dchi, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] u):
    cdef int m = u.shape[0]-1
    cdef int i
    cdef double hx = 1/dx**2
    cdef double lxm2 = 1./(Lx*Lx)
    with nogil, parallel(num_threads=8):
        for i in prange(1, m):
            u[i] += lxm2*(chi[i]*hx*(v[i+1] + v[i-1] - 2*v[i]) + dchi[i]*(v[i+1] + v[i-1])*.5/dx)



@cython.boundscheck(False)
@cython.wraparound(False)
cdef advection(double dx, double V, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] u):
    cdef int m = u.shape[0]-1
    cdef int i, start = 1
    cdef double hx = (V/dx)
    with nogil, parallel(num_threads=4):
        for i in prange(start, m):
                u[i] += hx*(v[i]  - v[i-1])


@cython.boundscheck(False)
@cython.wraparound(False)
cdef boundary(
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] u):
    cdef int m = u.shape[0]-1
    # periodic
    # u[0] = u[m-1]
    # u[m] = u[1]
    # neumann
    # u[0] = u[2]
    # u[m] = u[m-2]
    # dirichlet
    u[0] = -u[2]
    u[m] = -u[m-2]

@cython.boundscheck(False)
@cython.wraparound(False)
def time_step(
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] pp1, **kwargs):
    cdef double dx = kwargs.get('dx')
    cdef double C = kwargs.get('C', .001)
    cdef double V = kwargs.get('V', -.001)

    diffusion(dx, C, Field_p, pp1 )
    advection(dx, V, Field_p, pp1 )


@cython.boundscheck(False)
@cython.wraparound(False)
def eule(
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] rhs, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, **kwargs):
    """
        compute explicit euler time step
        Field_p = Field_p + dt* (-V d/dx + C d2/dx2)
        params Field_p :
        params rhs : temporary array to compute RHS of equation
        params kwargs : physical parameters (C, V, dx, dt)
    """

    cdef int m = Field_p.shape[0]-1
    cdef int idx_x, start = 1

    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
            rhs[idx_x] = 0
    time_step(Field_p, rhs, **kwargs)
    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
            Field_p[idx_x] += rhs[idx_x]
    boundary(Field_p)


@cython.boundscheck(False)
@cython.wraparound(False)
def euli(matrix.linearmatrix matA, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] pp1, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, **kwargs):

    cdef int m = Field_p.shape[0]
    cdef int idx_x, start = 1
    matA.solve(Field_p, pp1)
    boundary(pp1)
    for idx_x in range(0, m):
            Field_p[idx_x] = pp1[idx_x]
    boundary(Field_p)

@cython.boundscheck(False)
@cython.wraparound(False)
def CranckN(matrix.linearmatrix matA, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] rhs, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, **kwargs):

    eule(rhs, Field_p, **kwargs)
    euli(matA, rhs, Field_p, **kwargs)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK_step(
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] ki, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] ppi, **kwargs):

    cdef int m = Field_p.shape[0]-1
    cdef int idx_x, start = 1
    cdef double gamma = kwargs.get('gamma', .5)
    time_step(Field_p, ki, **kwargs)
    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
                ppi[idx_x] = Field_p[idx_x] + gamma*ki[idx_x]
    boundary(ppi)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK4(
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] k1, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] k2, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] k3, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] k4, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] y1, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] y2, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] y3, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, \
             **kwargs):

    cdef int m = Field_p.shape[0]-1
    cdef int idx_x, idx_y, start = 1

    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
                k1[idx_x] = 0
                k2[idx_x] = 0
                k3[idx_x] = 0
                k4[idx_x] = 0
                y1[idx_x] = 0
                y2[idx_x] = 0
                y3[idx_x] = 0
    kwargs['gamma'] = .5
    RK_step(Field_p, k1, y1, **kwargs)
    kwargs['gamma'] = .5
    RK_step(y1, k2, y2, **kwargs)
    kwargs['gamma'] = 1
    RK_step(y2, k3, y3, **kwargs)
    time_step(y3, k4, **kwargs)

    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
                Field_p[idx_x] = Field_p[idx_x] + \
                                    (1./6.)*(k1[idx_x]+2.*(k2[idx_x]+k3[idx_x])+k4[idx_x])
    boundary(Field_p)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK2(
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] k1, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] k2, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] y1, \
    np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] Field_p, \
             **kwargs):

    cdef int m = Field_p.shape[0]-1
    cdef int idx_x, idx_y, start = 1

    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
                k1[idx_x] = 0
                k2[idx_x] = 0
                y1[idx_x] = 0
    kwargs['gamma'] = .5
    RK_step(Field_p, k1, y1, **kwargs)
    kwargs['gamma'] = 1
    time_step(y1, k2, **kwargs)

    with nogil, parallel(num_threads=4):
        for idx_x in prange(start, m):
                Field_p[idx_x] = Field_p[idx_x] + k2[idx_x]
    boundary(Field_p)
