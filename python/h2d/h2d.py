# -*- coding: utf-8 -*-
__author__ = "Guillaume Fuhr"
import sys
import os
import setuptools
import pyximport
import numpy as np
import matplotlib.pyplot as plt



try:
    from utils.timer import Timer
    from utils.plotting import figformat, animated_plot_2d
except ModuleNotFoundError:
    sys.path.append(os.path.normpath(os.path.join(os.path.realpath(__file__), "../..")))
    from utils.timer import Timer
    from utils.plotting import figformat, animated_plot_2d

from parameters import load_params, initfield_2D
from utils.file import get_run_number, save_outputs
pyximport.install(setup_args={"include_dirs": np.get_include()})

if int(sys.version[0]) < 3:
    sys.exit("This script shouldn't be run by python 2 ")

import h2d.operators_2d as operators
from h2d.matrix2D import LinearMatrix2D

# use for figure format for the full script
figformat().apply()


def simulate(verbose=False, save_files=False, init=None,  **kwargs):
    """

    :param verbose:
    :param save_files:
    :param kwargs:
    :return:
    """
    # parameters
    global_params = load_params(**kwargs)

    Nx = global_params["Nx"]
    Ny = global_params["Ny"]
    dx = global_params["dx"]
    dy = global_params["dy"]
    dt = global_params["dt"]
    Tmax = global_params["Tmax"]
    Toutput = global_params["Toutput"]
    C = global_params["C"]
    V = global_params["V"]

    shape = (Ny, Nx)
    rhs = np.zeros(shape)
    # RK Fields
    k1 = np.zeros(shape)
    k2 = np.zeros(shape)
    k3 = np.zeros(shape)
    k4 = np.zeros(shape)
    y1 = np.zeros(shape)
    y2 = np.zeros(shape)
    y3 = np.zeros(shape)

    # init fields and constants
    Y, X = np.meshgrid(dx * np.linspace(0, Nx, num=Nx), dy * np.linspace(0, Ny, num=Ny))

    if init is None:
        Field_p = initfield_2D(X, Y)
    else:
        Field_p = init(X, Y)

    if verbose:
        print("stability numbers : ")
        print("advection in x direction: {0}".format(np.abs(V) * dt / dx))
        print("advection in y direction: {0}".format(np.abs(V) * dt / dy))
        print("diffusion in x direction: {0}".format(C * dt / dx ** 2))
        print("diffusion in y direction: {0}".format(C * dt / dy ** 2))

    mat2D = LinearMatrix2D(*shape)
    mat2D.init(-C * dt, dx, dy)

    # main loop
    t = 0
    tlast = 0
    Frames = []
    Frames.append(Field_p.copy())

    iterations = 0
    run_number = get_run_number(prefix='h2d_', as_text=True)

    if save_files:
        save_outputs(Field_p, run_number, as_text=True, prefix='advdiff_')
    with Timer() as tf:
        while t < Tmax:

            if global_params["scheme"] == "eule":
                operators.eule(rhs, Field_p, **global_params)
            elif global_params["scheme"] == "euli":
                operators.euli(mat2D, rhs, Field_p, **global_params)
            elif global_params["scheme"] == "RK2":
                operators.RK2(k1, k2, y1, Field_p, **global_params)
            elif global_params["scheme"] == "RK4":
                operators.RK4(k1, k2, k3, k4, y1, y2, y3, Field_p, **global_params)
            elif global_params["scheme"] == "CN":
                # operators.CranckN(Mat, k1, Field_w, **global_params)
                raise ValueError("CN scheme not implemented in 2D")
            else:
                raise ValueError("scheme not specified")

            if (t - tlast) > Toutput:
                if verbose:
                    print('processing.... {0}%'.format(int(100.*t/Tmax)))
                if save_files:
                    save_outputs(Field_p, run_number, as_text=True, prefix='h2d_')
                tlast += Toutput
                Frames.append(np.array(Field_p))
            t += dt
            iterations += 1

    print("number of frames {0}".format(len(Frames)))
    print("loop time  in µs {0}".format(tf.interval))
    print("used time for 1 time step in µs: {0:.2f}".format(tf.interval / iterations))
    # disable pickel for better compatibility
    np.save("data_2D.npy", Frames, allow_pickle=False)
    print("Data saved to file : ./data_2D.npy")
    return Frames


if __name__ == "__main__":

    Frames = simulate(verbose=True)
    anim = animated_plot_2d(Frames)
    plt.show()
