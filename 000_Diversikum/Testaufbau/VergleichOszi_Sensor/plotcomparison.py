#!/opt/anaconda/bin/python
"""
    example
    
    """


from pylab import *
import csv

#with open('golfb10k.csv', 'rb') as csvfile:
sen = genfromtxt('sensor.csv', delimiter=',')
osz = genfromtxt('osz.csv', delimiter=',')
#print raw

ts = [x for x,y in sen]
vs = [y for x,y in sen]
tos = [x for x,y in osz]
vos = [y for x,y in osz]

#print t, v
print(ts[0],ts[-1])
ts = [x-1487903 for x in ts]
vs = [x for x in vs]
tos = [x-1 for x in tos]
vos = [x*12.375-2100 for x in vos]
#print(t)

# Rohdaten in Uebersicht
figure(num=0)
xlim([-10,100])
plot(tos,vos, '-b', linewidth=0.6)
plot(ts,vs,'-r',linewidth=2)

xlabel('Zeit [ms]')
ylabel('Beschleunigung [g]')

grid(True)
show()
savefig("../../999_Doku/images/comparison/")