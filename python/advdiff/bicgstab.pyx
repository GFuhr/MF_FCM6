#!python
#cython: language_level=3
#cython: boundscheck=False
#cython: wraparound=False
#cython: initializedcheck=False
#cython: cdivision=True
#cython: nonecheck=False
import cython
import numpy as np
cimport numpy as np
from cython.parallel import prange, parallel
from libc.math cimport fabs

@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
cdef void triband_dot(const double[:,::1] band_matrix,
                      const double[::1] x, double[::1] vec,
                      const int matrix_size) nogil:
    cdef Py_ssize_t idx
    cdef double xm1
    cdef double xi
    cdef double xp1
    cdef double bmm1, bm, bmp1


    vec[0] = band_matrix[0,2]*x[0]+band_matrix[0,3]*x[1]\
             +band_matrix[0,4]*x[matrix_size-1]

    bmm1 =  band_matrix[1,1]
    bm=  band_matrix[1,2]
    bmp1 =  band_matrix[1,3]
    for idx in prange(1, matrix_size-1):
        vec[idx] = bmm1*x[idx-1]+bm*x[idx]+bmp1*x[idx+1]

    idx = matrix_size-1
    vec[idx]= band_matrix[2,1]*x[idx-1]+band_matrix[2,2]*x[idx]+band_matrix[2,0]*x[0]

@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
cdef double single_vec_dot(const double[::1] x, const Py_ssize_t vec_size) nogil:
    cdef double dot = 0.
    cdef Py_ssize_t idx = 0
    cdef int idx_end = vec_size & ~3

    for idx in prange(0, idx_end, 4):
        dot += x[idx]*x[idx] + x[idx+1]*x[idx+1] + x[idx+2]*x[idx+2] + x[idx+3]*x[idx+3]
    for idx in prange(idx_end, vec_size):
        dot += x[idx]*x[idx]

    return dot

@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
cdef double vec_dot(const double[::1] x, const double[::1] y,
                    const Py_ssize_t vec_size) nogil:
    cdef double dot = 0.
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = vec_size & ~3

    for idx in prange(0, idx_end, 4):
        dot += x[idx]*y[idx] + x[idx+1]*y[idx+1] + x[idx+2]*y[idx+2] + x[idx+3]*y[idx+3]
    for idx in prange(idx_end, vec_size):
        dot += x[idx]*y[idx]
    return dot

@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
cdef void vec_xmy(double[::1] res, const double[::1] x, const double[::1] y, const Py_ssize_t vec_size) nogil:
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = vec_size & ~3

    for idx in prange(0, idx_end, 4):
        res[idx] = x[idx]-y[idx]
        res[idx+1] = x[idx+1]-y[idx+1]
        res[idx+2] = x[idx+2]-y[idx+2]
        res[idx+3] = x[idx+3]-y[idx+3]

    for idx in prange(idx_end, vec_size):
        res[idx] = x[idx]-y[idx]



@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
cdef void vec_xpby(double[::1] res, const double[::1] x, const double[::1] y, const double factor, const Py_ssize_t vec_size) nogil:
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = vec_size & ~3

    for idx in prange(0, idx_end, 4):
        res[idx] = x[idx]+factor*y[idx]
        res[idx+1] = x[idx+1]+factor*y[idx+1]
        res[idx+2] = x[idx+2]+factor*y[idx+2]
        res[idx+3] = x[idx+3]+factor*y[idx+3]

    for idx in prange(idx_end, vec_size):
        res[idx] = x[idx]+factor*y[idx]

@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
cdef void dcopy(const Py_ssize_t vec_size, const double[::1] src, double[::1] dest) nogil:
    cdef Py_ssize_t idx = 0
    cdef Py_ssize_t idx_end = vec_size & ~3

    for idx in prange(0, idx_end, 4):
        dest[idx] = src[idx]
        dest[idx+1] = src[idx+1]
        dest[idx+2] = src[idx+2]
        dest[idx+3] = src[idx+3]

    for idx in prange(idx_end, vec_size):
        dest[idx] = src[idx]


@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
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


