#include <Servo.h>
#include <Arduino.h>
Servo myservo;
//digital pin controls the velocity
//analog pin shows the position of the servo 
//therefore, to move from one position to another, the move function must designates the desired analog value, the rotation position, and the error range to help check for accurate movement

int NEUTRAL_POSITION = 45;//position where it is unfolded (can be changed)
int PULL_OFFSET = 110;//how much it actuates in each direction during the folded state
int FOLDED_POSITION = NEUTRAL_POSITION+250;//position where it is folded
int LEFT_POSITION = FOLDED_POSITION-PULL_OFFSET;//position where it is actuating to the left
int RIGHT_POSITION = FOLDED_POSITION+PULL_OFFSET;//position where it is actuating to the right
const int DIGITAL_PIN = 9;
const int analogOutPin = A3; //analog Pin is the pin that shows the position of the servo horn. It is NOT from 0 to 1023, but rather 0 to around 500
const int clockwiseSpeed = 0; //speed input in the digital Pin so that the horn turns clockwise
const int counterclockwiseSpeed = 200; //speed input in the digital Pin so that the horn turns counterclockwise
const int stopSpeed = 100; //speed input in the digital Pin so that the horn stops
int POS_RECORD_1 = 0; //these are the three values that will help the arduino check for the analog position more accurately 
int POS_RECORD_2 = 0;
int POS_RECORD_3 = 0;
int POS_PRECISE = 0; //this is a close estimate for the actual analogValue that will be used to check for position in the code to switch between states
boolean actuationCompleted = false;
int moves = 0;
enum enumerator { //state machine to run through the process. 
  NEUTRALIZING,
  FOLDING,
  FOLDED,
  CENTRALISE, 
  LEFT,
  RIGHT,
  DONE_MOVING,
  DONE
};
int location;
int location2;
enumerator stage = NEUTRALIZING;



void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(DIGITAL_PIN);
  myservo.write(stopSpeed);
  delay(2000);
  enumerator stage = NEUTRALIZING; //beginning state that can be changed to check
  Serial.println("start");


}

void loop()
{
  // put your main code here, to run repeatedly:
  if (stage == NEUTRALIZING)
  {
    location2 = analogRead(analogOutPin);
    Serial.println("entering neutralizing stage");
   /* if (location2 <= 400)
    {
      moveToPosition(10, "clockwise",0,40);
      delay(100);
      myservo.write(100);
      
      //delay(2000);
    }*/
    //else
    {
      moveToPosition(NEUTRAL_POSITION , "counterclockwise",30,30); //always move counter clockwise to the neutral position at the beginning
      delay(100);
      myservo.write(100);
      //delay(2000);
    }
    Serial.println("Neutralized");
    Serial.print("\t Stopped at NEUTRAL analog = ");
    Serial.println(analogRead(analogOutPin));
    delay(4000);
    stage = FOLDING;
    //myservo.write(stopSpeed);

  }

  if (stage == FOLDING) // state the folding state
  {

    moveToPosition(FOLDED_POSITION, "counterclockwise",50,50); //move to the counter clockwise to the folded position from the unfolded position
    myservo.write(stopSpeed);
    Serial.println("Folded");
    Serial.print("\t Analog at Folded= ");
    Serial.println(analogRead(analogOutPin));
    //delay(5000);
    stage = FOLDED;
  }

  if (stage == FOLDED)//if the robot is folded, start actuating
  {
    //delay(2000);
    stage = LEFT;
    Serial.println("Start Actuating");

  }
  
  if (stage == LEFT) 
  {


    moveToPosition(LEFT_POSITION, "clockwise",70,55); //move to left pos
    myservo.write(stopSpeed);
    Serial.println("moved Left");
    moves++;
    if (moves >= 40 && moves <=50) //testing range for left actuation
    {
      moveToPosition(FOLDED_POSITION, "counterclockwise", 30,30);
      myservo.write(stopSpeed);
      Serial.println("movedToCenter");
    }
    //delay(2000);
    else if (actuationCompleted == false) //tell it to move right or stop moving
      stage = RIGHT;
    else
      stage = DONE_MOVING;

  }

  if (stage == RIGHT) 
  {

    moveToPosition(RIGHT_POSITION, "counterclockwise", 55,70);
    myservo.write(stopSpeed);
    Serial.println("moved right");
    moves++;
    
    //delay(2000);
    if (moves > 50 && moves <=60) //testing range for right actuation
    {
      moveToPosition(FOLDED_POSITION, "clockwise", 30,30);
      myservo.write(stopSpeed);
      Serial.println("movedToCenter");
    }
    else if (actuationCompleted == false) //tell it to move right or stop moving
      stage = LEFT;
    else
      stage = DONE_MOVING;
  }

  if (stage == DONE_MOVING)
  {
    stage = DONE; //if done_moving, done
    delay(2000);
  }
  if (stage == DONE)
  {
    moveToPosition(NEUTRAL_POSITION, "clockwise",50,50); //move clockwise to neutral positon to return to unfolded state
    delay(100);
    myservo.write(stopSpeed);
    moves = 0;
    Serial.println("Process Done!");
    delay(5000);
    stage = FOLDING; 
    moves = 0; 
  }

  if (moves > 70) //if moved more than 70 times, return to neutral position
  {

    actuationCompleted = true;

  }
  else if (moves <= 50)
    actuationCompleted = false;
}


