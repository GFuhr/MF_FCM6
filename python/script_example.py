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
    from utils.plotting import figformat, animated_plot_2d, animated_plot_1d

if int(sys.version[0]) < 3:
    sys.exit("This script shouldn't be run by python 2 ")

sys.path.append('./advdiff')
sys.path.append('./h2d')
pyximport.install(setup_args = {'include_dirs': [np.get_include(), os.path.abspath('./advdiff')]})
from advdiff import  advdiff
from h2d import h2d
from h2d import h2d_spectral
from ns2d import ns2d

try:
    os.chdir('./run')
except FileNotFoundError:
    os.mkdir('./run')
    os.chdir('./run')

if __name__=='__main__':
    data1d = advdiff.simulate()
    video_1 = animated_plot_1d(data1d)
    data2d = h2d.simulate()
    video_2 = animated_plot_2d(data2d)
    data2d = h2d_spectral.simulate()
    video_3 = animated_plot_2d(data2d)
    plt.show()