@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
@cython.initializedcheck(False)
@cython.nonecheck(False)
@cython.cdivision(True)
cdef void bicgstab_c(const double[:,::1] mat, const double[::1] b, double[::1] x,
    double[::1] dummy,
    double[::1]  r_i,
    double[::1]  s_i,
    double[::1]  v_i,
    double[::1]  p_i,
    double[::1]  t_i,
    double[::1]  r_hat_0,
    const double tol, const int max_iter,
                    ) nogil:
    """
    Solves the linear system Ax = b using the BiCGSTAB algorithm.

    Parameters:
    A (array): the matrix of the linear system
    b (array): the right-hand side vector of the linear system
    x0 (array): the initial guess for the solution
    tol (float, optional): the tolerance for the residual norm. Default is 1e-8.
    max_iter (int, optional): the maximum number of iterations. Default is 1000.

    Returns:
    x (array): the solution of the linear system
    """
    cdef Py_ssize_t i,j
    cdef Py_ssize_t vec_size = b.shape[0]
    cdef double rho_i, beta, alpha, omega_i, res, rho_im1, rho0
    # cdef np.ndarray[np.double_t, ndim=1] dummy = np.zeros_like(x)

    triband_dot(mat, x, dummy, vec_size)
    vec_xmy(r_i, b, dummy, vec_size)

    rho_im1 = rho_0 =  omega_i = 1.0
    alpha = 0
        #v_i = p_i = t_i = np.zeros_like(b)
        #x = x0
    dcopy(vec_size, r_i, r_hat_0)

    for i in range(max_iter):
        #rho_i = vec_dot(r_hat_0, r_i, vec_size)
        rho_i = vec_dot(r_hat_0, r_i, vec_size)

        beta = (rho_i / rho_im1) * (alpha / omega_i)
        # for j in range(vec_size):
        #     p_i[j] = r_i[j] + beta * (p_i[j] - omega_i * v_i[j])
        d_axpbypcz(vec_size, r_i, 1,v_i, -beta*omega_i, p_i, beta)

        # v_i =
        triband_dot(mat, p_i, v_i, vec_size)
        alpha = rho_i / vec_dot(r_hat_0, v_i, vec_size)
        #s_i = r_i - alpha * v_i
        vec_xpby(s_i, r_i, v_i, -alpha, vec_size)

        # t_i
        triband_dot(mat, s_i, t_i, vec_size)
        omega_i = vec_dot(t_i, s_i, vec_size) / single_vec_dot(t_i, vec_size)

        # for j in range(vec_size):
        #     x[j] = x[j] + alpha * p_i[j] + omega_i * s_i[j]
        d_axpbypcz(vec_size,p_i, alpha, s_i, omega_i, x, 1)

        #r_i = s_i - omega_i * t_i
        vec_xpby(r_i, s_i, t_i, -omega_i, vec_size)

        rho_im1 = rho_i
        res = single_vec_dot(r_i, vec_size)
        if fabs(res) < tol:
            break


# definition of a matrix representation used for implicit schemes
#
cdef class MatrixSolver(object):
    cdef Py_ssize_t size
    cdef double[::1] dummy
    cdef double[::1]  r_i
    cdef double[::1]  s_i
    cdef double[::1]  v_i
    cdef double[::1]  p_i
    cdef double[::1]  t_i
    cdef double[::1]  r_hat_0
    cdef double[:,::1]  operator
    cdef bint isinit
    cdef double tol
    cdef int iter

    def __cinit__(self, const Py_ssize_t size):
        self.size = size-2

        self.init_variables()
        self.tol = 1e-7
        self.iter = 1000
        self.isinit = False

    cdef init_variables(self):
        self.isinit = True
        self.dummy = np.zeros(self.size, dtype=np.double)
        self.r_i = np.zeros(self.size, dtype=np.double)
        self.s_i = np.zeros(self.size, dtype=np.double)
        self.v_i = np.zeros(self.size, dtype=np.double)
        self.p_i = np.zeros(self.size, dtype=np.double)
        self.t_i = np.zeros(self.size, dtype=np.double)
        self.r_hat_0 = np.zeros(self.size, dtype=np.double)
        self.operator = np.zeros(shape=(3,5), dtype=np.double)

    cpdef double[:,::1] create_matrix(self, const double[::1] vector_op,
                                      const long[::1] bc):
        if not self.isinit:
            self.init_variables()

        self.operator[0,1:4] = vector_op[0:3]
        # to take into account dirichlet or neumann bondary conditions on the left
        self.operator[0,3] += bc[0]*vector_op[0]
        self.operator[0,1] = 0
        # case where box is periodic on the left side
        self.operator[0,4] = bc[1]*vector_op[0]

        self.operator[1,1:4] = vector_op[0:3]

        self.operator[2,1:4] = vector_op[0:3]
        # to take into account dirichlet or neumann bondary conditions on the left
        self.operator[2,1] += bc[2]*vector_op[2]
        self.operator[2,3] = 0
        # case where box is periodic on the right side
        self.operator[2,0] = bc[3]*vector_op[2]
        return self.operator

    # routine d'inversion d'une matrice tridiagonale
    cpdef void solve(self,  const double[::1] rhs,
                 double[::1] x):
        """
        routine d'inversion d'une matrice tridiagonale : A * x = rhs
        :param rhs: vector
        :param x: vector

        :return: No return
        """
        bicgstab_c(self.operator, rhs[1:rhs.shape[0]-1], x[1:x.shape[0]-1],  self.dummy,
                   self.r_i, self.s_i, self.v_i, self.p_i, self.t_i, self.r_hat_0, self.tol, self.iter)

