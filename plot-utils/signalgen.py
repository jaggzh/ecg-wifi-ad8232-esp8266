#!/usr/bin/env python3
import numpy as np
import ipdb

def sine_wave_samps(f=None,
                    phase=0,
                    samplerate=None,
                    totsamps=None):
	"""
	Generate sine wave signal
	Parameters:
		f : frequency of sine wave in Hertz
		samplerate: samplerate in Hertz
		phase: phase
		totsamps: total samples for all phases
	Returns:
		(t,g) : time base (t) and the signal g(t) as tuple
	Example:
	"""
	samps_per_cycle = samplerate / f
	print("Samples per cycle :", samps_per_cycle)
	totcycles = totsamps / samps_per_cycle
	print("Total cycles      :", totcycles)
	totcycles_value = totcycles * np.pi*2
	print("Total cycles value:", totcycles_value)
	steps = totsamps
	t = np.arange(0,
	              totcycles_value,
	              totcycles_value/(totsamps)) # time base
	g = np.sin(t+phase) # replace with cos if a cosine wave is desired
	return (t,g) # return time base and signal g(t) as tuple

def test():
	import matplotlib.pyplot as plt
	(t,g) = sine_wave_samps(f=60,
	                        phase=np.pi/2,
	                        samplerate=60*16,
	                        totsamps=16*2+1)
	print("Timebase:")
	print(t)
	print("Signal:")
	print(g)
	plt.plot(t, g)
	plt.show()
	# fn="gp.txt"
	# F = open(fn, "w")
	# for i in range(len(t)):
	# 	print(f"{t[i]}\t{g[i]}")
	# 	F.write(f"{t[i]}\t{g[i]}\n")
	# F.close()
	# print("Wrote", fn)

if __name__ == "__main__": test()

