#!python
#cython: language_level=3

import numpy as np
import cython
cimport numpy as np
cimport cython
from cython.parallel import prange, parallel
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free
from cython.view cimport array as cvarray

DTYPE = np.double
ctypedef np.double_t DTYPE_t

MAXITS = 100000
EPS = 1e-1

cdef class linearmatrix(object):
    cdef int size
    cdef double[:] tridiag_a
    cdef double[:]  tridiag_b
    cdef double[:]  tridiag_c
    cdef double[:]  tridiag_gam
    cdef double[:]  tridiag_bet
    cdef double[:, :] matrix
    cdef bint isinit
    cdef tuple shape

    def __cinit__(self, int size):
        self.size = size
        self.shape = (size, size)
        self.init_tridiag()

    def init_tridiag(self):
        self.isinit = True
        self.tridiag_b = np.zeros(self.size, dtype=DTYPE)
        self.tridiag_a = np.zeros(self.size-1, dtype=DTYPE)
        self.tridiag_c = np.zeros(self.size-1, dtype=DTYPE)
        self.tridiag_gam = np.zeros(self.size, dtype=DTYPE)
        self.tridiag_bet = np.zeros(self.size, dtype=DTYPE)
        self.matrix = np.zeros(self.shape, dtype = DTYPE)

    def initInvMat1D(self):
        """
        initialisation des donnees pour l'inversion d'une matrice tridiagonale

        :return: None
        """
        cdef int k=0
        cdef int iSizeX = self.tridiag_b.shape[0]-1
        self.tridiag_bet[1]=self.tridiag_b[1]
        for k in range(2, iSizeX-1):
            self.tridiag_gam[k] = self.tridiag_c[k-1]/self.tridiag_bet[k-1]
            self.tridiag_bet[k] = self.tridiag_b[k]-self.tridiag_a[k]*self.tridiag_gam[k]

        for k in range(1, iSizeX-1):
            self.tridiag_bet[k] = 1./self.tridiag_bet[k]

    # routine d'inversion d'une matrice tridiagonale
    def solve(self, const double[::1] rhs,
                 double[::1] inv):
        """
        routine d'inversion d'une matrice tridiagonale : A * inv = rhs
        :param rhs: vector
        :param inv: vector
        :return: No return
        """
        cdef long iSizeX = rhs.shape[0]
        cdef long k

        inv[1]=rhs[1]*self.tridiag_bet[1]
        for k in range(2, iSizeX-1):
            inv[k] = (rhs[k]-self.tridiag_a[k]*inv[k-1])*self.tridiag_bet[k]
        for k in range(iSizeX-2, 0, -1):
            inv[k] -= self.tridiag_gam[k+1]*inv[k+1]


    def add_diffusion(self, const double cdx):
        if  self.isinit is False:
            self.init_tridiag()
        for k in range(0, self.size-1):
            self.tridiag_b[k] += -2.*cdx
            self.tridiag_a[k] += cdx
            self.tridiag_c[k] += cdx
        self.tridiag_b[self.size-1] += -2.*cdx

    def add_advection(self, const double[::1] vdx):
        if self.isinit is False:
            self.init_tridiag()
        for k in range(0, self.size-1):
            self.tridiag_b[k] -= vdx[1]
            self.tridiag_a[k] -= vdx[0]
            self.tridiag_c[k] -= vdx[2]
        self.tridiag_b[self.size-1] -= vdx[1]

    def add_diag_constant(self, const double C):
        for k in range(0, self.size):
            self.tridiag_b[k] += C

    def as_dense_matrix(self):
        self.matrix = np.diag(self.tridiag_b) + np.diag(self.tridiag_a, -1) \
                      + np.diag(self.tridiag_c, 1)

    def solve(self, x, b):
        pass
