import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
from os.path import basename, splitext
class h2Dloader(object):
    """
        H2D data loader and plot
    """

    def __init__(self, fname):
        self.fname = fname
        self.prefix = splitext(basename(fname))[0]
        self.figname_r = u'{0}_r.png'.format(self.prefix)
        self.figname_f = u'{0}_f.png'.format(self.prefix)
        self.nx = 0
        self.ny = 0
        self.datamodes = None
        self._load_h2D()
        self.iscplx = False

    def _load_h2D(self):
        self.data = np.loadtxt(self.fname)
        self.ny, self.nx = self.data.shape

    def _ascplx(self):
        if self.iscplx:
            return
        self.datamodes = self.data[0:self.ny/2, 1:self.nx-1]+1j*self.data[self.ny/2:self.ny, 1:self.nx-1]
        self.ny, self.nx = self.datamodes.shape
        self.iscplx = True
        self.data = None
        return

    def max(self):
        return np.max(np.max(self.data))

    def min(self):
        return np.min(np.min(self.data))

    def plotmodes(self):
        if self.datamodes is None:
            self.convertreal()

        ny, nx = self.datamodes.shape
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        X = np.arange(0, nx)
        Y = np.arange(0, ny)
        X, Y = np.meshgrid(X, Y)
        Z = np.abs(self.datamodes)
        surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=cm.coolwarm,
                               linewidth=0, antialiased=False)
        ax.zaxis.set_major_locator(LinearLocator(10))
        ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
        ax.set_title('$|u_m(x)|$')
        ax.set_xlabel('x')
        ax.set_ylabel('m')
        fig.colorbar(surf, shrink=0.5, aspect=5)
        fig.savefig(self.figname_f, format='png')

    def convertreal(self):
        self._ascplx()
        newy = (self.ny)*2

        self.rdata = np.zeros((newy, self.nx ))
        self.rdata = self.rdata + 0j
        for idx in np.arange(0, self.nx):
            vector = np.squeeze(self.datamodes[0:self.ny, idx])
            vector = np.append(vector, [0+0j, ])
            ivector = np.fft.hfft(vector)
            nv = ivector.shape[0]
            vy = np.zeros((nv,))
            vy = vy + 0j
            vy[0:nv/2] = ivector[nv/2:nv].copy()
            vy[nv/2:nv] = ivector[0:nv/2].copy()
            self.rdata[0:newy, idx] = vy
        self.ny = newy
        self.data = self.rdata.copy()

    def plot(self):
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        X = np.arange(0, self.nx)
        Y = np.arange(0, self.ny)
        X, Y = np.meshgrid(X, Y)
        Z = np.real(self.data)
        surf = ax.plot_surface(X, Y, Z, alpha = 0.4, rstride=1, cstride=1, cmap=cm.coolwarm,
                               linewidth=0, antialiased=False)
        ax.zaxis.set_major_locator(LinearLocator(10))
        ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
        ax.set_title('u(x,y)')
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        fig.colorbar(surf, shrink=0.5, aspect=5)
        fig.savefig(self.figname_r, format='png', transparent=False)

if __name__ == '__main__':
    import os
    os.chdir(r'../TP2')

    #data in fourier format
    data = h2Dloader('H2D_OCT_0430.dat')
    data.convertreal()
    data.plot()
    data.plotmodes()
    print(data.max())
    print(data.min())
    #data in real format
    data = h2Dloader('H2D_OCT_0190.dat')
    data.plot()
    plt.show()
