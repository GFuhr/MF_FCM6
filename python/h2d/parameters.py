from types import ModuleType
from numpy import pi
import numpy as np


# list of parameters used in both advdiff et H2D.
# each variable written in this file will be transmitted in the main function in the dictionnary global_params
# other variables (but not functions) can e inserted and will be added automatically

# time step
dt = .001

# x step
dx = .4

# y step (used only for H2D simulations)
dy = .4

# Points in X direction
Nx = 128

# Points in Y direction
Ny = 128

# modes in Y direction
Nm = 128

# wave number in Y direction
ky = 2*pi/(Ny*dy)


# end time
Tmax = 100000*dt

# output time
Toutput = 100*dt

# diffusion coefficient
C = 1.8

# advection coefficient
V = -2

# time scheme
# can be
# eule for euler explicit (default)
# euli for euler implicit
# RK2 for Runge-Kutta 2
# RK4 for Runge-Kutta 4
# CN for Cranck-Nicholson
scheme = 'eule'

	#############
# functions #
#############


def initfield_1D(x: np.array):
    """
    generate initial profile for advdiff simulations,
    :param x: meshgrid for X values
    :return: 1D field
    """
    u0 = np.zeros(x.shape)
    _dx = x[1] - x[0]
    u0 = np.sin(np.pi*x/x.max())

    # exemple for gate
    # U0[:] = 1
    # U0[0:U0.shape[0]//4] = 0
    # U0[3*U0.shape[0] // 4:-1] = 0
    return u0


def initfield_2D(x: np.array, y: np.array):
    """
    generate initial profile for H2D simulations,
    :param x: meshgrid for X values
    :param y: meshgrid for Y values
    :return: 2D field
    """
    u0 = np.zeros(x.shape)
    _dx = x[1] - x[0]
    _dy = y[1] - y[0]
    u0 = np.sin(x/x.max()+y/y.max())

    # exemple for gate
    u0[:, :] = 1
    u0[0:u0.shape[0]//4, :] = 0
    u0[3*u0.shape[0]//4:-1, :] = 0
    u0[:, 0:u0.shape[1]//4] = 0
    u0[:, 3*u0.shape[1]//4:-1] = 0
    return u0


# don't modify this function
def load_params(**kwargs):
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
    params['scheme'] = params['scheme'].strip()
    return params


if __name__ == '__main__':
    pass
