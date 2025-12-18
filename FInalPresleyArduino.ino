/*
  Controls: 2 buttons (left/right), 2 pots (position/aim)
  Auto-fires continuously, aim pot controls bullet direction
  Score display: 9 LEDs in binary (max 511)
*/

const int potPosPin = 12;
const int potAimPin = 10;
const int btnLeftPin = 13;
const int btnRightPin = 14;

// 9 LEDs for binary score (bit 8 = pin 4 RED, bit 7 = pin 5 YELLOW)
const int ledPins[] = {8, 18, 17, 16, 15, 7, 6, 5, 4};

int score = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);
  analogReadResolution(8);
  analogSetAttenuation(ADC_11db);
  
  pinMode(btnLeftPin, INPUT_PULLUP);
  pinMode(btnRightPin, INPUT_PULLUP);
  
  for (int i = 0; i < 9; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime >= 10) {
    lastTime = currentTime;
    
    int potPos = analogRead(potPosPin);
    int potAim = analogRead(potAimPin);
    int btnLeft = !digitalRead(btnLeftPin);
    int btnRight = !digitalRead(btnRightPin);
    
    // send: position,aim,left,right
    Serial.print(potPos);
    Serial.print(",");
    Serial.print(potAim);
    Serial.print(",");
    Serial.print(btnLeft);
    Serial.print(",");
    Serial.println(btnRight);
  }
  
  if (Serial.available() > 0) {
    score = Serial.parseInt();
    if (score > 511) score = 511;
  }
  
  updateLEDs();
}

void updateLEDs() {
  for (int i = 0; i < 9; i++) {
    digitalWrite(ledPins[i], (score >> i) & 1);
  }
}
