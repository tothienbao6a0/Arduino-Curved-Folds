
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 20;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 2, 4, 6, 7);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(90);
  // initialize the serial port:
  Serial.begin(9600);

    Serial.println("Folding");
  myStepper.step(12);
  delay(2000);

  Serial.println("First Actuation Position");
  myStepper.step(-4);
  delay(500);
}

void loop() {
  
  // 
  Serial.println("Right");
  myStepper.step(8);
  delay(500);
  
  // step one revolution in the other direction:
  Serial.println("Left]
  ");
  myStepper.step(-8);
  delay(500);
}