void moveToPosition(int positionValue, String rotationDirection, int errorRange1, int errorRange2)
{//this is the main function to move to a designated position as called in the state machine
  //the error range is so that in the case the analog Read is not updated fast enough, the servo horn can still stop around the designated position as noted in the function
  location = readAnalog(); //update location of servo horn using analog reader
  while (location < positionValue - errorRange1 || location > positionValue + errorRange2)
  {
    if (rotationDirection.equals("clockwise"))
    {
      //delay(100);
      location = readAnalog();
      myservo.write(clockwiseSpeed); //if rotating clockwise set digital pin to clockwise value
      Serial.println("ROTATING CLOCKWISE");
    }
    if (rotationDirection.equals("counterclockwise"))
    {
      location = readAnalog();
      myservo.write(counterclockwiseSpeed); //if rotating counterclockwise set digital pin to counterclockwise value
      Serial.println("ROTATING COUNTERCLOCKWISE");
    }
    location = readAnalog();
    //delay(100);
  }
  //delay(100);
  //myservo.write(stopSpeed);

}

int readAnalog() { //this is the function to check and return a more accurate value to get rid of the noise (faulty read values) from the reader. 
  POS_RECORD_1 = analogRead(analogOutPin);//read constantly 3 times
  Serial.println(POS_RECORD_1);
  //delay(5);
  POS_RECORD_2 = analogRead(analogOutPin);
  Serial.println(POS_RECORD_2);
  //delay(5);
  POS_RECORD_3 = analogRead(analogOutPin);
  Serial.println(POS_RECORD_3);
  //delay(5);

  if (POS_RECORD_1 >= POS_RECORD_2 && POS_RECORD_1 >= POS_RECORD_3 && POS_RECORD_1 <= 450)
    POS_PRECISE = POS_RECORD_1;

  else if (POS_RECORD_2 >= POS_RECORD_1 && POS_RECORD_2 >= POS_RECORD_3 && POS_RECORD_2 <= 450)
    POS_PRECISE = POS_RECORD_2;

  else if (POS_RECORD_3 >= POS_RECORD_2 && POS_RECORD_3 >= POS_RECORD_1 && POS_RECORD_3 <= 450)
    POS_PRECISE = POS_RECORD_3;
  Serial.println("analog Captured");
  
  return POS_PRECISE;//return a "precise" (more precise) position
}