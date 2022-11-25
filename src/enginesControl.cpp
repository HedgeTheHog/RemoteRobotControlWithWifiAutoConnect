#include "enginesControl.h"
#include <Arduino.h> 


//TODO define pins as variables

void forward()
{
  digitalWrite(14, LOW);
  digitalWrite(27, HIGH);
  digitalWrite(26, LOW);
  digitalWrite(25, HIGH);
}

void back()
{
  digitalWrite(14, HIGH);
  digitalWrite(27, LOW);
  digitalWrite(26, HIGH);
  digitalWrite(25, LOW);
}
void left()
{
  digitalWrite(14, LOW);
  digitalWrite(27, HIGH);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
}
void right()
{
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, HIGH);
}

void leftB()
{
  digitalWrite(14, HIGH);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
}
void rightB()
{
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, HIGH);
  digitalWrite(25, LOW);
}

void stop()
{
  digitalWrite(14, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
 }