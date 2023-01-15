#include "NewPing.h"
#include "autoNavigation.h"
#include "directionDefs.h"
#include "pinDefs.h"


int forwardCollsion=0;
int rightCollision=0;
int leftCollision=0;

int sonarOneReading=0 ;
int sonarTwoReading =0;
NewPing sonar2(5, 18, 200);
NewPing sonar1(19, 21, 200);


void getSensorReadings(){
    sonarOneReading = getSensorReading(sonar1.ping());
    sonarTwoReading = getSensorReading(sonar2.ping());
}
 

int getTravelDirection(){


Serial.println(sonarOneReading);
Serial.println(sonarTwoReading);

    // if after checking for three times distance read by both sensors is 0 perform turning to the back left
    if(sonarOneReading == 0&&sonarTwoReading == 0){
        forwardCollsion++;
        if(forwardCollsion==MAX_REPEAT_ON_COLLISION_CHECK){
          forwardCollsion=0;
          return BACK_LEFT;
        }
    }

//this protects from the problem when the distance in front of a robot exceeds max expected distantace and in that case it is set to 0;
    if (sonarOneReading == 0)
    {
      rightCollision++;
      if(rightCollision==MAX_REPEAT_ON_COLLISION_CHECK){
      sonarOneReading = MAX_SENSOR_DISTANCE;
      }else{
        rightCollision=0;
      }
    }

    if (sonarTwoReading == 0)
    {
      leftCollision++;
      if(leftCollision==MAX_REPEAT_ON_COLLISION_CHECK){
      sonarTwoReading = MAX_SENSOR_DISTANCE;
      }else{
        leftCollision=0;
      }
    }

    ////

    if (sonarOneReading < COLLISION_DISTANCE || sonarTwoReading < COLLISION_DISTANCE)
    {
      return BACK_LEFT;
    }


    //if closing to an obstacle check on which size you are closing and turn to other direction, if it is detected you are to close, reverse into oposite direction
    if (sonarTwoReading < TURNING_DISTANCE && sonarTwoReading < sonarOneReading)
    {
      if (sonarTwoReading < REVERSE_TURNING_DISTANCE)
      {
        Serial.println("AAAA");
        return BACK_LEFT;
      }
      else
      {
       return RIGHT;
      }
    }

    if (sonarOneReading < TURNING_DISTANCE && sonarTwoReading > sonarOneReading)
    {
      
      if (sonarOneReading < REVERSE_TURNING_DISTANCE)
      {
        Serial.println("BBB");
       return BACK_RIGHT;
      }
      else
      {
        return LEFT;
      }
    }
///  

  //if road is clear go forward
  if (sonarOneReading >= TURNING_DISTANCE && sonarTwoReading >= TURNING_DISTANCE)
  {
    return FORWARD;
  }
  return STOP;
}


//no idea  why  wrote it like this but if I touch it it stops working  - have to investigate and refactor
int getSensorReading(int ping)
{
  //Serial.println(ping);
  int distance1 = 1;
  int distance2 = 2;
  int distance3 = 1;
  float distance4 = 1;
  float tempD = 1;

  distance4 = ping;
  tempD = distance4 * 0.034 / 2;
  distance4 = tempD;
  distance1 = distance2;
  distance2 = distance3;
  distance3 = distance4;
  if (distance3 == distance1 + 1 || distance3 == distance1 - 1)
  {
    distance3 = distance2;
  }

  return distance3;
  //  sonarOneReading=distance4;
}