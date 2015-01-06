# import signal tools from scipy.signal package:
import scipy.signal.signaltools as sigtool

# import numpy to import data
import numpy as np

# if fileloc is a string containing the location of the file
# with your binary data:
data = np.fromfile(fileloc)
# alternatively if you have save your data in .txt format,
# you can also import it with data = np.loadtxt(fileloc),
# or something like that

# computation of the Hilbert transport for data
data_Hilbert = sigtool.hilbert(data)
# the envelope function is actually the absolute value of
# the analytical signal computed with the Hilbert transform:
data_env = abs(data_Hilbert)

# depending on the symmetricity of the signal, which is might
# be bounded to the sampling rate, you will want to detrend
# and/or smooth your resulting envelope.