#include <U8g2lib.h>

#define PIN 0
#define DT 10
#define LED 13
#define CYCLELIM 350
#define THRES 550

boolean gap = false;
unsigned char heartBeats = 0;
unsigned char serialInt = 0;
unsigned short rawPeak = 0;
unsigned short cycles = 0;
float heartRate = 0;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  u8g2.begin();
  u8g2.setFont(u8g2_font_logisoso62_tn);
}

void process(int value) {
  if (value >= THRES && value > rawPeak) {
    rawPeak = value;
  }
  else if (value >= THRES && value < rawPeak) {
    if (!gap) {
      digitalWrite(LED, HIGH);
      heartBeats++;
      heartRate = heartBeats * 60.0 / (cycles * DT / 1000.0);
      gap = true;
    }
  }
  else if (value < THRES) {
    rawPeak = 0;
    gap = false;
    digitalWrite(LED, LOW);
  }
}

void outputRate() {
  char HeartRateStr[3];
  sprintf(HeartRateStr, "%d", (unsigned short)(floor(heartRate)));
  unsigned short strWidth = u8g2.getStrWidth(HeartRateStr);
  unsigned char height = 62;
  u8g2.clearBuffer();
  u8g2.drawStr((128 - strWidth) / 2, height, HeartRateStr);
  u8g2.sendBuffer();
}

void loop() {

  /* Receive data from analog sensor */
  unsigned short rawValue = analogRead(PIN);
  process(rawValue);
  cycles++;

  /* Send data using serial port */
  if (Serial.available() > 0) {
    serialInt = Serial.read();
  }
  if (serialInt == 49) Serial.println(rawValue); 

  if (cycles % 70 == 0) outputRate();

  /* Reset settings on cycles end */
  if (cycles > CYCLELIM) {
    cycles = 1, heartBeats = 0, heartRate = 0; 
  }

  delay(DT);
}
