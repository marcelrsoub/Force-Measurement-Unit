
import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt(open("samples/sample-S0229-P2-4-2.csv", "rb"),delimiter=",")

time=data[:,0]
value1=data[:,1]
value2=data[:,2]

plt.plot(time,value1,time,value2)
# plt.title()
plt.ylabel("Force (g)")
plt.xlabel("Time (s)")
plt.show()
