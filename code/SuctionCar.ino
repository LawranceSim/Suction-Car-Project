#include <Servo.h>

// --- Main L298N Motor Driver (4 wheels) ---
const int in1 = 5;
const int in2 = 6;
const int in3 = 7;
const int in4 = 11;

// --- Rotator Motor on Second L298N Driver ---
const int rotator_IN1 = 2;
const int rotator_IN2 = 4;

// --- Servo Motors for Suction Arm ---
Servo suction_hor;  // Horizontal (left-right)
Servo suction_ver;  // Vertical (up-down)

const int suctionHorPin = 9;
const int suctionVerPin = 10;

// --- Control Flags ---
bool moveForward = false;
bool moveBackward = false;
bool turnLeft = false;
bool turnRight = false;
bool dcRotate = false;

// --- Suction Control Flags ---
bool isSuctionLeftToggled = false;
bool isSuctionRightToggled = false;
bool isSuctionUpToggled = false;
bool isSuctionDownToggled = false;

// --- Mode Toggle ---
bool suctionControlMode = false;  // false = car mode, true = suction mode

// --- Built-in LED for Mode Indication ---
const int modeLedPin = LED_BUILTIN;

// --- Suction Arm Servo Angles ---
int suctionHorAngle = 90;  // Centered horizontal
int suctionVerAngle = 90;  // Centered vertical

const int angleStep = 5;
const unsigned long servoDelay = 50;  // ms
unsigned long lastServoUpdate = 0;

// --- Motor Speed ---
int Speed_of_car = 204;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(rotator_IN1, OUTPUT);
  pinMode(rotator_IN2, OUTPUT);

  pinMode(modeLedPin, OUTPUT);
  digitalWrite(modeLedPin, LOW);

  suction_hor.attach(suctionHorPin);
  suction_ver.attach(suctionVerPin);
  suction_hor.write(suctionHorAngle);
  suction_ver.write(suctionVerAngle);

  Serial.begin(9600);
}

void loop() {
  if (!suctionControlMode) {
    if (moveForward) forward();
    else if (moveBackward) back();
    else if (turnLeft) left();
    else if (turnRight) right();
    else Stop();
  } else {
    Stop();
  }

  digitalWrite(rotator_IN1, dcRotate ? HIGH : LOW);
  digitalWrite(rotator_IN2, LOW);

  if (millis() - lastServoUpdate > servoDelay) {
    lastServoUpdate = millis();

    if (isSuctionLeftToggled) {
      suctionHorAngle = max(0, suctionHorAngle - angleStep);
      suction_hor.write(suctionHorAngle);
    } else if (isSuctionRightToggled) {
      suctionHorAngle = min(180, suctionHorAngle + angleStep);
      suction_hor.write(suctionHorAngle);
    }

    if (isSuctionUpToggled) {
      suctionVerAngle = max(0, suctionVerAngle - angleStep);
      suction_ver.write(suctionVerAngle);
    } else if (isSuctionDownToggled) {
      suctionVerAngle = min(180, suctionVerAngle + angleStep);
      suction_ver.write(suctionVerAngle);
    }
  }

  if (Serial.available() > 0) {
    char command = (char)Serial.read();
    Serial.println(command);

    if (command == 'U') {
      suctionControlMode = !suctionControlMode;
      digitalWrite(modeLedPin, suctionControlMode ? HIGH : LOW);
      Serial.println(suctionControlMode ? "Suction Mode" : "Car Mode");

      moveForward = moveBackward = turnLeft = turnRight = false;
      isSuctionLeftToggled = isSuctionRightToggled = false;
      isSuctionUpToggled = isSuctionDownToggled = false;
      return;
    }

    if (suctionControlMode) {
      switch (command) {
        case 'F': isSuctionUpToggled = true; break;
        case 'f': isSuctionUpToggled = false; break;
        case 'B': isSuctionDownToggled = true; break;
        case 'b': isSuctionDownToggled = false; break;
        case 'L': isSuctionLeftToggled = true; break;
        case 'l': isSuctionLeftToggled = false; break;
        case 'R': isSuctionRightToggled = true; break;
        case 'r': isSuctionRightToggled = false; break;
        case 'C':
          suctionHorAngle = 90;
          suctionVerAngle = 90;
          suction_hor.write(suctionHorAngle);
          suction_ver.write(suctionVerAngle);
          break;
      }
    } else {
      switch (command) {
        case 'F': moveForward = true; break;
        case 'f': moveForward = false; break;
        case 'B': moveBackward = true; break;
        case 'b': moveBackward = false; break;
        case 'L': turnLeft = true; break;
        case 'l': turnLeft = false; break;
        case 'R': turnRight = true; break;
        case 'r': turnRight = false; break;
        case 'W': dcRotate = true; break;
        case 'w': dcRotate = false; break;
      }
    }
  }
}

void forward() {
  analogWrite(in1, Speed_of_car);
  analogWrite(in2, 0);
  analogWrite(in3, Speed_of_car);
  analogWrite(in4, 0);
}
void back() {
  analogWrite(in1, 0);
  analogWrite(in2, Speed_of_car);
  analogWrite(in3, 0);
  analogWrite(in4, Speed_of_car);
}
void left() {
  analogWrite(in1, 0);
  analogWrite(in2, Speed_of_car);
  analogWrite(in3, Speed_of_car);
  analogWrite(in4, 0);
}
void right() {
  analogWrite(in1, Speed_of_car);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, Speed_of_car);
}
void Stop() {
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);
}
