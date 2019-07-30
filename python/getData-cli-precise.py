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
total_length = int(6.5 * 60 * 1000 / delay_time)
#data = np.empty(total_length // delay_time, dtype=np.string_)
data = ''
srl = serial.Serial("/dev/ttyUSB0", baudrate = 115200,
                    timeout = delay_time/1000, dsrdtr = True)

time.sleep(2)

print('Beginning...')
srl.write(b'1')
srl.reset_input_buffer()

for i in tqdm.trange(total_length):
    data += srl.readline().decode('cp1251')

srl.write(b'0')
srl.close()

y_lines = re.split(r'[\r\n]+', data.strip())
xy = list(map(lambda x: list(map(int, x.split(" "))), y_lines))
xy = np.array(xy)
y = xy[:, 1]
x = xy[:, 0]

np.savetxt(filename_date, xy)
