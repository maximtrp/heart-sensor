#!/usr/bin/python

import serial
import numpy as np
import time
import datetime as dt
import re
import tqdm
import sys

filename_date = dt.datetime.now().strftime("%Y-%m-%d %H-%M-%S ECG.txt.gz")

delay_time = 1 # ms
total_length = int(300 * 10**3 / delay_time) # ms
data = ''
srl = serial.Serial("/dev/ttyUSB0", baudrate = 115200,
                    timeout = delay_time/1000)

time.sleep(2)

srl.write(b'1')
srl.reset_input_buffer()

progress_bar = tqdm.tqdm(total=total_length)
n = 0
t1 = 0
t2 = 0
td = 0
while True:
    line = srl.readline().decode('cp1251')
    data += line
    if n % 100000:
        s = re.search(r'[\r\n]*(\d+)', line)
        if s:
            if not t1:
                t1 = int(s.group(0))
            t2 = int(s.group(0))
            if t2 > t1 and t2 - t1 > td:
                progress_bar.update(t2 - t1 - td)
                td = t2 - t1
    if td >= total_length:
        break
    n += 1

srl.write(b'0')
srl.close()

y_lines = re.split(r'[\r\n]+', data.strip())
xy = list(map(lambda x: list(map(int, x.split(" "))), y_lines))
xy = np.array(xy, dtype=np.int)
y = xy[:, 1]
x = xy[:, 0]

np.savetxt(filename_date, xy, fmt='%d')
