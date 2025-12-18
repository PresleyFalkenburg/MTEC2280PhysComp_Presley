/*
  SPACE INVADERS - P5.JS
  Serial controlled game 
  Nature of Code techniques (vectors + particles)
  
  Controls
  - Pot 1 (pin 12): up/down position
  - Pot 2 (pin 10): aim direction ,Button left (pin 13): move left ,Button right (pin 14): move right. Autofire continuously
*/

let serial;
let portName = '/dev/tty.usbserial-110';
let options = { baudRate: 9600 };

// control values from arduino
let potPos = 127;
let potAim = 127;
let btnLeft = 0;
let btnRight = 0;

// game objects
let player;
let bullets = [];
let enemies = [];
let particles = [];
let score = 0;
let gameOver = false;

let shootCooldown = 0;

function setup() {
  createCanvas(800, 600);
  
  serial = new p5.SerialPort();
  serial.on('list', printList);
  serial.on('connected', () => print("CONNECTED TO SERVER"));
  serial.on('open', () => print("SERIAL PORT OPEN"));
  serial.on('data', serialEvent);
  serial.on('error', (err) => print('ERROR: ' + err));
  serial.on('close', () => print("PORT CLOSED"));
  serial.list();
  serial.open(portName, options);
  
  player = new Player();
  spawnEnemies();
}

function draw() {
  background(10, 10, 30);
  drawStars();
  
  if (gameOver) {
    if (score >= 511) {
      fill(0, 255, 100);
      textSize(64);
      textAlign(CENTER, CENTER);
      text("YOU WIN!", width/2, height/2);
    } else {
      fill(255, 0, 0);
      textSize(64);
      textAlign(CENTER, CENTER);
      text("GAME OVER", width/2, height/2);
    }
    textSize(32);
    text("Score: " + score, width/2, height/2 + 60);
    return;
  }
  
  // update player from controls
  player.pos.y = map(potPos, 0, 255, height/2, height - 30);
  if (btnLeft) player.pos.x -= 5;
  if (btnRight) player.pos.x += 5;
  player.pos.x = constrain(player.pos.x, 30, width - 30);
  
  // get aim angle from pot (-60 to 60 degrees from vertical)
  let aimAngle = map(potAim, 0, 255, -PI/3, PI/3);
  
  // auto-fire with cooldown
  if (shootCooldown <= 0) {
    bullets.push(new Bullet(player.pos.x, player.pos.y - 20, aimAngle));
    shootCooldown = 20;
  }
  shootCooldown--;
  
  // draw aim line
  stroke(100, 255, 100, 150);
  strokeWeight(2);
  let aimX = player.pos.x + sin(aimAngle) * 50;
  let aimY = player.pos.y - 20 - cos(aimAngle) * 50;
  line(player.pos.x, player.pos.y - 20, aimX, aimY);
  noStroke();
  
  player.draw();
  
  for (let i = bullets.length - 1; i >= 0; i--) {
    bullets[i].update();
    bullets[i].draw();
    if (bullets[i].offscreen()) bullets.splice(i, 1);
  }
  
  let moveDown = false;
  for (let e of enemies) {
    e.update();
    if (e.pos.x < 20 || e.pos.x > width - 20) moveDown = true;
  }
  
  if (moveDown) {
    for (let e of enemies) {
      e.vel.x *= -1;
      e.pos.y += 20;
    }
  }
  
  // collision detection
  for (let i = bullets.length - 1; i >= 0; i--) {
    for (let j = enemies.length - 1; j >= 0; j--) {
      if (p5.Vector.dist(bullets[i].pos, enemies[j].pos) < 20) {
        for (let k = 0; k < 8; k++) {
          particles.push(new Particle(enemies[j].pos.x, enemies[j].pos.y));
        }
        enemies.splice(j, 1);
        bullets.splice(i, 1);
        score += 1;
        if (score >= 511) gameOver = true;
        serial.write(score + "\n");
        break;
      }
    }
  }
  
  for (let e of enemies) {
    e.draw();
    if (e.pos.y > height - 60) gameOver = true;
  }
  
  for (let i = particles.length - 1; i >= 0; i--) {
    particles[i].update();
    particles[i].draw();
    if (particles[i].isDead()) particles.splice(i, 1);
  }
  
  if (enemies.length == 0) spawnEnemies();
  
  fill(255);
  textSize(24);
  textAlign(LEFT, TOP);
  text("SCORE: " + score, 20, 20);
}

