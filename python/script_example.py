# -*- coding: utf-8 -*-
__author__ = "Guillaume Fuhr"
import sys
import os
import setuptools
import pyximport
import numpy as np
import matplotlib.pyplot as plt

try:
    from utils.plotting import figformat, animated_plot_2d, animated_plot_1d
except ModuleNotFoundError:
    sys.path.append(os.path.normpath(os.path.realpath(__file__)))
    from utils.plotting import figformat, animated_plot_2d

if int(sys.version[0]) < 3:
    sys.exit("This script shouldn't be run by python 2 ")

sys.path.append('./advdiff')
sys.path.append('./h2d')
pyximport.install(setup_args = {'include_dirs': [np.get_include(), os.path.abspath('./advdiff')]})
from advdiff import  advdiff
from h2d import h2d
from h2d import h2d_spectral
os.chdir('./run')

if __name__=='__main__':
    data1d = advdiff.simulate()
    animated_plot_1d(data1d)
    data2d = h2d_spectral.simulate()
    animated_plot_2d(data2d)
    plt.show()