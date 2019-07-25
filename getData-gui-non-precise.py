#!/usr/bin/python

import matplotlib as mpl
mpl.use('Qt5Agg')
#mpl.use('GTK3Agg') 

import peakutils as pu
import serial
import matplotlib.pyplot as plt
import numpy as np
import time
import datetime as dt
import matplotlib.animation as animation
import seaborn as sb

filename_date = dt.datetime.now().strftime("%Y-%m-%d %H-%M-%S ECG.txt")
filename_peaks = dt.datetime.now().strftime("%Y-%m-%d %H-%M-%S peaks.txt")

delay_time = 10.
data = ''

sb.set(font_scale=1.5)
fig, ax = plt.subplots()

line, = ax.plot(0, 0, 'k-', lw=1)
plt.ylim(0,1000)
xlim = 40000/delay_time
plt.xlim(0,xlim)

srl = serial.Serial("/dev/ttyUSB0", baudrate = 115200, timeout = 0)
time.sleep(2)

def init():  # only required for blitting to give a clean slate.
    srl.write(b'1')
    srl.reset_input_buffer()
    line.set_ydata([0])
    line.set_xdata([0])
    return line,


def animate(i):
    global data
    data += srl.readline().decode('cp1251')
    y = list(map(int, data.split('\r\n')[:-1]))
    x = range(len(y))

    line.set_xdata(x)
    line.set_ydata(y)

    ax.set_xlim(len(x) // xlim * xlim, ((len(x)//xlim)+1) * xlim)
    return line,

ani = animation.FuncAnimation(fig, animate, init_func=init, interval=delay_time, blit=True, frames=int(300000/delay_time), repeat=False)
plt.show()

srl.write(b'0')
srl.close()
y = np.array(line.get_ydata(), dtype = np.int)
x_peaks = pu.indexes(y, min_dist=int(350/delay_time), thres_abs=True, thres=(np.median(y) + np.diff(np.quantile(y, [0.25, 0.75]))[0] * 1.5))
#x_peaks = pu.indexes(y, min_dist=int(350/delay_time))
y_peaks = y[x_peaks]
heart_rate = round(x_peaks.size * 60 / (len(y) * delay_time / 1000), 0)

x = np.arange(y.size) * delay_time
np.savetxt(filename_date, np.stack((x, y)).T)
np.savetxt(filename_peaks, np.stack((x_peaks * delay_time, y_peaks)).T)
print("Heart rate is", heart_rate)