function spawnEnemies() {
  enemies = [];
  for (let row = 0; row < 3; row++) {
    for (let col = 0; col < 8; col++) {
      let x = 100 + col * 80;
      let y = 60 + row * 50;
      enemies.push(new Enemy(x, y));
    }
  }
}

function drawStars() {
  randomSeed(42);
  fill(255, 150);
  noStroke();
  for (let i = 0; i < 100; i++) {
    let x = random(width);
    let y = random(height);
    circle(x, y, random(1, 3));
  }
}

class Player {
  constructor() {
    this.pos = createVector(width/2, height - 50);
  }
  
  draw() {
    fill(0, 255, 100);
    noStroke();
    triangle(
      this.pos.x, this.pos.y - 20,
      this.pos.x - 15, this.pos.y + 15,
      this.pos.x + 15, this.pos.y + 15
    );
  }
}

class Bullet {
  constructor(x, y, angle) {
    this.pos = createVector(x, y);
    let speed = 10;
    this.vel = createVector(sin(angle) * speed, -cos(angle) * speed);
  }
  
  update() {
    this.pos.add(this.vel);
  }
  
  draw() {
    fill(255, 255, 0);
    noStroke();
    circle(this.pos.x, this.pos.y, 8);
  }
  
  offscreen() {
    return (this.pos.y < 0 || this.pos.x < 0 || this.pos.x > width);
  }
}

class Enemy {
  constructor(x, y) {
    this.pos = createVector(x, y);
    this.vel = createVector(1, 0);
  }
  
  update() {
    this.pos.add(this.vel);
  }
  
  draw() {
    fill(255, 50, 50);
    noStroke();
    rectMode(CENTER);
    rect(this.pos.x, this.pos.y, 30, 25);
    rectMode(CORNER);
  }
}

class Particle {
  constructor(x, y) {
    this.pos = createVector(x, y);
    this.vel = p5.Vector.random2D().mult(random(2, 5));
    this.acc = createVector(0, 0.1);
    this.lifespan = 255;
  }
  
  update() {
    this.vel.add(this.acc);
    this.pos.add(this.vel);
    this.lifespan -= 8;
  }
  
  draw() {
    fill(255, 200, 50, this.lifespan);
    noStroke();
    circle(this.pos.x, this.pos.y, 6);
  }
  
  isDead() {
    return this.lifespan <= 0;
  }
}

function serialEvent() {
  let data = serial.readLine();
  if (data.length > 0) {
    let vals = data.split(",");
    if (vals.length >= 4) {
      potPos = int(vals[0]);
      potAim = int(vals[1]);
      btnLeft = int(vals[2]);
      btnRight = int(vals[3]);
    }
  }
}

function printList(portList) {
  print("Available Ports:");
  for (let i = 0; i < portList.length; i++) {
    print(i + ": " + portList[i]);
  }
}

// keyboard fallback for testing
function keyPressed() {
  if (keyCode == LEFT_ARROW) btnLeft = 1;
  if (keyCode == RIGHT_ARROW) btnRight = 1;
  if (keyCode == UP_ARROW) potPos = max(0, potPos - 20);
  if (keyCode == DOWN_ARROW) potPos = min(255, potPos + 20);
  if (key == 'a' || key == 'A') potAim = max(0, potAim - 20);
  if (key == 'd' || key == 'D') potAim = min(255, potAim + 20);
}

function keyReleased() {
  if (keyCode == LEFT_ARROW) btnLeft = 0;
  if (keyCode == RIGHT_ARROW) btnRight = 0;
}
