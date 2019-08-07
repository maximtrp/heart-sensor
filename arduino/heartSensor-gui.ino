#define PIN 0
#define DT 10
#define LED 13
#define CYCLELIM 350
#define THRES 550

boolean gap = false;
unsigned char serialInt = 0;
unsigned short rawPeak = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
}

void process(int value) {
  if (value >= THRES && value > rawPeak) {
    rawPeak = value;
  }
  else if (value >= THRES && value < rawPeak) {
    if (!gap) {
      digitalWrite(LED, HIGH);
      gap = true;
    }
  }
  else if (value < THRES) {
    rawPeak = 0;
    gap = false;
    digitalWrite(LED, LOW);
  }
}


void loop() {

  /* Receive data from analog sensor */
  unsigned short rawValue = analogRead(PIN);
  process(rawValue);

  /* Send data using serial port */
  if (Serial.available() > 0) {
    serialInt = Serial.read();
  }
  if (serialInt == 49) Serial.println(rawValue); 


  delay(DT);
}
