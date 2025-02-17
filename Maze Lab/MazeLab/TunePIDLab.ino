#define DEBUG

 #include "encoders.h"

// #define MOTOR_LEFT_FORWARD 6
// #define MOTOR_LEFT_BACKWARD 9
// #define MOTOR_RIGHT_FORWARD 10
// #define MOTOR_RIGHT_BACKWARD 11

// Encoders encoders; 

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);

//   pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
//   pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
//   pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
//   pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);

//   encoders.begin();

//   delay(500);

//   encoders.resetEncoders();
//   delay(500);

//   moveForwardPD(185, 20, 0.3, 10, 100);
//   //encoders.resetEncoders();
//   //spinTurnRight(90, 20, 0.3);
//   //spinTurnLeft(90, 20, 0.3);
  
  
// }

// void loop() {
//   // put your main code here, to run repeatedly:
  

// }

void spinhalfRight() {
  spinTurnRight(90, 2, 0.1);
  spinTurnRight(90, 2, 0.1);
}
void spinhalfLeft() {
  spinTurnLeft(90, 0.1, 0.1);
  spinTurnLeft(90, 0.1, 0.1);
}

void spinTurnLeft(float degrees, float Kp, float Kd) {
  encoders.resetEncoders();
  // 1) กำหนดมุมเป้าหมาย
  float targetAngle = encoders.robotAngle - (degrees + 9);
  if (targetAngle < 0)   targetAngle += 360;
  if (targetAngle >= 360) targetAngle -= 360;

  // 2) ตัวแปรสำหรับ PD
  float error = 0.0, lastError = 0.0, output = 0.0;

  while (true) {
    encoders.updateRobotState(); // อ่านค่า Encoder เพื่อคำนวณ robotAngle

    #ifdef DEBUG
    Serial.print("Omega: ");
    Serial.print(encoders.robot_omega());
    Serial.print("\t target: ");
    Serial.print(targetAngle);
    Serial.print("\t Current: ");
    Serial.println(encoders.robotAngle);
    #endif
    // 3) คำนวณค่าข้อผิดพลาด
    error = targetAngle - encoders.robotAngle;

    // 4) Normalize error ให้อยู่ในช่วง -180..180
    if (error > 180)  error -= 360;
    if (error < -180) error += 360;

    // 5) เงื่อนไขหยุด
    if (fabs(error) < 1.0) {
      break;
    }

    // 6) คำนวณ PD
    float derivative = error - lastError;
    output = (Kp * error) + (Kd * derivative);

    // 7) กำหนด PWM และ minPWM
    int pwmLeft  = constrain((int)(-output), -255, 255);
    int pwmRight = constrain((int)(+output), -255, 255);

    int minPWM = 50;  // สามารถปรับได้
    if (abs(pwmLeft) < minPWM && pwmLeft != 0) {
      pwmLeft = (pwmLeft > 0) ? minPWM : -minPWM;
    }
    if (abs(pwmRight) < minPWM && pwmRight != 0) {
      pwmRight = (pwmRight > 0) ? minPWM : -minPWM;
    }

    // 8) ขับมอเตอร์
    setMotorSpeeds(-pwmLeft, -pwmRight);

    lastError = error;
    delay(10);
  }

  // หยุดมอเตอร์
  setMotorSpeeds(0, 0);
}

void spinTurnRight(float degrees, float Kp, float Kd) {
  // รีเซ็ต encoder ก่อนคำนวณ targetAngle
  encoders.resetEncoders();
  float targetAngle = encoders.robotAngle + (degrees + 7);
  if (targetAngle < 0)   targetAngle += 360;
  if (targetAngle >= 360) targetAngle -= 360;

  float error = 0.0, lastError = 0.0, derivative = 0.0;
  float output = 0.0;

  while (true) {
    encoders.updateRobotState();  // อัปเดต robotAngle ใหม่

    #ifdef DEBUG
    Serial.print("Omega: ");
    Serial.print(encoders.robot_omega());
    Serial.print("\t target: ");
    Serial.print(targetAngle);
    Serial.print("\t Current: ");
    Serial.println(encoders.robotAngle);
    #endif
    // คำนวณ error สำหรับการเลี้ยวซ้าย (เราต้องการให้ robotAngle เพิ่มขึ้น)
    error = targetAngle - encoders.robotAngle;

    // Normalize error ให้อยู่ในช่วง -180 ถึง 180
    if (error > 180.0)  error -= 360.0;
    if (error < -180.0) error += 360.0;

    // หยุดเมื่อ error ใกล้ 0
    if (fabs(error) < 1.0) {
      break;
    }

    derivative = error - lastError;
    output = (Kp * error) + (Kd * derivative);

    // กำหนดความเร็วให้มอเตอร์: ซ้ายถอยหลัง (-output), ขวาเดินหน้า (+output)
    int pwmLeft  = constrain((int)(-output), -255, 255);
    int pwmRight = constrain((int)(+output), -255, 255);

    // กำหนดค่า minPWM หาก output มีค่าน้อยเกินไป
    int minPWM = 50;
    if (abs(pwmLeft) < minPWM && pwmLeft != 0) {
      pwmLeft = (pwmLeft > 0) ? minPWM : -minPWM;
    }
    if (abs(pwmRight) < minPWM && pwmRight != 0) {
      pwmRight = (pwmRight > 0) ? minPWM : -minPWM;
    }

    setMotorSpeeds(-pwmLeft, -pwmRight);
    lastError = error;
    delay(10);
  }
  setMotorSpeeds(0, 0);
}

