#include <Servo.h>
#include <Arduino.h>
Servo myservo;
//LOOK AT THE NOTES DR.SUNG WROTE IN THE NOTEBOOK FOR STATE MACHINE!!
int FOLDED_POSITION = 300;
int NEUTRAL_POSITION = 0;
int LEFT_POSITION = 150;
int RIGHT_POSITION = 350;
const int DIGITAL_PIN = 9;
const int analogOutPin = A3;
const int clockwiseSpeed = 0;
const int counterclockwiseSpeed = 200;
const int stopSpeed = 100;
int POS_RECORD_1 = 0;
int POS_RECORD_2 = 0;
int POS_RECORD_3 = 0;
int POS_PRECISE = 0;
boolean actuationCompleted = false;
int moves = 0;
enum enumerator {
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
  enumerator stage = NEUTRALIZING;
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
      moveToPosition(NEUTRAL_POSITION , "counterclockwise",0,40);
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

  if (stage == FOLDING)
  {

    moveToPosition(FOLDED_POSITION - 50, "counterclockwise",50,50);
    myservo.write(stopSpeed);
    Serial.println("Folded");
    Serial.print("\t Analog at Folded= ");
    Serial.println(analogRead(analogOutPin));
    //delay(5000);
    stage = FOLDED;
  }

  if (stage == FOLDED)
  {
    //delay(2000);
    stage = LEFT;
    Serial.println("Start Actuating");

  }
  
  if (stage == LEFT)
  {


    moveToPosition(LEFT_POSITION, "clockwise",70,55);
    myservo.write(stopSpeed);
    Serial.println("moved Left");
    moves++;
    if (moves >= 10 && moves <=20)
    {
      moveToPosition(FOLDED_POSITION-50, "counterclockwise", 30,30);
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
    if (moves > 20 && moves <=30)
    {
      moveToPosition(FOLDED_POSITION-50, "clockwise", 30,30);
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
    stage = DONE; //ADD CODE HERE!!!!
    delay(2000);
  }
  if (stage == DONE)
  {
    moveToPosition(NEUTRAL_POSITION, "clockwise",50,50);
    delay(100);
    myservo.write(stopSpeed);
    moves = 0;
    Serial.println("Process Done!");
    delay(5000);
    stage = FOLDING; 
    moves = 0; 
  }

  if (moves > 50)
  {

    actuationCompleted = true;

  }
  else if (moves <= 50)
    actuationCompleted = false;
}


void moveToPosition(int positionValue, String rotationDirection, int errorRange1, int errorRange2)
{
  location = readAnalog();
  while (location < positionValue - errorRange1 || location > positionValue + errorRange2)
  {
    if (rotationDirection.equals("clockwise"))
    {
      //delay(100);
      location = readAnalog();
      myservo.write(clockwiseSpeed);
      Serial.println("ROTATING CLOCKWISE");
    }
    if (rotationDirection.equals("counterclockwise"))
    {
      location = readAnalog();
      myservo.write(counterclockwiseSpeed);
      Serial.println("ROTATING COUNTERCLOCKWISE");
    }
    location = readAnalog();
    //delay(100);
  }
  //delay(100);
  //myservo.write(stopSpeed);

}

int readAnalog() {
  POS_RECORD_1 = analogRead(analogOutPin);
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
  
  return POS_PRECISE;
}