# heart-sensor

This is a small Arduino project based on a simple heart rate sensor.

## Build setup

* Hardware:
	* Arduino UNO (or any other)
	* Heart rate sensor
  * OLED display (optional)

## Wiring

### Heart rate sensor (HRS):

HRS pin | Arduino pin
--- | ---
\+ | +5V
\- | GND
S | A1

### OLED display (optional):

OLED pin | Arduino pin
--- | ---
VDD | +5V
GND | GND
SDA | A4
SCK | A5

## Running

1. Download and install [U8g2](https://github.com/olikraus/u8g2/wiki) library in Arduino IDE: Sketch (Alt + S) → Include Library → Manage Libraries.

2. Connect all components to Arduino according to the above instructions.

3. Connect Arduino via USB, compile and upload the sketch using Arduino IDE.

## Reading data

1. Install all required Python packages with:

```
$ cd python-cli
$ pip install -r requirements.txt
```

2. Put your finger on a heart rate sensor and run the script:

```
python getData-cli.py
```

3. The dataset will be saved into this directory.

## Processing data

1. Run Jupyter Notebook (or Lab) in this project directory:

```
$ jupyter lab
```

2. Open `HRV analysis.ipynb` file and set `path` variable to a dir with your
dataset. You may wish to create `data` folder. In my case, it is `data/Me/`.

3. Run the cells to see the results.

