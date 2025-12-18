// 2 Buttons, 2 LEDs

const int BTN1 = 39;
const int BTN2 = 40;
const int LED1 = 4;
const int LED2 = 15;

void setup() 
{
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
  if (Serial.available()) 
  {
    char c = Serial.read();
    
    if (c == 'R') // Request
    {
      Serial.print(!digitalRead(BTN1));
      Serial.print(!digitalRead(BTN2));
      Serial.println();
    }
    else if (c == '1') digitalWrite(LED1, HIGH);
    else if (c == '2') digitalWrite(LED2, HIGH);
    else if (c == '0') 
    {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
    }
  }
}
