# __author__ = 'Guillaume Fuhr'
import sys

if int(sys.version[0]) < 3:
    sys.exit("This script shouldn't be run by python 2 ")
import pyximport
import os

sys.path.append(os.path.normpath(os.path.join(os.path.realpath(__file__), "../..")))

import numpy as np

import setuptools
import matplotlib.pyplot as plt

try:
    from utils.timer import Timer
    from utils.plotting import figformat, animated_plot_2d
except ModuleNotFoundError:
    sys.path.append(os.path.normpath(os.path.join(os.path.realpath(__file__), "../..")))
    from utils.timer import Timer
    from utils.plotting import figformat, animated_plot_2d
pyximport.install(setup_args={"include_dirs": np.get_include()})

import operators_2d_spectral as operators
from parameters import load_params, initfield_2D
from utils.file import get_run_number, save_outputs
from matrix import linearmatrix2D

if int(sys.version[0]) < 3:
    sys.exit("This script shouldn't be run by python 2 ")

figformat().apply()


def simulate(verbose=False, save_files=False, **kwargs):
    """

    :param verbose:
    :param save_files:
    :param kwargs:
    :return:
    """
    # parameters
    global_params = load_params(**kwargs)

    Nx = global_params["Nx"]
    Nm = global_params["Nm"]
    dx = global_params["dx"]

    ky = global_params["ky"]
    dy = global_params["dy"]

    dt = global_params["dt"]
    Tmax = global_params["Tmax"]
    Toutput = global_params["Toutput"]

    # RK Fields
    shape = (Nm, Nx)
    k1 = np.zeros(shape, dtype=np.complex128)
    k2 = np.zeros(shape, dtype=np.complex128)
    k3 = np.zeros(shape, dtype=np.complex128)
    k4 = np.zeros(shape, dtype=np.complex128)
    y1 = np.zeros(shape, dtype=np.complex128)
    y2 = np.zeros(shape, dtype=np.complex128)
    y3 = np.zeros(shape, dtype=np.complex128)
    rhs = np.zeros(shape, dtype=np.complex128)
    Field_p = np.zeros(shape, dtype=np.complex128)

    # init fields and constants
    Y, X = np.meshgrid(
        dx * np.linspace(0, Nx, num=Nx), dy * np.linspace(0, Nm, num=2 * Nm - 1)
    )

    U0 = initfield_2D(X, Y)

    # compute fourier represention of U(x,y,t=0)
    operators.compute_spectral_field(1, 1, U0, Field_p)

    # print stability condition
    C = global_params["C"]
    V = np.abs(global_params["V"])
    print("stability numbers : ")
    print("advection in x direction: {0}".format(V * dt / dx))
    print("advection in y direction: {0}".format(V * dt / dy))
    print("diffusion in x direction: {0}".format(C * dt / dx ** 2))
    print("diffusion in y direction: {0}".format(C * dt / dy ** 2))

    # main loop
    t = 0
    tlast = 0
    Field_p = Field_p.astype(np.complex)
    Frames = [Field_p.copy(),]

    iterations = 0
    run_number = get_run_number(prefix='h2dspec_', as_text=True)

    if save_files:
        save_outputs(U0, run_number, as_text=True, prefix='advdiff_')
    with Timer() as tf:
        while t < Tmax:
            if global_params["scheme"] == "eule":
                operators.eule(rhs, Field_p, **global_params)
            elif global_params["scheme"] == "euli":
                raise ValueError("euli scheme not implemented in 2D")
                # operators.euli(mat2D, rhs, Field_p, **global_params)
            elif global_params["scheme"] == "RK2":
                operators.RK2(k1, k2, y1, Field_p, **global_params)
            elif global_params["scheme"] == "RK4":
                operators.RK4(k1, k2, k3, k4, y1, y2, y3, Field_p, **global_params)
            elif global_params["scheme"] == "CN":
                #     operators.CranckN(Mat, k1, Field_p, **global_params)
                raise ValueError("CN scheme not implemented in 2D")
            else:
                raise ValueError("scheme not specified")

            if (t - tlast) > Toutput:
                if verbose:
                    print('processing.... {0}%'.format(int(100.*t/Tmax)))

                tlast += Toutput
                Frames.append(np.array(Field_p))
            t += dt
            iterations += 1

    print("number of frames {0}".format(len(Frames)))
    print("loop time  in µs {0}".format(tf.interval))
    print("used time for 1 time step : {0:.2f}".format(tf.interval / iterations))

    Frames_real = []
    real_data = np.zeros((2 * (Nm - 1), Nx), dtype=np.double)
    if verbose:
        print('converting snapshots from Fourier to real space')
    for slices in Frames:
        operators.compute_real_field(1, 1, slices, real_data)
        if save_files:
            save_outputs(real_data, run_number, as_text=True, prefix='h2dspec_')
        Frames_real.append(np.array(real_data))

    np.save("data_2D.npy", Frames_real, allow_pickle=False)
    print("Data saved to file : ./data_2D.npy")
    return Frames_real


if __name__ == "__main__":

    df = simulate()
    anim = animated_plot_2d(df)
    plt.show()
