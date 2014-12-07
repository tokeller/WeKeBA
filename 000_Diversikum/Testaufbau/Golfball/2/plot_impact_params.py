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
vr = [x for x in vr]
#print(t)

# horiz. und vertik. Linien
hl=[1847,2047,2247]
vl=[183.5,366.5,396.5]

# Rohdaten Zoom
figure(num=1)
xax=[160,480]
xlim(xax)
plot(tr,vr,'-g',linewidth=1)

xlabel('Sample Nr. []')
ylabel('Messwert []')
#title('Einschlag eines Golfballs auf einer Aluplatte\nRohdaten')
#thresholds
annotate('Threshold', xy=(416,1947), xytext=(430, 1500),arrowprops={'arrowstyle':'simple','facecolor':'none'})
annotate('Nullpegel', xy=(430,2047), xytext=(430, 2090))
annotate('Threshold', xy=(416,2147), xytext=(430, 2500),arrowprops={'arrowstyle':'simple','facecolor':'none'})

arrow(416, 1847, 0, 150,fc="k", ec="k",
head_width=5, head_length=50 )
arrow(416, 2047, 0, -150,fc="k", ec="k",
head_width=5, head_length=50 )
arrow(416, 2047, 0, 150,fc="k", ec="k",
head_width=5, head_length=50 )
arrow(416, 2247, 0, -150,fc="k", ec="k",
head_width=5, head_length=50 )

#timeout
annotate('Timeout', xy=(396.5, 3000), xytext=(370,3000))
arrow(376.5, 2900, 10, 0,fc="k", ec="k",
head_width=50, head_length=10 )
arrow(396.5, 2900, -20, 0,fc="k", ec="k",
head_width=50, head_length=10 )

hlines(hl,0,980,linestyles=['dashed','dashed'],linewidth=0.7)
vlines(vl,500,3500,linestyles=['solid','dashed','dashed'],linewidth=0.7,color='red')
grid(True)
savefig("../../../../999_Doku/images/impact_params.png")
show()
