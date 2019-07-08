# heart-sensor

This is a small Arduino project based on a simple heart rate sensor and an OLED display (SSD1306).

## Build setup

* Hardware:
	* Arduino UNO (or any other)
	* Heart rate sensor
  * OLED display

## Wiring

### Heart rate sensor (HRS):

HRS pin | Arduino pin
--- | ---
\+ | +5V
\- | GND
S | A1

### OLED display:

OLED pin | Arduino pin
--- | ---
VDD | +5V
GND | GND
SDA | A4
SCK | A5

## Video

...

## Running

1. Connect all components to Arduino according to the above instructions.

2. Download and install [U8g2](https://github.com/olikraus/u8g2/wiki) library in Arduino IDE: Sketch (Alt + S) → Include Library → Manage Libraries.

3. Connect Arduino via USB, compile and upload the sketch using Arduino IDE.

4. Use `getData.py` script to receive data over serial connection. Additional packages must be installed to run it: [Matplotlib](https://github.com/matplotlib/matplotlib), [Peakutils](https://bitbucket.org/lucashnegri/peakutils), [pySerial](https://github.com/pyserial/pyserial), [NumPy](https://github.com/numpy/numpy).

P.S. Adjust `THRES` macro in `heartSensor.ino` to fit your needs. I've found `550` value to be optimal.
