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

cdef class linearmatrix2D(object):
    cdef int size
    cdef int size_x
    cdef int size_y
    cdef double[:] vec_a
    cdef double[:] vec_b
    cdef double[:] vec_c
    cdef double[:] vec_d
    cdef double[:] vec_e
    cdef double rsor

    cdef bint isinit
    cdef tuple shape