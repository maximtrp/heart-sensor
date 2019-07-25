#!/usr/bin/python

import serial
import numpy as np
import time
import datetime as dt
import re
import tqdm
import peakutils as pu

filename_date = dt.datetime.now().strftime("%Y-%m-%d %H-%M-%S ECG.txt")
filename_peaks = dt.datetime.now().strftime("%Y-%m-%d %H-%M-%S peaks.txt")

delay_time = 1
total_length = int(5 * 60 * 1000 / delay_time)
#data = np.empty(total_length // delay_time, dtype=np.string_)
data = ''
srl = serial.Serial("/dev/ttyUSB0", baudrate = 115200, timeout = 0)
time.sleep(2)

srl.write(b'1')
srl.reset_input_buffer()

for i in tqdm.trange(total_length):
    s = time.time()
    #n = srl.read(7).decode('cp1251').strip().replace(r'\r','').replace(r'\n','')
    data += srl.readline().decode('cp1251')
    s2 = time.time()
    #time.sleep(max(delay_time/1000, (delay_time/1000) - (s2-s)))
    time.sleep(delay_time/1000)

srl.write(b'0')
srl.close()

y = np.array(list(map(lambda x: '0' if not x else x, re.split(r'\s+', data))), dtype=np.int)
#x_peaks = pu.indexes(y, min_dist=350/delay_time)
x_peaks = pu.indexes(y, min_dist=350, thres_abs=True, thres=(np.median(y) + np.diff(np.quantile(y, [0.25, 0.75]))[0] * 1.5))
y_peaks = y[x_peaks]
heart_rate = round(x_peaks.size * 60 / (len(y) * delay_time / 1000), 0)

x = np.arange(y.size) * delay_time
np.savetxt(filename_date, np.stack((x, y)).T)
np.savetxt(filename_peaks, np.stack((x_peaks * delay_time, y_peaks)).T)
print("Heart rate is", heart_rate)
