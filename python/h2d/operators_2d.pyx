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
cdef diffusion(double dx, double dy, double C, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] u):
    cdef int Nx = u.shape[1]-1
    cdef int Ny = u.shape[0]-1
    cdef int iy, ix
    cdef double v_c
    cdef double hx = C/(dx*dx)
    cdef double hy = C/(dy*dy)
    with nogil, parallel(num_threads=4):
        for iy in prange(1, Ny):
            for ix in range(1, Nx):
                v_c = 2*v[iy, ix]
                u[iy, ix] += hx*(v[iy, ix+1] + v[iy, ix-1] - v_c) + \
                          hy*(v[iy+1, ix] + v[iy-1, ix] - v_c)




@cython.boundscheck(False)
@cython.wraparound(False)
cdef advection(double dx, double dy, double V, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] u):
    cdef int Nx = u.shape[1]-1
    cdef int Ny = u.shape[0]-1
    cdef int iy, ix
    cdef double v_c
    cdef double hx = (V/dx)
    cdef double hy = (V/dy)
    with nogil, parallel(num_threads=4):
        for iy in prange(1, Ny):
            for ix in range(1, Nx):
                v_c = v[iy, ix]
                u[iy, ix] += hx*(v_c - v[iy, ix-1]) + hy*(v_c - v[iy-1, ix])


@cython.boundscheck(False)
@cython.wraparound(False)
cdef boundary(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] u):
    cdef int Nx = u.shape[1]-1
    cdef int Ny = u.shape[0]-1
    cdef int iy , ix

    # y boundary
    for ix in range(1, Nx-1):
        # u[0, ix] = -u[2, ix]
        # u[Ny, ix] = -u[Ny-2, ix]
        u[0, ix] = u[Ny-1, ix]
        u[Ny, ix] = u[1, ix]
        #u[0, ix] = 2.

    # x boundary
    for iy in range(0, Ny):
        u[iy, 0] = -u[iy, 2]
        u[iy, Nx] = -u[iy, Nx-2]




@cython.boundscheck(False)
@cython.wraparound(False)
def time_step(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] un, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] rhs, **kwargs):

    cdef int Ny = un.shape[0]
    cdef int Nx = un.shape[1]
    cdef int idx_x, idx_y
    cdef double C = kwargs.get('C', .01)
    cdef double V = kwargs.get('V', .01)
    cdef double dx = kwargs.get('dx')
    cdef double dy = kwargs.get('dy')

    for idx_y in range(0, Ny):
        for idx_x in range(0, Nx):
            rhs[idx_y, idx_x] = 0.

    diffusion(dx, dy, C, un, rhs)
    advection(dx, dy, V, un, rhs)


@cython.boundscheck(False)
@cython.wraparound(False)
def eule(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] rhs, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] un, **kwargs):

    cdef int m = un.shape[0]-1
    cdef int n = un.shape[1]-1
    cdef int idx_x, idx_y
    cdef double dt = kwargs.get('dt')

    time_step(un, rhs, **kwargs)
    with nogil, parallel(num_threads=4):
        for idx_y in prange(1, m):
            for idx_x in range(1, n):
                un[idx_y, idx_x] += dt*rhs[idx_y, idx_x]
    boundary(un)

@cython.boundscheck(False)
@cython.wraparound(False)
def euli(matrix.linearmatrix2D matA, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] pp1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_p, **kwargs):
    cdef int m = Field_p.shape[0]-1
    cdef int n = Field_p.shape[1]-1
    cdef int idx_x, idx_y

    with nogil, parallel(num_threads=4):
        for idx_y in prange(1, m):
            for idx_x in range(1, n):
                pp1[idx_y, idx_x] = Field_p[idx_y, idx_x]
    matA.solve(Field_p, pp1)

    with nogil, parallel(num_threads=4):
        for idx_y in prange(1, m):
            for idx_x in range(1, n):
                Field_p[idx_y, idx_x] = pp1[idx_y, idx_x]
    boundary(Field_p)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK_step(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] un, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] ki, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] yi, **kwargs):

    cdef int m = un.shape[0]-1
    cdef int n = un.shape[1]-1
    cdef int idx_x, idx_y
    cdef double gamma = kwargs.get('gamma', .5)

    time_step(un, ki, **kwargs)
    with nogil, parallel(num_threads=4):
        for idx_y in prange(1, m):
            for idx_x in range(1, n):
                yi[idx_y, idx_x] = un[idx_y, idx_x] + gamma*ki[idx_y, idx_x]
    boundary(yi)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK4(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k2, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k3, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k4, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] y1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] y2, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] y3, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_p,
             **kwargs):

    cdef int m = Field_p.shape[0]
    cdef int n = Field_p.shape[1]
    cdef int idx_x, idx_y, start = 1
    cdef double dt = kwargs.get('dt')

    with nogil, parallel(num_threads=4):
        for idx_y in prange(0, m):
            for idx_x in range(0, n):
                k1[idx_y, idx_x] = 0
                k2[idx_y, idx_x] = 0
                k3[idx_y, idx_x] = 0
                k4[idx_y, idx_x] = 0
                y1[idx_y, idx_x] = 0
                y2[idx_y, idx_x] = 0
                y3[idx_y, idx_x] = 0

    # y1 = p + dt/2*k1
    kwargs['gamma'] = .5*dt
    RK_step(Field_p, k1, y1, **kwargs)

    # y2 = p + dt/2*k1
    kwargs['gamma'] = .5*dt
    RK_step(y1, k2, y2, **kwargs)

    # y3 = p + dt*k2
    kwargs['gamma'] = dt
    RK_step(y2, k3, y3, **kwargs)

    # k4 = rhs(y3)
    time_step(y3, k4, **kwargs)

    with nogil, parallel(num_threads=4):
        for idx_y in prange(start, m):
            for idx_x in range(start, n):
                Field_p[idx_y, idx_x] = Field_p[idx_y, idx_x]+\
                                    (dt/6.)*(k1[idx_y, idx_x]+2.*(k2[idx_y, idx_x]+k3[idx_y, idx_x])+k4[idx_y, idx_x])
    boundary(Field_p)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK2(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k2, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] y1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_p,
             **kwargs):

    cdef int m = Field_p.shape[0]
    cdef int n = Field_p.shape[1]
    cdef int idx_x, idx_y, start = 1
    cdef double dt = kwargs.get('dt')

    with nogil, parallel(num_threads=4):
        for idx_y in prange(0, m):
            for idx_x in range(0, n):
                k1[idx_y, idx_x] = 0
                k2[idx_y, idx_x] = 0
                y1[idx_y, idx_x] = 0

    # p+1 = p + dt/2*k1
    kwargs['gamma'] = .5*dt
    RK_step(Field_p, k1, y1, **kwargs)

    # k4 = rhs(y3)
    time_step(y1, k2, **kwargs)

    with nogil, parallel(num_threads=4):
        for idx_y in prange(start, m):
            for idx_x in range(start, n):
                Field_p[idx_y, idx_x] = Field_p[idx_y, idx_x] + (dt)*k1[idx_y, idx_x]
    boundary(Field_p)