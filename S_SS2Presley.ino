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

// Potentiometer pin
const int potPin = 13;

// Variables for timing
unsigned long lastColorChange = 0;
const unsigned long colorChangeInterval = 1000; // 1 second in milliseconds

// Structure to hold RGB color values
struct Color {
  int red;
  int green;
  int blue;
  String name;
};

// Array of random colors
Color colors[] = {
  {255, 0, 0, "Red"},
  {0, 255, 0, "Green"},
  {0, 0, 255, "Blue"},
  {255, 255, 0, "Yellow"},
  {255, 0, 255, "Magenta"},
  {0, 255, 255, "Cyan"},
  {255, 165, 0, "Orange"},
  {128, 0, 128, "Purple"},
  {255, 192, 203, "Pink"},
  {0, 128, 0, "Dark Green"},
  {255, 255, 255, "White"},
  {75, 0, 130, "Indigo"}
};

int totalColors = sizeof(colors) / sizeof(colors[0]);
int led1ColorIndex = 0;
int led2ColorIndex = 1;
float brightness = 1.0; // Brightness multiplier (0.0 to 1.0)

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Two LED Automatic Color Changer with Brightness Control");
  
  // Initialize LED pins as outputs
  pinMode(LED1_RED, OUTPUT);
  pinMode(LED1_GREEN, OUTPUT);
  pinMode(LED1_BLUE, OUTPUT);
  
  pinMode(LED2_RED, OUTPUT);
  pinMode(LED2_GREEN, OUTPUT);
  pinMode(LED2_BLUE, OUTPUT);
  
  // Initialize potentiometer pin as input
  pinMode(potPin, INPUT);
  
  // Seed random number generator
  randomSeed(analogRead(0));
  
  // Display initial colors
  displayColors();
  
  Serial.println("Colors will automatically cycle every second.");
  Serial.println("Use potentiometer to control brightness.");
}

void loop() {
  // Read potentiometer value and convert to brightness (0.0 to 1.0)
  int potValue = analogRead(potPin);
  brightness = (float)potValue / 4095.0; // ESP32 ADC is 12-bit (0-4095)
  
  // Check if it's time for automatic color change
  unsigned long currentTime = millis();
  if (currentTime - lastColorChange >= colorChangeInterval) {
    // Time for automatic color change
    generateRandomColors();
    displayColors();
    
    Serial.println("Auto color change:");
    Serial.println("LED 1: " + colors[led1ColorIndex].name);
    Serial.println("LED 2: " + colors[led2ColorIndex].name);
    Serial.println("Brightness: " + String((int)(brightness * 100)) + "%");
    
    lastColorChange = currentTime;
  } else {
    // Update brightness even if not changing colors
    displayColors();
  }
  
  delay(10);
}

void generateRandomColors() {
  // Generate two different random color indices
  led1ColorIndex = random(0, totalColors);
  
  // Make sure LED2 gets a different color than LED1
  do {
    led2ColorIndex = random(0, totalColors);
  } while (led2ColorIndex == led1ColorIndex);
}

void displayColors() {
  // Apply brightness to LED 1 colors
  int led1_red_bright = (int)(colors[led1ColorIndex].red * brightness);
  int led1_green_bright = (int)(colors[led1ColorIndex].green * brightness);
  int led1_blue_bright = (int)(colors[led1ColorIndex].blue * brightness);
  
  // Apply brightness to LED 2 colors  
  int led2_red_bright = (int)(colors[led2ColorIndex].red * brightness);
  int led2_green_bright = (int)(colors[led2ColorIndex].green * brightness);
  int led2_blue_bright = (int)(colors[led2ColorIndex].blue * brightness);
  
  // Set LED 1 color with brightness applied
  analogWrite(LED1_RED, led1_red_bright);
  analogWrite(LED1_GREEN, led1_green_bright);
  analogWrite(LED1_BLUE, led1_blue_bright);
  
  // Set LED 2 color with brightness applied
  analogWrite(LED2_RED, led2_red_bright);
  analogWrite(LED2_GREEN, led2_green_bright);
  analogWrite(LED2_BLUE, led2_blue_bright);
}