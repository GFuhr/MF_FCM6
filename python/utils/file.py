from numpy import save as npsave
from numpy import savetxt as npsavetxt
from os.path import exists, isfile


def save_outputs(output, serie=0, prefix=None, as_text=True):
    """
    save output as numpy format in a file and increment file number if file already exists
    """
    idx = 0
    fprefix = prefix or 'out_'
    if as_text is True: 
        ext = 'txt'
    else:
        ext = 'npy'

    fname = '{0}{1:04d}_{2:04d}.{3}'.format(fprefix, serie, idx, ext)

    while exists(fname):
        idx += 1
        fname = '{0}{1:04d}_{2:04d}.{3}'.format(fprefix, serie,  idx, ext)

    if ext=='txt':
        npsavetxt(fname, output)
    else:
        npsave(fname, output)
    print('data saved in file [{0}]'.format(fname))
    return fname


def get_run_number(prefix=None, as_text=True):
    """
        get the next number available for a run starting with a given prefix
        as_text : check with respect to txt files (default, npy files)

        return : next available run number
    """
    idx = 0
    fprefix = prefix or 'out_'
    if as_text is True:
        ext = 'txt'
    else:
        ext = 'npy'
    fname = '{0}{1:04d}_{2:04d}.{3}'.format(fprefix, idx,  0, ext)
    while exists(fname):
        idx += 1
        fname = '{0}{1:04d}_{2:04d}.{3}'.format(fprefix, idx,  0, ext)

    return idx

def save_with_params(output, params):
    prefix = ''
    if params.get('C', None) is not None:
        prefix = 'diff_'
    if params.get('V', None) is not None:
        prefix += 'adv_'
    prefix += '{0:03d}_'.format(params['Nx'])
    save_outputs([params, output], prefix=prefix)