/*
  DINO JUMPER - ARDUINO CONTROLLER WITH HANDSHAKING
  
  Implements serial handshaking (call & response) between P5.js and Arduino.
  
  INPUTS (Sensors):
  - Pin 39: Jump Button
  - Pin 40: Crouch Button
  
  OUTPUTS (Actuators - 4 LEDs for binary score):
  - Pin 4:  LED 1 (bit 0 - ones place) - First digit
  - Pin 15: LED 2 (bit 1 - twos place) - Second digit
  - Pin 8:  LED 3 (bit 2 - fours place) - Third digit
  - Pin 10: LED 4 (bit 3 - eights place) - Fourth digit
  
  HANDSHAKING PROTOCOL:
  - P5 sends: 'R' (Request button state)
  - Arduino responds: "J1C0\n" (Jump=1, Crouch=0) or similar
  - P5 sends: score as integer (0-15)
  - Arduino displays score in binary on LEDs
  
  WIRING:
  - Pin 39 → Jump Button → GND (use internal pullup)
  - Pin 40 → Crouch Button → GND (use internal pullup)
  - Pin 4 → 220Ω → LED 1 (First digit) → GND
  - Pin 15 → 220Ω → LED 2 (Second digit) → GND
  - Pin 8 → 220Ω → LED 3 (Third digit) → GND
  - Pin 10 → 220Ω → LED 4 (Fourth digit) → GND
*/

// Button pins (inputs)
const int JUMP_BUTTON = 39;
const int CROUCH_BUTTON = 40;

// LED pins (outputs) - for binary score display
const int LED_BIT0 = 4;   // Ones place (1) - First digit
const int LED_BIT1 = 15;  // Twos place (2) - Second digit
const int LED_BIT2 = 8;   // Fours place (4) - Third digit
const int LED_BIT3 = 10;  // Eights place (8) - Fourth digit

// Button states
int jumpState = 0;
int crouchState = 0;

// Score from P5
int score = 0;

void setup() 
{
  // Setup button pins with internal pullup resistors
  pinMode(JUMP_BUTTON, INPUT_PULLUP);
  pinMode(CROUCH_BUTTON, INPUT_PULLUP);
  
  // Setup LED pins as outputs
  pinMode(LED_BIT0, OUTPUT);
  pinMode(LED_BIT1, OUTPUT);
  pinMode(LED_BIT2, OUTPUT);
  pinMode(LED_BIT3, OUTPUT);
  
  // Start with all LEDs off
  digitalWrite(LED_BIT0, LOW);
  digitalWrite(LED_BIT1, LOW);
  digitalWrite(LED_BIT2, LOW);
  digitalWrite(LED_BIT3, LOW);
  
  // Start serial communication
  Serial.begin(9600);
}

void loop() 
{
  // HANDSHAKING: Wait for request from P5
  if (Serial.available() > 0) 
  {
    char incoming = Serial.read();
    
    // If P5 requests button state with 'R'
    if (incoming == 'R') 
    {
      // Read button states (LOW = pressed because of pullup)
      jumpState = !digitalRead(JUMP_BUTTON);    // Invert for easier logic
      crouchState = !digitalRead(CROUCH_BUTTON); // Invert for easier logic
      
      // RESPONSE: Send button states back to P5
      // Format: "J#C#\n" where # is 1 (pressed) or 0 (not pressed)
      Serial.print("J");
      Serial.print(jumpState);
      Serial.print("C");
      Serial.print(crouchState);
      Serial.println(); // Newline for readStringUntil()
    }
    // If P5 sends a number (score)
    else if (incoming >= '0' && incoming <= '9')
    {
      // Put the character back and parse the integer
      score = incoming - '0'; // Convert ASCII to number
      
      // Read any additional digits if available
      while (Serial.available() > 0)
      {
        char nextChar = Serial.peek();
        if (nextChar >= '0' && nextChar <= '9')
        {
          Serial.read();
          score = score * 10 + (nextChar - '0');
        }
        else
        {
          break;
        }
      }
      
      // Limit score to 0-15 (4-bit display)
      score = constrain(score, 0, 15);
      
      // Display score in binary on LEDs
      displayBinaryScore(score);
    }
  }
}

// Function to display score in binary using 4 LEDs
void displayBinaryScore(int value)
{
  // Bit 0 (ones place - value 1)
  digitalWrite(LED_BIT0, (value & 1) ? HIGH : LOW);
  
  // Bit 1 (twos place - value 2)
  digitalWrite(LED_BIT1, (value & 2) ? HIGH : LOW);
  
  // Bit 2 (fours place - value 4)
  digitalWrite(LED_BIT2, (value & 4) ? HIGH : LOW);
  
  // Bit 3 (eights place - value 8)
  digitalWrite(LED_BIT3, (value & 8) ? HIGH : LOW);
}
