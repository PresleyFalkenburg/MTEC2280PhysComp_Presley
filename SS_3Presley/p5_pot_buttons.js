

let serial; 
let portName = '/dev/tty.usbserial-0001';  // CHANGE THIS to your serial port name
let options = { baudRate: 9600}; 

// Variables for incoming data
let potValue = 0;      
let button1State = 0;  
let button2State = 0;   
function setup() 
{
  // P5 SerialPort Setup
  serial = new p5.SerialPort();
  serial.on('list', printList);
  serial.on('connected', serverConnected);
  serial.on('open', portOpen);
  serial.on('data', serialEvent);
  serial.on('error', serialError);
  serial.on('close', portClose);
 
  serial.list();
  serial.open(portName, options);

  // P5.js Canvas Setup
  createCanvas(800, 800);
  textSize(32);
  textAlign(CENTER, CENTER);
}

function draw() 
{
  // Background color changes based on button states
  if (button1State && button2State) {
    background(255, 200, 255); 
  } else if (button1State) {
    background(255, 200, 200); 
  } else if (button2State) {
    background(200, 200, 255); 
  } else {
    background(200, 255, 255); 

  // Draw main circle controlled by potentiometer
  push();
  fill(potValue, 255 - potValue, 150); // Color based on pot value
  let diameter = map(potValue, 0, 255, 50, width - 50);
  circle(width/2, height/2, diameter);
  pop();

  // button 1 indicator
  push();
  fill(button1State ? color(255, 0, 0) : color(100)); // red 
  circle(width/4, height - 100, 80);
  fill(255);
  textSize(20);
  text("BTN 1", width/4, height - 100);
  pop();

  // draw button 2 indicator
  push();
  fill(button2State ? color(0, 0, 255) : color(100)); // Blue 
  circle(3*width/4, height - 100, 80);
  fill(255);
  textSize(20);
  text("BTN 2", 3*width/4, height - 100);
  pop();

  // Display data values as text
  push();
  fill(0);
  textSize(24);
  text("Potentiometer: " + potValue, width/2, 50);
  text("Button 1: " + (button1State ? "PRESSED" : "Released"), width/2, 100);
  text("Button 2: " + (button2State ? "PRESSED" : "Released"), width/2, 150);
  pop();
}

function printList(portList) 
{
  print("List of Available Serial Ports: ");
  for (var i = 0; i < portList.length; i++) 
  {
    print(i + " " + portList[i]);
  }
}

function serverConnected() 
{
  print("CONNECTED TO SERVER");
}
 
function portOpen() 
{
  print("SERIAL PORT OPEN");
}
 
function serialEvent() 
{
  // Read incoming string data
  let inString = serial.readLine();
  
  if (inString.length > 0) {
   
    let sensors = split(trim(inString), ',');
    
    
    if (sensors.length >= 3) {
      potValue = Number(sensors[0]);
      button1State = Number(sensors[1]);
      button2State = Number(sensors[2]);
      
      // Debug: print to console
      // print("Pot:", potValue, "Btn1:", button1State, "Btn2:", button2State);
    }
  }
}
 
function serialError(err) 
{
  print('ERROR: ' + err);
}
 
function portClose() 
{
  print("*____SERIAL PORT CLOSED");
}
