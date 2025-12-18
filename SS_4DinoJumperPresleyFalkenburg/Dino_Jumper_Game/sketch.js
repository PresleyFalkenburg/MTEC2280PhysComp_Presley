/*
  DINO JUMPER - SIMPLIFIED VERSION
  
  Minimal code, basic shapes, simple logic.
  
  CONTROLS:
  - Jump Button: Jump over obstacles
  - Crouch Button: Duck under flying objects
*/

let serial;
let portName = '/dev/tty.usbserial-1110'; // CHANGE THIS
let options = { baudRate: 9600 };

// Button states
let jumpPressed = false;
let crouchPressed = false;

// Dino
let dinoX = 100;
let dinoY = 480;
let dinoSize = 50;
let dinoVelY = 0;
let isJumping = false;

// Ground
let groundY = 530;

// Obstacles array
let obstacles = [];
let spawnTimer = 0;

// Game
let score = 0;
let gameOver = false;
let speed = 4;

function setup() 
{
  createCanvas(1080, 720);
  
  // Serial
  serial = new p5.SerialPort();
  serial.on('open', portOpen);
  serial.on('data', serialEvent);
  serial.open(portName, options);
}

function draw() 
{
  background(255);
  
  if (!gameOver)
  {
    // Request button states
    serial.write('R');
    
    // Update
    updateDino();
    updateObstacles();
    checkHit();
    
    // Send score
    serial.write(score + '\n');
  }
  
  // Draw
  drawGround();
  drawDino();
  drawObstacles();
  drawUI();
  
  if (gameOver) drawGameOver();
}

function updateDino()
{
  // Jump
  if (jumpPressed && !isJumping)
  {
    dinoVelY = -15;
    isJumping = true;
  }
  
  // Gravity
  dinoVelY += 0.8;
  dinoY += dinoVelY;
  
  // Land
  if (dinoY >= groundY - dinoSize/2)
  {
    dinoY = groundY - dinoSize/2;
    dinoVelY = 0;
    isJumping = false;
  }
}

function drawDino()
{
  fill(0);
  noStroke();
  
  if (crouchPressed && !isJumping)
  {
    // Crouch: wide rectangle
    rect(dinoX - 30, dinoY - 15, 60, 30);
  }
  else
  {
    // Stand: square + circle head
    rect(dinoX - dinoSize/2, dinoY - dinoSize/2, dinoSize, dinoSize);
    circle(dinoX, dinoY - dinoSize/2 - 15, 25);
  }
}

function updateObstacles()
{
  // Spawn
  spawnTimer++;
  if (spawnTimer > 80)
  {
    // 66% cactus, 33% flying
    let type = (random() < 0.66) ? 'cactus' : 'fly';
    obstacles.push({ x: width, type: type, passed: false });
    spawnTimer = 0;
  }
  
  // Move
  for (let i = obstacles.length - 1; i >= 0; i--)
  {
    obstacles[i].x -= speed;
    
    // Score
    if (!obstacles[i].passed && obstacles[i].x < dinoX)
    {
      obstacles[i].passed = true;
      score++;
      
      // Speed up every 10 points
      if (score % 10 === 0) speed += 0.5;
    }
    
    // Remove
    if (obstacles[i].x < -50) obstacles.splice(i, 1);
  }
}

function drawObstacles()
{
  fill(0);
  noStroke();
  
  for (let obs of obstacles)
  {
    if (obs.type === 'cactus')
    {
      // Cactus: tall rectangle
      rect(obs.x, groundY - 50, 25, 50);
    }
    else
    {
      // Flying: circle at head height
      circle(obs.x, 450, 30);
    }
  }
}

function checkHit()
{
  for (let obs of obstacles)
  {
    let hit = false;
    
    if (obs.type === 'cactus')
    {
      // Check ground obstacle
      if (abs(dinoX - obs.x) < 40 && dinoY > groundY - 60)
      {
        hit = true;
      }
    }
    else
    {
      // Check flying obstacle (crouch to avoid)
      if (abs(dinoX - obs.x) < 40 && !crouchPressed)
      {
        hit = true;
      }
    }
    
    if (hit) gameOver = true;
  }
}

function drawGround()
{
  // Ground line
  stroke(0);
  strokeWeight(3);
  line(0, groundY, width, groundY);
}

function drawUI()
{
  fill(0);
  noStroke();
  textSize(24);
  textAlign(LEFT);
  text("SCORE: " + score, 20, 40);
  text("SPEED: " + speed.toFixed(1), 20, 70);
}

function drawGameOver()
{
  fill(0, 200);
  rect(0, 0, width, height);
  
  fill(255);
  textAlign(CENTER);
  textSize(48);
  text("GAME OVER", width/2, height/2);
  textSize(24);
  text("Score: " + score, width/2, height/2 + 40);
  text("Press any key", width/2, height/2 + 80);
}

function serialEvent()
{
  let data = serial.readStringUntil('\n');
  if (data)
  {
    data = data.trim();
    if (data.startsWith('J'))
    {
      jumpPressed = (data.charAt(1) === '1');
      let c = data.indexOf('C');
      if (c !== -1) crouchPressed = (data.charAt(c + 1) === '1');
    }
  }
}

function keyPressed()
{
  if (gameOver)
  {
    gameOver = false;
    score = 0;
    obstacles = [];
    speed = 4;
    dinoY = groundY - dinoSize/2;
    dinoVelY = 0;
  }
}

function portOpen() 
{
  print("PORT OPEN");
}
