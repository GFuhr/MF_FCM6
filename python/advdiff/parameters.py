from types import ModuleType
from numpy import pi
import numpy as np

# list of parameters used in both advdiff et H2D.
# each variable written in this file will be transmitted in the main function in the dictionnary global_params
# other variables (but not functions) can be added and will be inserted automatically in the dictionnary global_params

# time step
dt = .01

# x step
dx = .4

# y step (used only for H2D simulations)
dy = .4

# Points in X direction
Nx = 128

# Points in Y direction
Ny = 128

# modes in Y direction
Nm = 32

# wave number in Y direction
ky = 2 * pi / (Ny * dy)

# end time
Tmax = 10000 * dt

# output time
Toutput = 10 * dt

# diffusion coefficient
C = 1.8

# advection coefficient
V = -.5

# time scheme
# can be
# eule for euler explicit (default)
# euli for euler implicit
# RK2 for Runge-Kutta 2
# RK4 for Runge-Kutta 4
# CN for Cranck-Nicholson
scheme = 'RK4'

# first order derivatives
# fwd for forward  (u[i+1]-u[i])/dx
# bwd for backward [default](u[i]-u[i-1])/dx
# cent for centered (u[i+1]-u[i-1])/(2dx)
derivative = "bwd"

# boundary condition
# dir for Dirichlet u[0] = 0
# neu for Von Neumann du/dx[0] = 0
# per for periodic u[0] = u[Lx]
boundaries = "neu"


#############
# functions #
#############


def initfield_1d(x: np.array):
    """
    generate initial profile for advdiff simulations,
    :param x: meshgrid for X values
    :return: 1D field
    """

    _dx = x[1] - x[0]
    u0 = np.sin(np.pi * x / x.max())

    # exemple for gate
    # U0[:] = 1
    # U0[0:U0.shape[0]//4] = 0
    # U0[3*U0.shape[0] // 4:-1] = 0
    return u0


def initfield_2d(x: np.array, y: np.array) -> np.array:
    """
    generate initial profile for H2D simulations,
    :param x: meshgrid for X values
    :param y: meshgrid for Y values
    :return: 2D field
    """
    _dx = x[1] - x[0]
    _dy = y[1] - y[0]
    u0 = np.sin(x / x.max() + y / y.max())

    # exemple for gate
    u0[:, :] = 1
    u0[0:u0.shape[0] // 4, :] = 0
    u0[3 * u0.shape[0] // 4:-1, :] = 0
    u0[:, 0:u0.shape[1] // 4] = 0
    u0[:, 3 * u0.shape[1] // 4:-1] = 0
    return u0


# don't modify this function
def load_params(**kwargs) -> dict:
    """
    convert all global variable of this script as a dict with entries :
    variable_names_as_string : variable_value
    :param kwargs: additional arguments or modified values for default ones
    :return: dict of parameters
    """
    params = {}
    lvars = dict(globals())
    for k, v in lvars.items():
        # check if variable is not internal variable
        if isinstance(v, ModuleType):
            continue
        if not k.startswith('_'):
            # check if it's not a function
            if not hasattr(v, '__call__'):
                params.update({str(k): v})
    params.update(kwargs)
    if params.get('scheme') is None:
        params['scheme'] = 'eule'
    if params.get('derivative') is None:
        params['derivative'] = 'bwd'

    if params.get("boundaries") is None:
        params['boundaries'] = 'dir'
    params['scheme'] = params['scheme'].strip()
    params['boundaries'] = params['boundaries'].strip()
    return params


if __name__ == '__main__':
    raise RuntimeError("this module is not supposed to be run directly")
