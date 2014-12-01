#!/opt/anaconda/bin/python
"""
    example
    
    """


from pylab import *
import csv

#with open('golfb10k.csv', 'rb') as csvfile:
raw = genfromtxt('golfb10k_raw.csv', delimiter=',')
detailed = genfromtxt('golfb10k_detailed.csv', delimiter=',')
peaks = genfromtxt('golfb10k_peaks.csv', delimiter=',')
#print raw

tr = [x for x,y in raw]
vr = [y for x,y in raw]
td = [x for x,y in detailed]
vd = [y for x,y in detailed]
tp = [x for x,y in peaks]
vp = [y for x,y in peaks]
#print t, v
print(tr[0],tr[-1])
tr = [x/100 for x in tr]
vr = [(x-2047)/16 for x in vr] # 16mV / g
td = [x/100 for x in td]
vd = [(x-2047)/16 for x in vd] # 16mV / g
tp = [x/100 for x in tp]
vp = [(x-2047)/16 for x in vp] # 16mV / g
#print(t)

# Rohdaten in Uebersicht
figure(num=0)
plot(tr,vr,'-g',linewidth=0.7)

xlabel('Zeit [ms]')
ylabel('Beschleunigung [g]')
title('Einschlag eines Golfballs auf einer Aluplatte\nRohdaten')

hl=[-200/16,200/16]
vl=[1.835,3.675]
hlines(hl,0,10,linestyles=['dashed','dashed'],linewidth=0.7)
vlines(vl,-80,80,linewidth=0.7,color='red')
grid(True)
savefig("raw.png")

# Rohdaten Zoom
figure(num=1)
xax=[1.60,4.40]
xlim(xax)
plot(tr,vr,'-g',linewidth=1)

xlabel('Zeit [ms]')
ylabel('Beschleunigung [g]')
#title('Einschlag eines Golfballs auf einer Aluplatte\nRohdaten')

hlines(hl,0,10,linestyles=['dashed','dashed'],linewidth=0.7)
vlines(vl,-80,80,linewidth=0.7,color='red')
grid(True)
savefig("rawshort.png")

# Detailmodus Zoom
figure(num=2)
xlim(xax)
plot(tr,vr,'--b',linewidth=0.7)
plot(td,vd,'-g',linewidth=1)


xlabel('Zeit [ms]')
ylabel('Beschleunigung [g]')
#title('Einschlag eines Golfballs auf einer Aluplatte\nDetailmodus')

hlines(hl,0,10,linestyles=['dashed','dashed'],linewidth=0.7)
vlines(vl,-80,80,linewidth=0.7,color='red')

grid(True)
savefig("detailed.png")
#show

# Peaks
figure(num=3)
xlim(xax)
plot(tr,vr,'--b',linewidth=0.7)
plot(tp,vp,'xg',markersize=9, markeredgewidth=2)

xlabel('Zeit [ms]')
ylabel('Beschleunigung [g]')
#title('Einschlag eines Golfballs auf einer Aluplatte\nPeaks')

hlines(hl,0,10,linestyles=['dashed','dashed'],linewidth=0.7)
vlines(vl,-80,80,linewidth=0.7,color='red')

grid(True)
savefig("peaks.png")
show()