#
#
# cdef class LinearMatrix2D(object):
#     cdef long size
#     cdef int size_x
#     cdef int size_y
#     cdef double[:] vec_a
#     cdef double[:] vec_b
#     cdef double[:] vec_c
#     cdef double[:] vec_d
#     cdef double[:] vec_e
#     cdef double rsor
#
#     cdef bint isinit
#     cdef tuple shape
#
#     def __cinit__(self, int sizeY, int sizeX):
#         self.size = sizeX*sizeY
#         self.size_x = sizeX
#         self.size_y = sizeY
#
#
#     def __SorRadius(self, double dx, double dy):
#         cdef double  dstep=dx/dy
#         cdef double  dstep2=dstep*dstep
#
#         self.rsor=(np.cos(np.pi/self.size_x)+dstep2*np.cos(2*np.pi/self.size_y))/(1.+dstep2)
#
#     @cython.boundscheck(False)
#     def init(self, double dFactor, double dx, double dy):
#         cdef int x,y,pos_ij;
#         cdef double dFx = dFactor/(dx*dx)
#         cdef double dFy = dFactor/(dy*dy)
#         self.isinit = True
#         self.vec_a = np.zeros(self.size, dtype=DTYPE)
#         self.vec_b = np.zeros(self.size, dtype=DTYPE)
#         self.vec_c = np.zeros(self.size, dtype=DTYPE)
#         self.vec_d = np.zeros(self.size, dtype=DTYPE)
#         self.vec_e = np.zeros(self.size, dtype=DTYPE)
#
#         self.__SorRadius(dx, dy)
#         with nogil, parallel(num_threads=4):
#             for y in prange(0, self.size_y):
#                 for x in range(0, self.size_x):
#                     pos_ij=x+self.size_x*y
#                     self.vec_a[pos_ij]=dFy
#                     self.vec_b[pos_ij]=dFy
#                     self.vec_c[pos_ij]=dFx
#                     self.vec_d[pos_ij]=dFx
#                     self.vec_e[pos_ij]=1.-2.*(dFx+dFy)
#
#
#     @cython.boundscheck(False)
#     @cython.wraparound(False)
#     @cython.nonecheck(False)
#     @cython.cdivision(True)
#     def solve(self, np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] RHS,
#                   np.ndarray[DTYPE_t, ndim=2, negative_indices=False, mode='c'] inv,
#                   ):
#         cdef int ipass,j,jsw,k,lsw,n;
#         cdef double anorm=0.,anormf=0.0,omega=1.0,resid=0.
#         cdef double rjac=self.rsor
#         cdef long posxy, posxyp1, posxym1, posxp1y, posxm1y
#
#         for j in range(1, self.size_y-1):
#             for k in range(1, self.size_x - 1):
#                 anormf += np.abs(RHS[j, k])
#
#         #omega = 1.5
#         # print(omega)
#         for n in range(1, MAXITS+1):
#             anorm=0.0
#             jsw=1
#             for ipass in range(1,3):
#                 lsw=jsw
#                 for j in range(1, self.size_y-1):
#                     for k in range(lsw, self.size_x - 1, 2):
#                         posxy = k + j*(self.size_x-1)
#                         # posxyp1 = jj + (j+1)*self.size_x
#                         # posxym1 = jj + (j-1)*self.size_x
#                         # posxp1y = jj + 1 + (j)*self.size_x
#                         # posxm1y = jj - 1 + (j)*self.size_x
#                         resid=self.vec_a[posxy]*inv[j+1, k] \
#                         +self.vec_b[posxy]*inv[j-1, k] \
#                         +self.vec_c[posxy]*inv[j, k+1] \
#                         +self.vec_d[posxy]*inv[j, k-1] \
#                         +self.vec_e[posxy]*inv[j, k] \
#                         -RHS[j, k]
#                         anorm += np.abs(resid)
#
#
#                         # resid=self.vec_a[posxy]*inv[j+1, k] \
#                         # +self.vec_b[posxy]*inv[j-1, k] \
#                         # +self.vec_c[posxy]*inv[j, k+1] \
#                         # +self.vec_d[posxy]*inv[j, k-1] \
#                         # +RHS[j, k]
#                         # inv[j,k] = (1-omega)*inv[j,k]+omega*resid/self.vec_e[posxy]
#                         inv[j, k] -= omega*resid/self.vec_e[posxy]
#                     lsw=3-lsw
#                 jsw=3-jsw
#                 if (n == 1) and (ipass == 1):
#                     omega = 1.0/(1.0-0.5*rjac*rjac)
#                 else:
#                     omega = 1.0/(1.0-0.25*rjac*rjac)
#             if (anorm < EPS*anormf):
#                 return
#
#         raise ValueError('max iterations exceeded')
#
#
#     def add_diffusion(self, double C, double dx):
#         pass
#
#     def add_advection(self, double V, double dx):
#         pass
#
#     def add_diag_constant(self, double C):
#         pass
#
#
# def laplacian2D(N, dx):
#     diag_block = np.diag(4*np.ones(N)) + \
#          np.diag(-1*np.ones(N-1), 1) + \
#          np.diag(-1*np.ones(N-1), -1)
#     next_j_block = np.diag(np.ones(N))
#     prev_j_block = np.diag(np.ones(N))
#     D2 = np.zeros((N*N, N*N))
#     for block_id in range(0,N):
#         D2[block_id*N:block_id*N+N, block_id*N:block_id*N+N ] = diag_block[:,:]
#     for block_id in range(0,N-1):
#         bp1 = block_id + 1
#         D2[block_id*N:block_id*N+N, N*bp1:(bp1+1)*N ] = next_j_block[:,:]
#
#     for block_id in range(1,N):
#         bm1 = block_id - 1
#         D2[block_id*N:block_id*N+N, N*bm1:(bm1+1)*N ] = next_j_block[:,:]
#
#     return D2


