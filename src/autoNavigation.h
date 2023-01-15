
#define MAX_SENSOR_DISTANCE 200
#define MAX_REPEAT_ON_COLLISION_CHECK 4
#define TURNING_DISTANCE 16
#define REVERSE_TURNING_DISTANCE 6
#define COLLISION_DISTANCE 4

extern int forwardCollsion;
extern int rightCollision;
extern int leftCollision;
extern int sonarOneReading;
extern int sonarTwoReading;

void getSensorReadings();
int getSensorReading(int ping);
int getTravelDirection();
