import matplotlib.pyplot as plt
from matplotlib import animation
from IPython.display import HTML
from numpy import linspace as nplinspace





import matplotlib.pyplot as plt
from matplotlib import animation
import os
from numpy import linspace



class figformat(object):
    """
    class containing figures format specifications :
      size, linewidth...
      used by matplotlib
    """

    def __init__(self, *args, **kwargs):
        self.linewidth = kwargs.get('LineWidth', 4)
        self.markersize = kwargs.get('MarkerSize', 12)
        self.markeredgewidth = kwargs.get('MarkerEdgeWidth', 1)
        self.fontsize = kwargs.get('FontSize', 20)
        self.fontname = kwargs.get('FontName', 'Verdana')
        self.figsize = kwargs.get('FigSize', (9, 9))
        self.titlesize = kwargs.get('TitleSize', 24)

    def apply(self):
        import matplotlib as mpl
        mpl.rc('lines', linewidth=self.linewidth)
        mpl.rc('lines', markersize=self.markersize)
        mpl.rc('lines', markeredgewidth=self.markeredgewidth)
        #mpl.rc('font', family=self.fontname)
        mpl.rc('font', size=self.fontsize)
        #mpl.rc('tetex', usetex=True)
        mpl.rc('figure', figsize=self.figsize)
        #mpl.rc(u'text', usetex=True)
        pass

    def savefigs( prefix=None):
        fname = prefix or 'output_'
        for idxf in plt.get_fignums():
            plt.figure(idxf)
            plt.savefig('{0}{1}.png'.format(fname, idxf), format='png')#, transparent=False)
            plt.savefig('{0}{1}.jpg'.format(fname, idxf), format='jpg', transparent=False)
            try:
                plt.savefig('{0}{1}.eps'.format(fname, idxf), format='eps')
            except RuntimeError:
                print('unable to handle postscript generation')
                print('if you are on windows, see http://stackoverflow.com/questions/19745282/cant-save-figure-as-eps-gswin32c-is-not-recognized')



def plot_profile(frames, idx, save=False):
    figformat().apply()
    fig = plt.figure(figsize=(10,10))
    plt.plot(frames[idx])
    plt.xlabel("x")
    plt.ylabel("u(x)")
    plt.grid(True)
    if save:
        plt.savefig('{0}{1}.png'.format("fig_", 0), format='png')#, transparent=False)



def animated_plot_1d(Frames):
    """
     create animation from simulation outputs
    :param Frames: simulation results
    :return: None
    """
    Nx = Frames[0].shape[0]
    X = nplinspace(0, Nx - 1, num=Nx)
    ### animated plot ###
    def update_line(num, data, line):
        line.set_data(X, Frames[num])
        return line,
    
    fig = plt.figure(figsize=(8,8))
    ax = plt.axes(xlim=(X.min(), X.max()), ylim=(1.1*Frames[0].min(), 1.1*Frames[0].max()))
    ax.grid(color='r', linestyle='--', linewidth=2)
    l, = ax.plot([], [], lw=4)

    plt.xlabel('x')
    plt.ylabel('y(x)')
    idx = 0
    for f in Frames:
        if f.max()>10000*Frames[0].max():
            print('Warning : growing instability detected')
            break
        idx +=1
    anim_length = min(idx, len(Frames)-1 )
    anim = animation.FuncAnimation(fig, update_line, fargs=(Frames, l),
                                   frames=anim_length,  blit=True)
    #plt.close(fig)
    return anim


def animated_plot_2d(Frames):
    fig = plt.figure()
    plt.title('time evolution')
    ax = plt.axes(label='2D plot')
    PCM=ax.pcolorfast(0*Frames[0], vmin=0, vmax=1, cmap='seismic')
    plt.colorbar(PCM)
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    def animate( i):
        # fig.clf()
        # This is where new data is inserted into the plot.
        ax.clear()
        ax.pcolorfast(Frames[i], vmin=0, vmax=1, cmap='seismic')

    anim = animation.FuncAnimation(fig, animate,
                                   frames=len(Frames)-1,  blit=False)

#    plt.close(fig)
    return anim

def plot_results(output, save=False, filename=None):
    """
        make an animation from a list of numpy array and plot it
        params : save : save animation on disk, default: False
                 filename : name of the file, default: movie.mp4

        this function need ffmpeg to work. The best way to install it on windows is through conda : 
        conda -c conda-forge install ffmpeg
                 
    """
    anim = animated_plot_1d(output)
    if save:
        savefile = filename or "movie.mp4"
        anim.save(savefile, fps=24)
    return HTML(anim.to_html5_video())