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

delay_time = 40.
data = ''

sb.set(font_scale=1.5)
fig, ax = plt.subplots()

line, = ax.plot(0, 0, 'k-', lw=1)
plt.ylim(0,1000)
plt.xlim(0,1000)

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
    data += srl.read(7).decode('cp1251')
    y = list(map(int, data.split('\r\n')[:-1]))
    x = range(len(y))

    line.set_xdata(x)
    line.set_ydata(y)

    ax.set_xlim(len(x) // 1000 * 1000, ((len(x)//1000)+1) * 1000)
    return line,

ani = animation.FuncAnimation(fig, animate, init_func=init, interval=40, blit=True, frames=7500, repeat=False)
plt.show()

srl.write(b'0')
srl.close()
y = np.array(line.get_ydata(), dtype = np.int)
x_peaks = pu.indexes(y, min_dist=10)
y_peaks = y[x_peaks]
heart_rate = round(x_peaks.size * 60 / (len(y) * delay_time / 1000), 0)

x = np.arange(y.size) * delay_time
np.savetxt(filename_date, np.stack((x, y)).T)
np.savetxt(filename_peaks, np.stack((x_peaks * delay_time, y_peaks)).T)
print("Heart rate is", heart_rate)
