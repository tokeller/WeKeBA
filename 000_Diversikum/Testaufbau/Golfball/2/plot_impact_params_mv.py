#!/opt/anaconda/bin/python
"""
    example
    
    """


from pylab import *
import csv

raw = genfromtxt('golfb10k_raw.csv', delimiter=',')

tr = [x for x,y in raw]
vr = [y for x,y in raw]
#print t, v
print(tr[0],tr[-1])
tr = [x for x in tr]
vr = [x/4096*3.3 for x in vr]
#print(t)

# horiz. und vertik. Linien
hl=[1.50,1.66,1.82]
vl=[183.5,366.5,396.5]

# Rohdaten Zoom
figure(num=1)
xax=[160,480]
xlim(xax)
plot(tr,vr,'-g',linewidth=1)

xlabel('Sample Nr. []')
ylabel('Messwert [mV]')
#title('Einschlag eines Golfballs auf einer Aluplatte\nRohdaten')
#thresholds
annotate('Threshold', xy=(416,1.57), xytext=(430, 1.216),arrowprops={'arrowstyle':'simple','facecolor':'none'})
annotate('Nullpegel', xy=(430,1.66), xytext=(430, 1.68))
annotate('Threshold', xy=(416,1.72), xytext=(430, 2.02),arrowprops={'arrowstyle':'simple','facecolor':'none'})

arrow(416, 1.50, 0, 0.120,fc="k", ec="k",
head_width=5, head_length=0.04 )
arrow(416, 1.66, 0, -0.120,fc="k", ec="k",
head_width=5, head_length=0.04 )
arrow(416, 1.66, 0, 0.120,fc="k", ec="k",
head_width=5, head_length=0.04 )
arrow(416, 1.82, 0, -0.120,fc="k", ec="k",
head_width=5, head_length=0.04 )

#timeout
annotate('Timeout', xy=(396.5, 2.42), xytext=(370,2.42))
arrow(376.5, 2.33, 10, 0,fc="k", ec="k",head_width=0.04, head_length=10 )
arrow(396.5, 2.33, -20, 0,fc="k", ec="k",head_width=0.04, head_length=10 )

hlines(hl,0,980,linestyles=['dashed','dashed'],linewidth=0.7)
vlines(vl,0.405,2.839,linestyles=['solid','dashed','dashed'],linewidth=0.7,color='red')
grid(True)
savefig("../../../../999_Doku/images/impact_params_mv.png")
show()
