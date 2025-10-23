#include <Arduino.h>

// Pin definitions for 2 RGB LEDs
// LED 1
const int LED1_RED = 4;
const int LED1_GREEN = 5;
const int LED1_BLUE = 18;

// LED 2
const int LED2_RED = 19;
const int LED2_GREEN = 21;
const int LED2_BLUE = 22;

// Potentiometer pins - using reliable ADC pins
const int POT_RED = 33;    // Controls Red intensity - ADC1_CH5
const int POT_GREEN = 32;  // Controls Green intensity - ADC1_CH4
const int POT_BLUE = 35;   // Controls Blue intensity - ADC1_CH7

// Laser sensor pin
const int LASER_SENSOR = 26;  // HW-493 laser sensor input

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nESP32 Two LED RGB Controller - Inverse Control");
  
  // Initialize LED pins as outputs
  pinMode(LED1_RED, OUTPUT);
  pinMode(LED1_GREEN, OUTPUT);
  pinMode(LED1_BLUE, OUTPUT);
  
  pinMode(LED2_RED, OUTPUT);
  pinMode(LED2_GREEN, OUTPUT);
  pinMode(LED2_BLUE, OUTPUT);
  
  // Initialize potentiometer pins as inputs
  pinMode(POT_RED, INPUT);
  pinMode(POT_GREEN, INPUT);
  pinMode(POT_BLUE, INPUT);
  
  // Initialize laser sensor pin as input with pullup
  pinMode(LASER_SENSOR, INPUT_PULLUP);
  
  Serial.println("Pot 1 (GPIO33): Red intensity (inverse between LEDs)");
  Serial.println("Pot 2 (GPIO32): Green intensity (inverse between LEDs)");
  Serial.println("Pot 3 (GPIO35): Blue intensity (inverse between LEDs)");
  Serial.println("Laser Sensor (GPIO26): Turns off LEDs when triggered");
  Serial.println("\nReading potentiometer values...\n");
}

void loop() {
  // Read laser sensor (LOW when beam is broken/triggered)
  int laserState = digitalRead(LASER_SENSOR);
  
  // Read potentiometer values (0-4095 for ESP32 12-bit ADC)
  int potRedValue = analogRead(POT_RED);
  int potGreenValue = analogRead(POT_GREEN);
  int potBlueValue = analogRead(POT_BLUE);
  
  // Debug: Print raw ADC values and laser state every second
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.print("Raw ADC - Red: ");
    Serial.print(potRedValue);
    Serial.print(" | Green: ");
    Serial.print(potGreenValue);
    Serial.print(" | Blue: ");
    Serial.print(potBlueValue);
    Serial.print(" | Laser: ");
    Serial.println(laserState == LOW ? "TRIGGERED (OFF)" : "Normal (ON)");
    lastPrint = millis();
  }
  
  // If laser is triggered (LOW), turn off all LEDs
  if (laserState == LOW) {
    // Turn off all LEDs
    analogWrite(LED1_RED, 0);
    analogWrite(LED1_GREEN, 0);
    analogWrite(LED1_BLUE, 0);
    
    analogWrite(LED2_RED, 0);
    analogWrite(LED2_GREEN, 0);
    analogWrite(LED2_BLUE, 0);
  } else {
    // Laser not triggered - normal operation
    // Map potentiometer values to 0-255 for PWM
    int led1Red = map(potRedValue, 0, 4095, 0, 255);
    int led1Green = map(potGreenValue, 0, 4095, 0, 255);
    int led1Blue = map(potBlueValue, 0, 4095, 0, 255);
    
    // Calculate inverse values for LED 2
    int led2Red = 255 - led1Red;
    int led2Green = 255 - led1Green;
    int led2Blue = 255 - led1Blue;
    
    // Set LED 1 colors
    analogWrite(LED1_RED, led1Red);
    analogWrite(LED1_GREEN, led1Green);
    analogWrite(LED1_BLUE, led1Blue);
    
    // Set LED 2 colors (inverse)
    analogWrite(LED2_RED, led2Red);
    analogWrite(LED2_GREEN, led2Green);
    analogWrite(LED2_BLUE, led2Blue);
  }
  
  delay(10);
}