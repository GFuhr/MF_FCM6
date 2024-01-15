#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = "Guillaume Fuhr"

import sys
import os
import setuptools
import pyximport
import matplotlib
import numpy as np
import matplotlib.pyplot as plt

sys.path.append(os.path.normpath(os.path.join(os.path.realpath(__file__), "../..")))
pyximport.install(setup_args={"include_dirs": np.get_include()})

try:
    from parameters import load_params, initfield_1d
except ModuleNotFoundError:
    from advdiff.parameters import load_params, initfield_1D

from utils.timer import Timer
from utils.plotting import figformat, animated_plot_1d
from utils.file import get_run_number, save_outputs

try:
    import operators_1d as operators
    from bicgstab import MatrixSolver
except ModuleNotFoundError:
    import advdiff.operators_1d as operators
    from advdiff.bicgstab import MatrixSolver

# normalized figures output adapted for better reading in articles/reports
figformat().apply()


# parameters
def simulate(verbose: bool = False, save_files: bool = False, init=None, **kwargs):
    """

    :param verbose: use verbose mode for outputs
    :param save_files: save snapshot in files
    :param kwargs: dict containing parameters used in simulation
    :return: list of snapshots
    """
    global_params = load_params(**kwargs)
    Nx = global_params["Nx"]
    dx = global_params["dx"]
    dt = global_params["dt"]
    Tmax = global_params["Tmax"]
    Toutput = global_params["Toutput"]
    C = global_params["C"]
    V = global_params["V"]

    # init fields and constants
    X = dx * np.linspace(0, Nx - 1, num=Nx)

    init_u0 = init or initfield_1d
    Field_p_init = init_u0(X)

    # RK Fields
    k1 = np.zeros(Nx)
    k2 = np.zeros(Nx)
    k3 = np.zeros(Nx)
    k4 = np.zeros(Nx)
    y1 = np.zeros(Nx)
    y2 = np.zeros(Nx)
    y3 = np.zeros(Nx)

    # matrix for implicit schemes
    # Mat = matrix.linearmatrix(Nx)
    Mat = MatrixSolver(Nx)

    # init fields and constants
    if verbose:
        print("stability numbers : ")
        print("advection : {0}".format(np.abs(V) * dt / dx))
        print("diffusion : {0}".format(C * dt / dx ** 2))
        print("scheme : {0}".format(global_params["scheme"]))
        print("boundaries : {0}".format(global_params["boundaries"]))

    t = 0
    tlast = 0
    global_params["C"] *= dt
    global_params["V"] *= dt
    derivative = global_params["derivative"].strip().lower()
    if derivative == "fwd":
        darray = np.array([0, -1, 1]) / dx
    elif derivative == "bwd":
        darray = np.array([-1, 1, 0]) / dx
    elif derivative == "cent":
        darray = np.array([-1, 0, 1]) / (2 * dx)
    else:
        raise ValueError("derivative can be : fwd, bwd or cent")
    global_params["V"] *= darray
    vector_op = global_params["V"] + np.array([1, -2, 1]) * global_params["C"] / (dx * dx)

    boundaries = global_params.get("boundaries", "dir").strip().lower()
    if boundaries == "dir":
        bc = np.array([-1, 0, -1, 0])
    elif boundaries == "neu":
        bc = np.array([1, 0, 1, 0])
    elif boundaries == "per":
        bc = np.array([0, 1, 0, 1])
    else:
        raise ValueError("boundary conditions not defined")
    global_params["bc"] = bc

    Field_p = Field_p_init.copy()
    Frames = [Field_p_init.copy()]
    TimeOutput = [0, ]

    # generate matrix for implicit part I + Vdu/dx - C d2u/dx2
    formatted_scheme = global_params["scheme"].strip().lower()

    if formatted_scheme == "cn":
        vector_op *= .5
    Mat.create_matrix(np.array([0, 1, 0]) - vector_op, bc)

    iterations = 0
    run_number = get_run_number(prefix='advdiff_', as_text=True)
    if save_files:
        save_outputs(Field_p, run_number, as_text=True, prefix='advdiff_')

    field_write_timer = 0
    with Timer() as tf:
        while t < Tmax:
            if formatted_scheme == "eule":
                operators.eule(k1, Field_p, vector_op, bc)
            elif formatted_scheme == "euli":
                operators.euli(Mat, k1, Field_p, bc)
            elif formatted_scheme == "rk2":
                operators.RK2(k1, k2, y1, Field_p, vector_op, bc)
            elif formatted_scheme == "rk4":
                operators.RK4(k1, k2, k3, k4,
                              y1, y2, y3,
                              Field_p,
                              vector_op, bc)
            elif formatted_scheme == "cn":
                operators.CranckN(Mat, k1, Field_p, vector_op, bc)
            else:
                raise ValueError("scheme not specified")

            if (t - tlast) > Toutput:
                with Timer() as tf_file:
                    if verbose:
                        print('processing.... {0}%'.format(int(100. * t / Tmax)))
                    if save_files:
                        save_outputs(Field_p, run_number, as_text=True, prefix='advdiff_')
                field_write_timer += tf_file.interval
                tlast += Toutput
                Frames.append(np.array(Field_p))
                TimeOutput.append(t + dt)
            t += dt
            iterations += 1
    print("total execution time in µs : {0}".format(tf.interval-field_write_timer))
    print("number of snapshots : {0}".format(len(Frames)))
    print("used time for 1 time step : {0:.2f} µs".format((tf.interval-field_write_timer) / iterations))

    return TimeOutput, Frames, (tf.interval-field_write_timer) / iterations


if __name__ == "__main__":
    # get all output in a list of numpy array

    simu = simulate()

    # plot data
    anim_data = animated_plot_1d(simu)
    plt.show()