# routine d'inversion d'une matrice tridiagonale
def invMat1D(np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] rhs,
             np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] inv,

             np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c']  a,
             np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] pGam,
             np.ndarray[DTYPE_t, ndim=1, negative_indices=False, mode='c'] pBet):
    """
    routine d'inversion d'une matrice tridiagonale : A * inv = rhs
    :param rhs: vector
    :param inv: vector
    :param a:
    :param pGam:
    :param pBet:
    :return: No return
    """
    cdef int iSizeX = rhs.shape[0]
    cdef int k

    inv[1]=rhs[1]*pBet[1]
    for k in range(2, iSizeX-1):
        inv[k] = (rhs[k]-a[k]*inv[k-1])*pBet[k]
    for k in range(iSizeX-2, 0, -1):
        inv[k] -= pGam[k+1]*inv[k+1]


cpdef void initInvMat1D(const double[::1] a,
                 const double[::1] b,
                 const double[::1] c,
                 double[::1] pGam,
                 double[::1] pBet):
    """
    initialisation des donnees pour l'inversion d'une matrice tridiagonale
    :param a: diagonale inferieure
    :param b: diagonale principale
    :param c: diagonale superieure
    :param pGam:
    :param pBet:
    :return: None
    """
    cdef int k=0
    cdef int iSizeX = b.shape[0]-1
    pBet[1]=b[1]
    for k in range(2, iSizeX-1):
        pGam[k] = c[k-1]/pBet[k-1]
        pBet[k] = b[k]-a[k]*pGam[k]

    for k in range(1, iSizeX-1):
        pBet[k] = 1./pBet[k]


def gausseidel(A, b, x, tol):
    cdef int m,n,i,j, num_iterations
    cdef double error

    m = A.shape[0]
    n = A.shape[1]

    prev_x = np.zeros(np.shape(b))
    diff = np.zeros(np.shape(b))

    num_iterations = 0
    error = tol + 1
    while error > tol:
        for i in range(0, m):
            prev_x[i] = x[i]
            sum = b[i]
            for j in range(0, m):
                if i != j:
                    sum = sum - A[i][j]*x[j][0]
            sum = sum / A[i][i]
            x[i][0] = sum
        num_iterations += 1

        diff = np.subtract(x, prev_x)
        error = np.linalg.norm(diff) / np.linalg.norm(x)


def SOR(A, b, x, w, tol):

    cdef int m = A.shape[0]
    cdef int n = A.shape[1]

    prev_x = np.array(x)
    diff = np.zeros(np.shape(b))

    cdef int num_iterations = 0
    cdef double error = tol + 1.0
    cdef int max_iterations = 200
    while error > tol and num_iterations < max_iterations:
        num_iterations += 1
        for i in range(0, m):
            prev_x[i] = x[i]
            sum = b[i]
            old_x = x[i][0]
            for j in range(0, m):
                if i != j:
                    sum = sum - A[i][j]*x[j][0]
            sum /= A[i][i]
            x[i][0] = sum
            x[i][0] = x[i][0] * w + (1.0 - w)*old_x
        # relative error
        error = np.linalg.norm(np.subtract(x, prev_x)) / np.linalg.norm(x)


if __name__ =='__main__':
    pass