#include "enginesControl.h"
#include <Arduino.h> 
#include "pinDefs.h"
#include "directionDefs.h"

//TODO define pins as variables

void forward()
{
  digitalWrite(ENGINE_ONE_PLUS, LOW);
  digitalWrite(ENGINE_ONE_MINUS, HIGH);
  digitalWrite(ENGINE_TWO_PLUS, LOW);
  digitalWrite(ENGINE_TWO_MINUS, HIGH);
}

void back()
{
  digitalWrite(ENGINE_ONE_PLUS, HIGH);
  digitalWrite(ENGINE_ONE_MINUS, LOW);
  digitalWrite(ENGINE_TWO_PLUS, HIGH);
  digitalWrite(ENGINE_TWO_MINUS, LOW);
}
void leftt()
{
  Serial.println("left 4");
  digitalWrite(ENGINE_ONE_PLUS, LOW);
  digitalWrite(ENGINE_ONE_MINUS, HIGH);
  digitalWrite(ENGINE_TWO_PLUS, LOW);
  digitalWrite(ENGINE_TWO_MINUS, LOW);
}
void right()
{
  digitalWrite(ENGINE_ONE_PLUS, LOW);
  digitalWrite(ENGINE_ONE_MINUS, LOW);
  digitalWrite(ENGINE_TWO_PLUS, LOW);
  digitalWrite(ENGINE_TWO_MINUS, HIGH);
}

void leftB()
{
  digitalWrite(ENGINE_ONE_PLUS, HIGH);
  digitalWrite(ENGINE_ONE_MINUS, LOW);
  digitalWrite(ENGINE_TWO_PLUS, LOW);
  digitalWrite(ENGINE_TWO_MINUS, LOW);
}
void rightB()
{
  digitalWrite(ENGINE_ONE_PLUS, LOW);
  digitalWrite(ENGINE_ONE_MINUS, LOW);
  digitalWrite(ENGINE_TWO_PLUS, HIGH);
  digitalWrite(ENGINE_TWO_MINUS, LOW);
}

void stop()
{
  digitalWrite(ENGINE_ONE_PLUS, LOW);
  digitalWrite(ENGINE_ONE_MINUS, LOW);
  digitalWrite(ENGINE_TWO_PLUS, LOW);
  digitalWrite(ENGINE_TWO_MINUS, LOW);
 }

void setupEngine(){
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(ENGINE_ONE_PLUS, OUTPUT);
  pinMode(ENGINE_ONE_MINUS, OUTPUT);
  pinMode(ENGINE_TWO_PLUS, OUTPUT);
  pinMode(ENGINE_TWO_MINUS, OUTPUT);
}

 int manualControl(int direction){
  switch(direction) {
  case STOP:
  //    Serial.println("stop");
    stop();
    break;
  case FORWARD:
 //     Serial.println("forward");
    forward();
    break;
  case BACK:
//    Serial.println("back");
    back();
    break;
  case LEFT:
    Serial.println("left 3");
    leftt();
    break;
  case RIGHT:
    Serial.println("right 3");
    right();
    break;
  case BACK_LEFT:
//    Serial.println("leftB");
    leftB();
    break;
  case BACK_RIGHT:
 //   Serial.println("rightB");
    rightB();
    break;
  case SWITCH_AUTO_CONTROLL:
    // code block
    break;
  default:
    break;
    // code block
}
return TAKE_NO_ACTION;
}

