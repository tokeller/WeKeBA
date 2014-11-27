#!/opt/anaconda/bin/python
"""
    example
    
    """


from pylab import *
import csv

with open('golfb10k.csv', 'rb') as csvfile:
    my_data = genfromtxt('golfb10k.csv', delimiter=',')
print my_data

t = [x for x,y in my_data]
v = [y for x,y in my_data]
print t, v
t = [x/100 for x in t]
v = [(x-2047)/16 for x in v] # 16mV / g
print(t)
plot(t,v)

xlabel('Zeit (ms)')
ylabel('Beschleunigung (g)')
title('Einschlag eines Golfballs auf einer Aluplatte')
grid(True)
savefig("test.png")
show()
    
'''
    for row in my_data:
        print row[0]+row[1]'''