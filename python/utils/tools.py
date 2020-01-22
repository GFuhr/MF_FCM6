# -*- coding: utf-8 -*-
__author__ = "Guillaume Fuhr"

import numpy as np


# function extracting the max value and his position for each serie of a list of profiles
def _extract_max_1D(Frames):
    list_max = np.zeros(shape=(2, len(Frames)))
    idx = 0
    for frame in Frames:
        list_max[:,idx] = (frame.argmax(), frame.max())
        idx += 1
    return np.asarray(list_max)


def _extract_max_2D(Frames)->np.array:
    list_max = np.zeros(shape=(3, len(Frames)))
    idx = 0
    for frame in Frames:
        idx_tuple = np.unravel_index(frame.argmax(), frame.shape)
        list_max[:,idx] = (*idx_tuple, frame.max())
        idx += 1
    return np.asarray(list_max)


def extract_max(Frames:list)->np.array:
    """
    extract and return the max with his corresponding index for a list of snapshots
    :param Frames: list of snapshots
    :return: a list containing :
       - in 1D : [index_for_max, value_of max]
       - in 2D : [index_for_max_y, index_for_max_x, value_of max]
    """
    dims = len(Frames[0].shape)
    if dims == 1:
        return _extract_max_1D(Frames)
    else:
        return _extract_max_2D(Frames)
