# https://stackoverflow.com/questions/25191620/
#   creating-lowpass-filter-in-scipy-understanding-methods-and-units

import numpy as np
from scipy.signal import butter, filtfilt, freqz, hilbert
from matplotlib import pyplot as plt


def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def offsetRemover(data):
    offset = float((np.max(data)+np.min(data))/2)
    return [val - offset for val in data]
    # return data

def butter_lowpass_filter_full_wave(data, cutoff, fs, order=5):
    data = [(-1)*val if val < 0 else val for val in data]
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    # y=data
    return y