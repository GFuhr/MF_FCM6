from numpy import save as npsave
from os.path import exists, isfile

def save_outputs(output, prefix=None):
    """
    save output as numpy format in a file and increment file number if file already exists
    """
    idx = 0
    fprefix = prefix or 'out_'
    fname = '{0}{1:04d}.npy'.format(fprefix, idx)
    while exists(fname):
        idx +=1
        fname = '{0}{1:04d}.npy'.format(fprefix, idx)
        
    npsave(fname, output)
    print('data saved in file [{0}]'.format(fname))
    return fname