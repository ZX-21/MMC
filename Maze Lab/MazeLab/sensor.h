#include <Arduino.h>
#include <VL6180X.h>

#define RightSensorAdd 0x20
#define FrontSensorAdd 0x24
#define LeftSensorAdd 0x26

VL6180X RightSensor;
VL6180X FrontSensor;
VL6180X LeftSensor;

//template<byte leftEmitter, byte leftDetector, byte frontEmitter, byte frontDetector, byte rightEmitter, byte rightDetector>

class MouseSensors {

private:
  // Used to store sensor values
  int distanceFront = 0;
  int distanceLeft = 0;
  int distanceRight = 0;
  // Variables used for smoothing
  int leftTotal;
  int frontTotal;
  int rightTotal;
  static const byte numReadings = 10;
  byte index;
  int leftReadings[numReadings];
  int frontReadings[numReadings];
  int rightReadings[numReadings];

  int leftSmoothed;
  int frontSmoothed;
  int rightSmoothed;

public:

  int left;
  int front;
  int right;

  void setupVL6180X() 
  {
    PORTC |= (1 << 2);
    delay(1);
    RightSensor.init();
    RightSensor.configureDefault();
    RightSensor.setAddress(RightSensorAdd);
    RightSensor.setScaling(2);
    RightSensor.setTimeout(50);

    PORTC |= (1 << 1);
    delay(1);
    FrontSensor.init();
    FrontSensor.configureDefault();
    FrontSensor.setAddress(FrontSensorAdd);
    FrontSensor.setScaling(2);
    FrontSensor.setTimeout(50);

    PORTC |= (1 << 0);
    delay(1);
    LeftSensor.init();
    LeftSensor.configureDefault();
    LeftSensor.setAddress(LeftSensorAdd);
    LeftSensor.setScaling(2);
    LeftSensor.setTimeout(50);

  }

  void configure()
  {
    DDRC = 0x0F; // Set PIN A0-A3 for OUTPUT to control SHUT PIN

    Wire.begin();
    delay(20);
  }

  void sense() {
    distanceRight = ReadRightSensor() - 68; //+26 for error right sensor 
    distanceFront = ReadFrontSensor() - 0;
    distanceLeft = ReadLeftSensor() - 30; // -10 for error left sensor
    

    // Smoothing ////////////////////////////
    leftTotal -= leftReadings[index];
    frontTotal -= frontReadings[index];
    rightTotal -= rightReadings[index];

    leftReadings[index] = distanceLeft;
    frontReadings[index] = distanceFront;
    rightReadings[index] = distanceRight;

    leftTotal += leftReadings[index];
    frontTotal += frontReadings[index];
    rightTotal += rightReadings[index];

    leftSmoothed = leftTotal / numReadings;
    frontSmoothed = frontTotal / numReadings;
    rightSmoothed = rightTotal / numReadings;

    left = leftSmoothed;
    front = frontSmoothed;
    right = rightSmoothed;

    index += 1;

    if (index >= numReadings) {
      index = 0;
    }


    //view();
  }

  void initialize() {
    for (byte i = 0; i < numReadings; i++) {
      sense();
    }
  }

  void halfInitialize() {
    for (byte i = 0; i < numReadings / 2; i++) {
      sense();
    }
  }

  void view() {
    Serial.print("Sensors: ");

    Serial.print(distanceLeft);
    Serial.print("\t L -> ");
    Serial.print(left);

    Serial.print("\t | \t");

    Serial.print(distanceFront);
    Serial.print("\t FR -> ");
    Serial.print(front);

    Serial.print("\t | \t");

    Serial.print(distanceRight);
    Serial.print("\t R -> ");
    Serial.println(right);
  }

  int ReadRightSensor() {
  PORTC |= (1 << 0);
  delay(1);
  return RightSensor.readRangeSingleMillimeters();
  }

  int ReadFrontSensor() {
    PORTC |= (1 << 3);
    delay(1);
    return FrontSensor.readRangeSingleMillimeters();
  }

  int ReadLeftSensor() {
    PORTC |= (1 << 2);
    delay(1);
    return LeftSensor.readRangeSingleMillimeters();
  }
};