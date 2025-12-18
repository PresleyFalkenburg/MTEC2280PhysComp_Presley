

let serial;
let portName = '/dev/tty.usbserial-1110';

let btn1 = false;
let btn2 = false;
let mode = 1;
let x = 300;
let y = 300;

function setup() 
{
  createCanvas(600, 600);
  serial = new p5.SerialPort();
  serial.on('data', getData);
  serial.open(portName, { baudRate: 9600 });
}

function draw() 
{
  background(220);
  
  serial.write('R');
  
  // Update
  if (btn1) 
  {
    mode = 1;
    x = random(100, 500);
  }
  if (btn2) 
  {
    mode = 2;
    y = random(100, 500);
  }
  
  // Draw
  if (mode === 1)
  {
    fill(100, 200, 255);
    circle(x, y, 100);
  }
  else
  {
    fill(255, 100, 100);
    rect(x - 50, y - 50, 100, 100);
  }
  
  fill(0);
  textSize(20);
  text("Mode: " + mode, 20, 30);
  
  serial.write(mode === 1 ? '1' : '2');
}

function getData()
{
  let d = serial.readStringUntil('\n');
  if (d)
  {
    btn1 = (d.charAt(0) === '1');
    btn2 = (d.charAt(1) === '1');
  }
}
