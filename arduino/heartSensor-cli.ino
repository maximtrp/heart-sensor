#define PIN 0
#define LED 13
#define DT 200
#define THRES 550

unsigned short serialInt = 0;
unsigned short rawValue = 0;
unsigned long timeMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
}

void process(int value) {
  if (value > THRES) digitalWrite(LED, HIGH);
  else digitalWrite(LED, LOW);
}

void loop() {

  /* Receive data from analog sensor */
  rawValue = analogRead(PIN);
  process(rawValue);
  timeMs = millis();

  /* Send data using serial port */
  if (Serial.available() > 0 && serialInt == 0) {
    serialInt = Serial.read();
  }
  
  if (serialInt == 49) {
    Serial.println(String(timeMs) + " " + String(rawValue));
  }

  delayMicroseconds(DT);
}
