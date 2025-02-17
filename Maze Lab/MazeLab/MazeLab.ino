#include "Maze.h"
#include "encoders.h"
#include "Sensor.h"

#define DEBUG

#define MOTOR_LEFT_FORWARD 9
#define MOTOR_LEFT_BACKWARD 11
#define MOTOR_RIGHT_FORWARD 6
#define MOTOR_RIGHT_BACKWARD 10

int R = 10;
int C = 0;
int TR = 3;
int TC = 7;

MouseSensors sensors;
Encoders encoders; 
MouseMaze<11,11> maze; // <rows,cols>

void setup() {
  Serial.begin(115200);

  sensors.configure();
  sensors.setupVL6180X();
  sensors.initialize();


  maze.mouseRow = R;
  maze.mouseColumn = C;
  maze.mouseHeading = NORTH;

  // // Target position
  maze.targetRow = TR;
  maze.targetColumn = TC;

  //maze.AddCostAtNorth();
  //maze.addVirtualWalls_test2();
  maze.solveFloodFill();

  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);

  encoders.begin();

  delay(500);
  encoders.resetEncoders();
  delay(100);


 // moveForwardPD(180, 10, 0.3, 10, 10);
  //spinhalfLeft();
  //moveForwardPD(185, 1, 2, 10, 30);
  // spinTurnRight(90.0, 2, 0.1);
  //spinTurnLeft(91.0, 0.1, 0.1);


  //  scanWalls();
  //maze.printMaze();
  //  turnToNextMove();
  //spinTurnLeft(90.0, 0.2, 0.5);
  //spinTurnhalfLeft(90.0, 20, 0.2);
  //  delay(1000);
  // Serial.println(maze.findNextMove());

}

void loop() {

  if (maze.mouseRow ==  maze.targetRow && maze.mouseColumn == maze.targetColumn){
    maze.printMaze();
    delay(1000);
  } else {
     maze.printMaze();\
     delay(200);
     scanWalls();
     delay(200);
     turnToNextMove();
     delay(300);
  }


  // //  Serial.println(maze.findNextMove());

      delay(10);



  // put your main code here, to run repeatedly:

}

void turnToNextMove(){
  
  byte desiredHeading = maze.findNextMove();
  int difference = maze.mouseHeading - desiredHeading;

    if(difference == 1 || difference == -3){
      spinTurnLeft(91.0, 0.1, 0.2);
      sensors.initialize();//update ค่าเซนเซอร์
    }else if(difference == 3 || difference == -1 ) {
      spinTurnRight(90.0, 0.1, 0.2);
      sensors.initialize(); //update ค่าเซนเซอร์
    }else if(difference == 2 || difference == -2){
      sensors.sense(); //update ค่าเซนเซอร์
      if(sensors.left > sensors.right) {
        spinhalfLeft();
       
      } else { 
       spinhalfRight();
      //(random(2) == 0) ? spinTurnRight(180.0, 1.8, 0.2) : spinTurnLeft(180.0, 1.8, 0.4);
        sensors.initialize();//update ค่าเซนเซอร์
      }
    } else if (difference == 0) {
      // while(1) {
      //   maze.mouseHeading;
      //   sensors.sense();


      //   if (sensors.left < 35) {
      //     spinTurnRight(10.0, 0.1, 0.2);

          if (maze.mouseHeading == 0) {
          delay(200);
          moveForwardPD(180, 2, 0.3, 1, 40);
          R = R -= 1;
         maze.mouseRow = R;
      ///   break;
           } else if (maze.mouseHeading == 1) {
          delay(200);
         moveForwardPD(180, 2, 0.3, 1, 40);
         C = C += 1;
         maze.mouseColumn = C;
       ///  break;
           } else if (maze.mouseHeading == 2) {
          delay(200);
         moveForwardPD(180, 2, 0.3, 1, 40);
         R = R += 1;
         maze.mouseRow = R;
     //    break;
          } else if (maze.mouseHeading == 3) {
          delay(200);
         moveForwardPD(180,  2, 0.3, 1, 40);
         C = C -= 1;
         maze.mouseColumn = C;
      //  break;
        }
     //   } 



        // if (sensors.right < 35) {
        //  spinTurnLeft(10.0, 0.1, 0.2);
        //  if ( maze.mouseHeading == 0) {
        //   delay(200);
        //   moveForwardPD(180, 2, 0.3, 1, 40);
        //   R = R -= 1;
        //  maze.mouseRow = R;
        //  break;
        //    } else if ( maze.mouseHeading == 1) {
        //   delay(200);
        //  moveForwardPD(180, 2, 0.3, 1, 40);
        //  C = C += 1;
        //  maze.mouseColumn = C;
        //  break;
        //    } else if (maze.mouseHeading == 2) {
        //   delay(200);
        //  moveForwardPD(180, 2, 0.3, 1, 40);
        //  R = R += 1;
        //  maze.mouseRow = R;
        //  break;
        //   } else if ( maze.mouseHeading == 3) {
        //   delay(200);
        //  moveForwardPD(180,  2, 0.3, 1, 40);
        //  C = C -= 1;
        //  maze.mouseColumn = C;
        //  break;
        // }
        // }


        
        // if (sensors.left > 35 && sensors.right > 35) {

        //    if (desiredHeading == 0 && maze.mouseHeading == 0) {
        //   delay(200);
        //   moveForwardPD(180, 2, 0.3, 1, 40);
        //   R = R -= 1;
        //  maze.mouseRow = R;
        //  break;
        //    } else if (desiredHeading == 1 && maze.mouseHeading == 1) {
        //   delay(200);
        //  moveForwardPD(180, 2, 0.3, 1, 40);
        //  C = C += 1;
        //  maze.mouseColumn = C;
        //  break;
        //    } else if (desiredHeading == 2 && maze.mouseHeading == 2) {
        //   delay(200);
        //  moveForwardPD(180, 2, 0.3, 1, 40);
        //  R = R += 1;
        //  maze.mouseRow = R;
        //  break;
        //   } else if (desiredHeading == 3 && maze.mouseHeading == 3) {
        //   delay(200);
        //  moveForwardPD(180,  2, 0.3, 1, 40);
        //  C = C -= 1;
        //  maze.mouseColumn = C;
        //  break;
        // }
        // }
       }
    

    maze.mouseHeading = desiredHeading;

}

void scanWalls(){
  int front_threshold = 100;
  int left_threshold = 100;
  int right_threshold = 100;

   sensors.initialize();//update ค่าเซนเซอร์
  
  if(sensors.front < front_threshold)  
    maze.addWalls(maze.mouseHeading);
    
  if(sensors.right < right_threshold)
    maze.addWalls((maze.mouseHeading + 1) % 4); //แก้ปัญหากรณีค่าที่ได้เกินที่นิย่ามเอาไว้

  if(sensors.left < left_threshold)
    maze.addWalls((maze.mouseHeading - 1 + 4) % 4);
    
}