void moveForwardPD(float targetDistanceMM,float Kp, float Kd, float KpBalance, int minPWM) {
  encoders.resetEncoders();
  delay(500);

  float error = 0.0;
  float lastError = 0.0;
  float derivative = 0.0;
  float controlSignal = 0.0;
  float distanceTravelled = 0.0;

  int baseSpeed = 90; // Base motor speed (adjust as needed)

  while (distanceTravelled < targetDistanceMM) {

    // Serial.print("Encoder Left: ");
    // Serial.print(leftEncoderCount);
    // Serial.print("| EncoderRight: ");
    // Serial.println(rightEncoderCount);
    
    
    // คำนวณระยะทางที่เดินทางได้
    distanceTravelled = encoders.averageEncoderCount() / encoders.pulsesPerMM;

    // คำนวณค่า error
    error = targetDistanceMM - distanceTravelled;

    // คำนวณค่า derivative
    derivative = error - lastError;

    // คำนวณสัญญาณควบคุม PD
    controlSignal = (Kp * error) + (Kd * derivative);

    // คำนวณความสมดุลของล้อซ้ายและขวา
    float balanceError = encoders.leftEncoderCount - encoders.rightEncoderCount;
    float balanceSignal = KpBalance * balanceError;

    // ปรับความเร็วล้อซ้ายและขวา
    int leftSpeed = ( baseSpeed + controlSignal );
// สลับทิศทางของมอเตอร์ขวา
    int rightSpeed = (baseSpeed + controlSignal );


    // ตรวจสอบและปรับค่า PWM ให้อยู่ในช่วงที่มอเตอร์เคลื่อนที่ได้
    if (abs(leftSpeed) > 0 && abs(leftSpeed) < minPWM) {
      leftSpeed = (leftSpeed > 0) ? minPWM : -minPWM;
    }
    if (abs(rightSpeed) > 0 && abs(rightSpeed) < minPWM) {
      rightSpeed = (rightSpeed > 0) ? minPWM : -minPWM;
    }

    // จำกัดค่า PWM อยู่ในช่วง 0 ถึง 255
    leftSpeed = constrain(leftSpeed, -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);

    // Serial.print("Left speed: ");
    // Serial.print(leftSpeed);
    // Serial.print("| Right speed: ");
    // Serial.println(rightSpeed);

    setMotorSpeeds(leftSpeed, rightSpeed - 5);

    // บันทึกค่า error ล่าสุด
    lastError = error;

    // Delay เล็กน้อยเพื่อความเสถียร
    delay(10);
  }

  // หยุดมอเตอร์เมื่อถึงเป้าหมาย
  setMotorSpeeds(0, 0);

}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  if (leftSpeed > 0) {
    analogWrite(MOTOR_LEFT_FORWARD, leftSpeed);
    analogWrite(MOTOR_LEFT_BACKWARD, 0);
  } else {
    analogWrite(MOTOR_LEFT_FORWARD, 0);
    analogWrite(MOTOR_LEFT_BACKWARD, -leftSpeed);
  }

  if (rightSpeed > 0) {
    analogWrite(MOTOR_RIGHT_FORWARD, rightSpeed);
    analogWrite(MOTOR_RIGHT_BACKWARD, 0);
  } else {
    analogWrite(MOTOR_RIGHT_FORWARD, 0);
    analogWrite(MOTOR_RIGHT_BACKWARD, -rightSpeed);
  }
}
void setLeftMotorPWM(int pwm) 
{
  pwm = constrain(pwm, -255, 255);
  if (pwm > 0) 
  {
    analogWrite(MOTOR_LEFT_FORWARD, pwm);
    analogWrite(MOTOR_LEFT_BACKWARD, 0);
  } 
  else 
  {
    analogWrite(MOTOR_LEFT_FORWARD, 0);
    analogWrite(MOTOR_LEFT_BACKWARD, -pwm);
  }
}

void setRightMotorPWM(int pwm) 
{
  pwm = constrain(pwm, -255, 255);
  if (pwm > 0) 
  {
    analogWrite(MOTOR_RIGHT_FORWARD, pwm);
    analogWrite(MOTOR_RIGHT_BACKWARD, 0);
  } 
  else 
  {
    analogWrite(MOTOR_RIGHT_FORWARD, 0);
    analogWrite(MOTOR_RIGHT_BACKWARD, -pwm);
  }
}

