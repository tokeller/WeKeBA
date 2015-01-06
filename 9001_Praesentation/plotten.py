# import plot functions of matplotlib
import matplotlib.pyplot as plt

# Figur kreieren:
fig = plt.figure(figsize=(8,3), dpi=80, facecolor='w', edgecolor='k')
# unique subplot to modify attributes later:
ax = fig.add_subplot(111)

# plot original signal (p1) and its enveloppe funktion in ax:
p1, = ax.plot(data)
p2, = plot(data_env)
# you need to divide the number of counts in data by the sampling
# rate to get seconds unit in the x axis... etc... etc...

# with matplotlib you can do almost everything you can imagine,
# to modify plot attributes and some very useful examples see:
# http://matplotlib.org/