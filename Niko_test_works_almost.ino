 //Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
//Receiver

#include <ezButton.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define PS2 joystick pins
int joyX = A0;     // Connect VRx (X-axis) to A0
int joyY = A1;     // Connect VRy (Y-axis) to A1
int joySW = 5;     // Connect SW (switch) to D5

// Define motor control pins for Motor 1

#define motor1IN1  3
#define motor1IN2  4

#define motor2IN1  7
#define motor2IN2  8

#define CE 8
#define CSN 9

/*
int motor1IN1 = 3;  // Connect to IN1 on the motor driver for Motor 1
int motor1IN2 = 4;  // Connect to IN2 on the motor driver for Motor 1

// Define motor control pins for Motor 2
int motor2IN1 = 7;  // Connect to IN3 on the motor driver for Motor 2
int motor2IN2 = 8;  // Connect to IN4 on the motor driver for Motor 2
*/

RF24 radio(CE, CSN); // CE, CSN
const byte address[6] = "00001";

char receivedData[32] = "";

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set joystick switch pin as input
  pinMode(joySW, INPUT_PULLUP);

  // Set motor control pins as outputs
  pinMode(motor1IN1, OUTPUT);
  pinMode(motor1IN2, OUTPUT);
  pinMode(motor2IN1, OUTPUT);
  pinMode(motor2IN2, OUTPUT);

  // Set the motors to sleep mode (active low)
  digitalWrite(2, LOW);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Print initial message
  Serial.println("Joystick Motor Control - Ready");
}

void loop() {
  // Read joystick values
  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);

  // Map joystick values to motor speed range (-255 to 255)
  int motorSpeed1 = map(xValue, 0, 1023, -255, 255);
  int motorSpeed2 = map(yValue, 0, 1023, -255, 255);


  // Read joystick switch state
  int switchState = digitalRead(joySW);

  // Print joystick values
  Serial.print("X-axis: ");
  Serial.print(xValue);
  Serial.print("\tY-axis: ");
  Serial.print(yValue);
  Serial.print("\tSwitch: ");
  Serial.println(switchState);

  if (radio.available()) {   // If the NRF240L01 module received data
    radio.read(&receivedData, sizeof(receivedData)); // Read the data and put it into character array
    xValue = atoi(&receivedData[0]); // Convert the data from the character array (received X value) into integer
    delay(10);
    radio.read(&receivedData, sizeof(receivedData));
    yValue = atoi(&receivedData[0]);
    delay(10);
  }


  // Determine motor direction based on joystick position
  if (switchState == LOW) {
    // Joystick switch pressed, stop motors
    Serial.println("Joystick Switch Pressed - Motors Stopped");
    motorStop();
  } else {
    // Joystick switch not pressed, control motors based on joystick position
    if ((yValue > 600) && (450 < xValue < 550))  {
      // Joystick moved forward, drive both motors forward
      Serial.println("Joystick Moved Backward");
      motorBackward(motorSpeed1, motorSpeed2);

    } else if ((yValue < 100) && (450 < xValue < 550)){
      // Joystick moved backward, drive both motors backward
      Serial.println("Joystick Moved Forward");
      motorForward(motorSpeed1, motorSpeed2);

    } else if ((yValue < 550) && (xValue > 900)){
      // Joystick moved right, drive both motors to the right
      Serial.println("Joystick Moved Right");
      motorTurnRight(motorSpeed1, motorSpeed2);

    } else if ((yValue < 550) && (xValue < 100)){
      // Joystick moved left, drive both motors to the left
      Serial.println("Joystick Moved Left");
      motorTurnLeft(motorSpeed1, motorSpeed2);

    } else {
      // Joystick in neutral position, stop motors
      Serial.println("Joystick in Neutral Position - Motors Stopped");
      motorStop();
    }
  }

  delay(50);  // Add a delay for stability
}

// Function to drive both motors forward
void motorForward(int speed1, int speed2) {
  Serial.println("Driving Forward");
  digitalWrite(motor1IN1, HIGH); //SPEED1
  digitalWrite(motor1IN2, LOW); //0
  digitalWrite(motor2IN1, HIGH); // SPEED2
  digitalWrite(motor2IN2, LOW); //0 all with analogWrite
}

// Function to drive both motors backward
void motorBackward(int speed1, int speed2) {
  Serial.println("Driving Backward");
  digitalWrite(motor1IN1, LOW);
  digitalWrite(motor1IN2, HIGH);
  digitalWrite(motor2IN1, LOW);
  digitalWrite(motor2IN2, HIGH);
}

// Function to turn left (Motor 2 forward, Motor 1 backward)
void motorTurnLeft(int speed1, int speed2) {
  Serial.println("Turning Left");
  digitalWrite(motor1IN1, LOW);
  digitalWrite(motor1IN2, HIGH);
  digitalWrite(motor2IN1, HIGH);
  digitalWrite(motor2IN2, LOW);
}

// Function to turn right (Motor 1 forward, Motor 2 backward)
void motorTurnRight(int speed1, int speed2) {
  Serial.println("Turning Right");
  digitalWrite(motor1IN1, HIGH);
  digitalWrite(motor1IN2, LOW);
  digitalWrite(motor2IN1, LOW);
  digitalWrite(motor2IN2, HIGH);
}

// Function to stop both motors
void motorStop() {
  Serial.println("Motors Stopped");
  digitalWrite(motor1IN1, LOW);
  digitalWrite(motor1IN2, LOW);
  digitalWrite(motor2IN1, LOW);
  digitalWrite(motor2IN2, LOW);
}
