#include <Servo.h>
#include <Arduino.h>
Servo myservo;
//LOOK AT THE NOTES DR.SUNG WROTE IN THE NOTEBOOK FOR STATE MACHINE!!
int FOLDED_POSITION = 300;
int NEUTRAL_POSITION = 8;
const int DIGITAL_PIN = 9;
const int analogOutPin = A3;
const int clockwiseSpeed = 20;
const int counterclockwiseSpeed = 180;
const int stopSpeed = 100;

boolean actuationCompleted=false;
int moves=0;
enum enumerator {
  NEUTRALIZING,
  FOLDING,
  FOLDED,
  LEFT,
  RIGHT,
  DONE_MOVING,
  DONE
};

enumerator stage = NEUTRALIZING;



void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(DIGITAL_PIN);
  delay(2000);
  enumerator stage = NEUTRALIZING;

}

void loop()
{
  // put your main code here, to run repeatedly:
  if (stage == NEUTRALIZING)
  {
    if (analogRead(analogOutPin) != NEUTRAL_POSITION - 8 )
    {
      if (analogRead(analogOutPin) <= 450)
        myservo.write(clockwiseSpeed);
      else
        myservo.write(counterclockwiseSpeed);
      //          returnToUnwind();
    }
    else
    {
      myservo.write(100);
      Serial.println("Neutralized");
      Serial.print("\t Analog at NEUTRAL = ");
      Serial.println(analogRead(analogOutPin));
      delay(1000);
      stage = FOLDING;
    }
  }

  if (stage == FOLDING)
  {
    if (analogRead(analogOutPin) != FOLDED_POSITION - 50)
    {
      myservo.write(counterclockwiseSpeed);

    }

    if (analogRead(analogOutPin) > FOLDED_POSITION) //THIS MIGHT BE AN ISSUE
    {
      myservo.write(clockwiseSpeed);
    }

    if (analogRead(analogOutPin) == FOLDED_POSITION)
    {
      myservo.write(100);
      Serial.println("Folded");
      Serial.print("\t Analog at Folded= ");
      Serial.println(analogRead(analogOutPin));
      delay(2000);
      stage = FOLDED;
    }
  }

  if (stage == FOLDED)
  {
    delay(2000);
    stage = LEFT;
    Serial.println("Start Actuating");

  }

  if (stage == LEFT)
  {
    while(analogRead(analogOutPin)>=200){
    myservo.write(clockwiseSpeed);
    }
    myservo.write(NEUTRAL_POSITION);
    //delay(350);
    moves++;
    Serial.println("Moved Left");
    if (actuationCompleted == false) //tell it to move right or stop moving
      stage = RIGHT;
    else
      stage = DONE_MOVING;
  }

  if (stage == RIGHT)
  {
    while(analogRead(analogOutPin)<=400)
    {
    myservo.write(counterclockwiseSpeed);
    }
    myservo.write(NEUTRAL_POSITION);
    //delay(350);
    moves++;
    Serial.println("Moved Right");
    if (actuationCompleted == false) //tell it to move left or stop moving
      stage = LEFT;
    else
      stage = DONE_MOVING;
  }

  if (stage == DONE_MOVING)
  {
    stage =DONE;  //ADD CODE HERE!!!!

  }
  if (stage == DONE)
  {
    myservo.write(stopSpeed);
    moves=0;
    Serial.println("Process Done!");

  }

  if (moves>50)
  {

    actuationCompleted=true;
   
  }
  else if (moves<=50)
  actuationCompleted=false;
}
