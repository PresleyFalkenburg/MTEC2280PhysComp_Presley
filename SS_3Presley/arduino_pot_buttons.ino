

// Pin definitions
const int potPin = A0;     
const int button1Pin = 14;  
const int button2Pin = 27;  


int potValue = 0;      
int button1State = 0;  
int button2State = 0;   

int lastTime = 0;
int currentTime = 0;
int timerInterval = 20; /
void setup() 
{
  Serial.begin(9600);         // Start serial communication @ 9600 baud rate
  analogReadResolution(8);    // Adjust ADC read range to 0-255, 1-byte, 8-bit
  
  // Set button pins as inputs with internal pull-up resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
}

void loop() 
{
  // Use timer to control serial update rate
  currentTime = millis(); // Read current elapsed time
  
  if (currentTime - lastTime >= timerInterval)  // If we have reached our timer interval...
  {
    lastTime = currentTime; // Store current time as last time
    
    // Read all sensors
    potValue = analogRead(potPin);          // Read potentiometer (0-255)
    button1State = digitalRead(button1Pin); // Read button 1 (LOW when pressed due to pull-up)
    button2State = digitalRead(button2Pin); // Read button 2 (LOW when pressed due to pull-up)
    
   
    button1State = !button1State;
    button2State = !button2State;
    
 
    Serial.print(potValue);
    Serial.print(",");
    Serial.print(button1State);
    Serial.print(",");
    Serial.println(button2State);
    
  
  }
  
  
}