void spinTurnhalfLeft(float degrees, float Kp, float Kd) {
  // รีเซ็ต encoder ก่อนคำนวณ targetAngle
  encoders.resetEncoders();
  float targetAngle = encoders.robotAngle + (degrees += 8.08);
  if (targetAngle < 0)   targetAngle += 360;
  if (targetAngle >= 360) targetAngle -= 360;

  float error = 0.0, lastError = 0.0, derivative = 0.0;
  float output = 0.0;

  while (true) {
    encoders.updateRobotState();  // อัปเดต robotAngle ใหม่

    #ifdef DEBUG
    Serial.print("Omega: ");
    Serial.print(encoders.robot_omega());
    Serial.print("\t target: ");
    Serial.print(targetAngle);
    Serial.print("\t Current: ");
    Serial.println(encoders.robotAngle);
    #endif
    // คำนวณ error สำหรับการเลี้ยวซ้าย (เราต้องการให้ robotAngle เพิ่มขึ้น)
    error = targetAngle - encoders.robotAngle;

    // Normalize error ให้อยู่ในช่วง -180 ถึง 180
    if (error > 180.0)  error -= 360.0;
    if (error < -180.0) error += 360.0;

    // หยุดเมื่อ error ใกล้ 0
    if (fabs(error) < 1.0) {
      break;
    }

    derivative = error - lastError;
    output = (Kp * error) + (Kd * derivative);

    // กำหนดความเร็วให้มอเตอร์: ซ้ายถอยหลัง (-output), ขวาเดินหน้า (+output)
    int pwmLeft  = constrain((int)(-output), -255, 255);
    int pwmRight = constrain((int)(+output), -255, 255);

    // กำหนดค่า minPWM หาก output มีค่าน้อยเกินไป
    int minPWM = 50;
    if (abs(pwmLeft) < minPWM && pwmLeft != 0) {
      pwmLeft = (pwmLeft > 0) ? minPWM : -minPWM;
    }
    if (abs(pwmRight) < minPWM && pwmRight != 0) {
      pwmRight = (pwmRight > 0) ? minPWM : -minPWM;
    }

    setMotorSpeeds(pwmLeft, pwmRight);
   // setRightMotorPWM(pwmRight); 

    lastError = error;
    delay(10);
  }
  setMotorSpeeds(0, 0);
  delay(100);
}

void spinTurnhalfRight(float degrees, float Kp, float Kd) {
  // รีเซ็ต encoder ก่อนคำนวณ targetAngle
  encoders.resetEncoders();
  float targetAngle = encoders.robotAngle + (degrees += 8.08);
  if (targetAngle < 0)   targetAngle += 360;
  if (targetAngle >= 360) targetAngle -= 360;

  float error = 0.0, lastError = 0.0, derivative = 0.0;
  float output = 0.0;

  while (true) {
    encoders.updateRobotState();  // อัปเดต robotAngle ใหม่

    #ifdef DEBUG
    Serial.print("Omega: ");
    Serial.print(encoders.robot_omega());
    Serial.print("\t target: ");
    Serial.print(targetAngle);
    Serial.print("\t Current: ");
    Serial.println(encoders.robotAngle);
    #endif
    // คำนวณ error สำหรับการเลี้ยวซ้าย (เราต้องการให้ robotAngle เพิ่มขึ้น)
    error = targetAngle - encoders.robotAngle;

    // Normalize error ให้อยู่ในช่วง -180 ถึง 180
    if (error > 180.0)  error -= 360.0;
    if (error < -180.0) error += 360.0;

    // หยุดเมื่อ error ใกล้ 0
    if (fabs(error) < 1.0) {
      break;
    }

    derivative = error - lastError;
    output = (Kp * error) + (Kd * derivative);

    // กำหนดความเร็วให้มอเตอร์: ซ้ายถอยหลัง (-output), ขวาเดินหน้า (+output)
    int pwmLeft  = constrain((int)(-output), -255, 255);
    int pwmRight = constrain((int)(+output), -255, 255);

    // กำหนดค่า minPWM หาก output มีค่าน้อยเกินไป
    int minPWM = 50;
    if (abs(pwmLeft) < minPWM && pwmLeft != 0) {
      pwmLeft = (pwmLeft > 0) ? minPWM : -minPWM;
    }
    if (abs(pwmRight) < minPWM && pwmRight != 0) {
      pwmRight = (pwmRight > 0) ? minPWM : -minPWM;
    }

    setMotorSpeeds(pwmLeft, pwmRight);
   // setRightMotorPWM(pwmRight); 

    lastError = error;
    delay(10);
  }
  setMotorSpeeds(0, 0);
  delay(100);
}