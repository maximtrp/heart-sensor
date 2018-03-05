#!/usr/bin/python

import matplotlib as mpl
mpl.use('Qt5Agg')
import peakutils as pu
import serial
import matplotlib.pyplot as plt
import numpy as np
import time

delay_time = 40.

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(0, 0, 'k-')
plt.ylim(0,1000)
plt.xlim(0,400)

srl = serial.Serial("/dev/ttyUSB0", baudrate = 115200, timeout = 0)
time.sleep(2)
srl.write(b'1')
srl.reset_input_buffer()
data = ''

fig.canvas.draw()

while len(data) < 2000:
    data += srl.read(7).decode('cp1251')
    y = list(map(int, data.split('\r\n')[:-1]))
    x = range(len(y))

    line.set_xdata(x)
    line.set_ydata(y)

    ax.draw_artist(line)

    fig.canvas.update()
    fig.canvas.flush_events()
    
srl.write(b'0')
srl.close()
y = np.array(y, dtype = np.int)
x_peaks = pu.indexes(y, min_dist=10)
y_peaks = y[x_peaks]
heart_rate = round(x_peaks.size * 60 / (len(y) * delay_time / 1000), 0)
np.savetxt('data.txt', y)
np.savetxt('peaks.txt', np.stack((x_peaks, y_peaks)).T)
print(heart_rate)
