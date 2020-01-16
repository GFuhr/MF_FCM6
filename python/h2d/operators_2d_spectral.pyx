#!python
#cython: language_level=3

import numpy as np
import cython
cimport numpy as np
cimport cython
from cython.parallel import prange, parallel

DTYPE = np.complex128
ctypedef np.complex128_t DTYPE_t
ctypedef np.double_t DTYPE_REAL_t

@cython.boundscheck(False)
@cython.wraparound(False)
cdef diffusion_spectral(double dx, double ky, double C, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] u):
    cdef int Ny = u.shape[0]
    cdef int Nx = u.shape[1]-1
    cdef int x,m
    cdef double complex v_c
    cdef double hx = C/(dx*dx)
    cdef double hy = C*ky*ky

    with nogil, parallel(num_threads=4):
        for m in prange(0, Ny):
            for x in range(1, Nx):
                v_c = v[m,x]
                u[m,x] = u[m,x] + hx*(v[m,x+1] + v[m,x-1] - 2*v_c)  - m*m*hy*v_c


@cython.boundscheck(False)
@cython.wraparound(False)
cdef advection_spectral(double dx, double ky, double V, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] v, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] u):
    cdef int Ny = u.shape[0]-1
    cdef int Nx = u.shape[1]
    cdef int m,x
    cdef double complex v_c
    cdef double hx = (V/dx)
    cdef double complex hy = V*ky*1j

    with nogil, parallel(num_threads=4):
        for m in prange(0, Ny):
            for x in range(1, Nx):
                v_c = v[m,x]
                u[m,x] = u[m,x] + hx*( v_c - v[m,x-1]) + m*hy*v_c


@cython.boundscheck(False)
@cython.wraparound(False)
cdef boundary(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] u):
    cdef int Nx = u.shape[1]-1
    cdef int Ny = u.shape[0]
    cdef int m
    for m in range(0, Ny):
        u[m, 0] = u[m, 2]
        u[m, Nx] = u[m, Nx-2]


@cython.boundscheck(False)
@cython.wraparound(False)
def time_step(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_p, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] pp1, **kwargs):
    cdef int Nx = Field_p.shape[1]-1
    cdef int Nm = Field_p.shape[0]
    cdef int idx_x, m
    cdef double C = kwargs.get('C')
    cdef double V = kwargs.get('V')
    cdef double dx = kwargs.get('dx')
    cdef double ky = kwargs.get('ky')

    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in prange(1, Nx):
                pp1[m, idx_x] = 0.

    diffusion_spectral(dx, ky, C, Field_p, pp1 )
    advection_spectral(dx, ky, V, Field_p, pp1)


@cython.boundscheck(False)
@cython.wraparound(False)
def eule(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] rhs, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] un, **kwargs):

    cdef int Nm = un.shape[0]
    cdef int n = un.shape[1]-1
    cdef int idx_x, m
    cdef double dt = kwargs.get('dt')

    time_step(un, rhs, **kwargs)
    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in range(1, n):
                un[m, idx_x] = un[m, idx_x] + dt*rhs[m, idx_x]
    boundary(un)




@cython.boundscheck(False)
@cython.wraparound(False)
def RK4_step(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] un, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] ki, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] yi, **kwargs):

    cdef int Nm = un.shape[0]
    cdef int Nx = un.shape[1]-1
    cdef int idx_x, m
    cdef double gamma = kwargs.get('gamma', .5)

    time_step(un, ki, **kwargs)
    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in range(1, Nx):
                yi[m, idx_x] = un[m, idx_x] + gamma*ki[m, idx_x]
    boundary(yi)


@cython.boundscheck(False)
@cython.wraparound(False)
def RK2(
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] k2, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] y1, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_p,
             **kwargs):

    cdef int Nm = Field_p.shape[0]
    cdef int n = Field_p.shape[1]
    cdef int idx_x, m
    cdef double dt = kwargs.get('dt')

    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in range(0, n):
                k1[m, idx_x] = 0
                k2[m, idx_x] = 0
                y1[m, idx_x] = 0

    # y1 = p + dt/2*k1
    kwargs['gamma'] = .5*dt
    RK4_step(Field_p, k1, y1, **kwargs)

    # k4 = rhs(y3)
    time_step(y1, k2, **kwargs)

    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in range(1, n):
                Field_p[m, idx_x] = Field_p[m, idx_x]+\
                                    (dt*(k2[m, idx_x]))
    boundary(Field_p)


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

    cdef int Nm = Field_p.shape[0]
    cdef int n = Field_p.shape[1]
    cdef int idx_x, m
    cdef double dt = kwargs.get('dt')

    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in range(0, n):
                k1[m, idx_x] = 0
                k2[m, idx_x] = 0
                k3[m, idx_x] = 0
                k4[m, idx_x] = 0
                y1[m, idx_x] = 0
                y2[m, idx_x] = 0
                y3[m, idx_x] = 0

    # y1 = p + dt/2*k1
    kwargs['gamma'] = .5*dt
    RK4_step(Field_p, k1, y1, **kwargs)

    # y2 = p + dt/2*k1
    kwargs['gamma'] = .5*dt
    RK4_step(y1, k2, y2, **kwargs)

    # y3 = p + dt*k2
    kwargs['gamma'] = dt
    RK4_step(y2, k3, y3, **kwargs)

    # k4 = rhs(y3)
    time_step(y3, k4, **kwargs)

    with nogil, parallel(num_threads=4):
        for m in prange(0, Nm):
            for idx_x in range(1, n):
                Field_p[m, idx_x] = Field_p[m, idx_x]+\
                                    (dt/6.)*(k1[m, idx_x]+2.*(k2[m, idx_x]+k3[m, idx_x])+k4[m, idx_x])
    boundary(Field_p)


@cython.boundscheck(False)
@cython.wraparound(False)
def compute_real_field(double dx, double dy,\
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_C, \
    np.ndarray[DTYPE_REAL_t, ndim=2, negative_indices=False, mode='c'] Field_R):

    cdef int Nx = Field_C.shape[1]
    cdef int Nm = Field_C.shape[0]
    cdef int idx_x, idx_y, start = 1
    cdef np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] tmp_C
    cdef np.ndarray[DTYPE_REAL_t, ndim=1, negative_indices=False, mode='c'] tmp_R

    for idx_x in range(0, Nx):
        tmp_C = np.array(Field_C[:, idx_x])

        tmp_R = np.fft.irfft(tmp_C)
        Field_R[:, idx_x] = np.array(tmp_R)


@cython.boundscheck(False)
@cython.wraparound(False)
def compute_spectral_field(double dx, double dy,\
    np.ndarray[DTYPE_REAL_t, ndim=2, negative_indices=False, mode='c'] Field_R, \
    np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] Field_C):

    cdef int Nx = Field_C.shape[1]
    cdef int Nm = Field_C.shape[0]
    cdef int idx_x, idx_y
    cdef np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] tmp_C
    cdef np.ndarray[DTYPE_REAL_t, ndim=1, negative_indices=False, mode='c'] tmp_R

    for idx_x in range(0, Nx):
        tmp_R = np.array(Field_R[:, idx_x])
        tmp_C = np.fft.rfft(tmp_R)
        Field_C[:, idx_x] = np.array(tmp_C)
