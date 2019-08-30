#%% Imports and Filename
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

filename="bottom-over-trigger-no-b-26-07"

#%% Data
real_weight= np.array([0.0, 154.76,158.12, 176.26, 210.99, 257.74])	#* in grams
real_uncertainty=0.01

# fmu_raw = np.array([0, 257.1666, 261.3333, 275, 316.1667,342.1667])  # * top in points (0-1023)
fmu_raw = np.array([0,910,912.333,917,923.6667,931])  # * bottom in points (0-1023)

fmu_uncertainty=30	#in points (0-1023)
fmu_raw=fmu_raw-fmu_raw[0]*np.ones((len(fmu_raw),))		#normalize raw data for 0 (first value)

#%% FMU Uncertainty raw to Force
# fmu_uncertainty=np.array([fmu_uncertainty])

voltage = fmu_uncertainty * (5.0 / 1023.0)
resistance = 10000.0 * (5.0 / (voltage)-1.0)  # Pont diviseur de tension
conductance = 1.0/resistance
k = 10000.0
force = k*conductance
weight=force* 1E3 / 9.81
fmu_uncertainty=weight


#%% FMU raw data to Force
# Convert the analog reading(which goes from 0 - 1023) to a voltage(0 - 5V):
voltage = fmu_raw * (5.0 / 1023.0)*np.ones((len(fmu_raw)),)

for i in range(len(voltage)):	# prevent division by zero on next line
	if voltage[i]==0:			# set 0 to Arduino Voltage Resolution: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/
		voltage[i] = 0.0000001

resistance = 10000.0 * (5.0 / (voltage)-1.0)  # Pont diviseur de tension
conductance = 1.0/resistance
force = k*conductance
weight=force* 1E3 / 9.81

fmu_weight = weight

#%% Grams to Newtons and curve_fit

real_weight=real_weight*1E-3*9.81 #grams to Newtons
fmu_weight=fmu_weight*1E-3*9.81 #grams to Newtons

fmu_uncertainty=fmu_uncertainty*1E-3*9.81 #grams to Newtons
real_uncertainty=real_uncertainty*1E-3*9.81 #grams to Newtons


# def f(x,a,b):
# 	return a*x+b

def f(x,a):
	return a*x

popt, pcov = curve_fit(f, fmu_weight, real_weight)
print('curve_fit: a=%5.5E, b=%5.5E' % tuple(popt))

#%% Plotting

plt.plot(fmu_weight, real_weight, 'o', label="data")
plt.plot(fmu_weight, f(fmu_weight, *popt), 'r-', label='curve_fit: a=%5.5E, b=%5.5E' % tuple(popt))


plt.errorbar(fmu_weight, real_weight, yerr=real_uncertainty, xerr= fmu_uncertainty, fmt="none")

plt.xlabel('FMU Values (N)')
plt.ylabel('Real Values (N)')
plt.legend()
plt.show()

#%%
plt.savefig("./calibration/calibration_curvefit_"+str(filename)+".png")

# print('curve_fit: a=%5.5E' % tuple(popt))


#%%
