# -*- coding: utf-8 -*-
__author__ = "Guillaume Fuhr"

import sys
import os
import setuptools
import pyximport
import numpy as np
import matplotlib.pyplot as plt

sys.path.append('../')
try:
    from utils.plotting import figformat, animated_plot_2d, animated_plot_1d
except ModuleNotFoundError:
    sys.path.append(os.path.normpath(os.path.realpath(__file__)))
    from utils.plotting import figformat, animated_plot_2d

if int(sys.version[0]) < 3:
    sys.exit("This script should not be run using python 2 ")
pyximport.install(setup_args={'include_dirs': [np.get_include(), './advdiff']})

# os.chdir('./advdiff')
print(os.getcwd())
import matrix
import advdiff


if __name__ == '__main__':
    import matplotlib.pyplot as plt

    data1d = advdiff.simulate()
    anim = animated_plot_1d(data1d)
    plt.show()
