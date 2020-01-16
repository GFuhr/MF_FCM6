    #!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'Guillaume Fuhr'
import sys
import os
import setuptools
import pyximport
import matplotlib
import numpy as np
import matplotlib.pyplot as plt

sys.path.append(os.path.normpath(
                                 os.path.join(os.path.realpath(__file__), '../..')
                                 )
                )
pyximport.install(setup_args={'include_dirs': np.get_include()})
matplotlib.use('Qt5Agg')

try:
    from parameters import load_params, initfield_1D
except ModuleNotFoundError:
    from advdiff.parameters import load_params, initfield_1D

from utils.timer import Timer
from utils.plotting import figformat, animated_plot_1d
try:
    import matrix
    import operators_1d as operators
except ModuleNotFoundError:
    import advdiff.matrix
    import advdiff.operators_1d as operators


# normalized figures output adapted for better reading in articles/reports
figformat().apply()


# parameters
def simulate(**kwargs):
    global_params = load_params(**kwargs)

    Nx = global_params['Nx']
    dx = global_params['dx']
    dt = global_params['dt']
    Tmax = global_params['Tmax']
    Toutput = global_params['Toutput']
    C = global_params['C']
    V = global_params['V']

    # init fields and constants
    X = dx*np.linspace(0, Nx-1, num=Nx)

    Field_p_init = initfield_1D(X)

    # RK Fields
    k1 = np.zeros(Nx)
    k2 = np.zeros(Nx)
    k3 = np.zeros(Nx)
    k4 = np.zeros(Nx)
    y1 = np.zeros(Nx)
    y2 = np.zeros(Nx)
    y3 = np.zeros(Nx)

    # matrix for implicit schemes
    Mat = matrix.linearmatrix(Nx)

    # init fields and constants
    print('stability numbers : ')
    print('advection : {0}'.format(np.abs(V)*dt/dx))
    print('diffusion : {0}'.format(C*dt/dx**2))

    t = 0
    tlast = 0
    global_params['C'] *= dt
    global_params['V'] *= dt

    Field_p = Field_p_init.copy()
    Frames = []
    Frames.append(Field_p_init.copy())

    # generate matrix for implicit part I + Vdu/dx - C d2u/dx2
    scheme = global_params['scheme']
    if scheme == 'CN':
        global_params['V'] *= .5
        global_params['C'] *= .5
    C = global_params['C']
    V = global_params['V']
    Mat.add_advection(V, dx)
    Mat.add_diffusion(-C, dx)
    Mat.add_diag_constant(1)
    Mat.initInvMat1D()

    iterations = 0
    with Timer() as tf:
        while (t < Tmax):
            # uncomment line depending on choosen scheme
            if global_params['scheme'] == 'eule':
                operators.eule(k1, Field_p, **global_params)
            elif global_params['scheme'] == 'euli':
                operators.euli(Mat, k1, Field_p, **global_params)
            elif global_params['scheme'] == 'RK2':
                operators.RK2(k1, k2, y1, Field_p, **global_params)
            elif global_params['scheme'] == 'RK4':
                operators.RK4(k1, k2, k3, k4,
                              y1, y2, y3,
                              Field_p, **global_params)
            elif global_params['scheme'] == 'CN':
                operators.CranckN(Mat, k1, Field_p, **global_params)
            else:
                raise ValueError('scheme not specified')

            if (t - tlast) > Toutput:
                # print('processing.... {0}%'.format(int(100.*t/Tmax)))
                tlast += Toutput
                Frames.append(np.array(Field_p))
            t += dt
            iterations += 1
    print('total execution time in µs : {0}'.format(tf.interval))
    print('number of snapshots : {0}'.format(len(Frames)))
    print('used time for 1 time step : {0:.2f} µs'.format(
                                                     tf.interval/iterations
                                                     )
          )
    return Frames


if __name__ == '__main__':
    # get all output in a list of numpy array
    simu = simulate()

    # plot data
    anim_data = animated_plot_1d(simu)
    plt.show()
