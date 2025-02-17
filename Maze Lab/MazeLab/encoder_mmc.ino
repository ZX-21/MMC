// #define MOTOR_LEFT_FORWARD 6
// #define MOTOR_LEFT_BACKWARD 9
// #define MOTOR_RIGHT_FORWARD 10
// #define MOTOR_RIGHT_BACKWARD 11

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
//  	pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
// 	pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
// 	pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);

// }

// void loop() {
  
//   if (Serial.available() > 0) {
//     String receivedString = Serial.readStringUntil('\n');  // อ่านค่าทั้งหมดจนกด Enter

//     receivedString.trim();  // ตัดช่องว่างและอักขระ '\r' ออก (เผื่อมี '\r' มาด้วย)

//     // ตรวจสอบว่า receivedString เป็นตัวเลขหรือไม่
//     bool isValid = true;
//     for (int i = 0; i < receivedString.length(); i++) {
//       if (!isDigit(receivedString[i])) {
//           isValid = false;
//           break;
//       }
//     }

//     if (isValid && receivedString.length() > 0) {
//       int number = receivedString.toInt();  // แปลงเป็น int
//       Serial.print("Received number: ");
//       Serial.println(number);
//       motorAction(number);  // ส่งไปยังฟังก์ชัน motorAction
//     } else {
//       Serial.println("Invalid input. Please enter a number.");
//     }
// }
  
// }

// void setLeftMotorPWM(int pwm) 
// {
//   pwm = constrain(pwm, -255, 255);
//   if (pwm > 0) 
//   {
//     analogWrite(MOTOR_LEFT_FORWARD, pwm);
//     analogWrite(MOTOR_LEFT_BACKWARD, 0);
//   } 
//   else 
//   {
//     analogWrite(MOTOR_LEFT_FORWARD, 0);
//     analogWrite(MOTOR_LEFT_BACKWARD, -pwm);
//   }
// }

// void setRightMotorPWM(int pwm) 
// {
//   pwm = constrain(pwm, -255, 255);
//   if (pwm > 0) 
//   {
//     analogWrite(MOTOR_RIGHT_FORWARD, pwm);
//     analogWrite(MOTOR_RIGHT_BACKWARD, 0);
//   } 
//   else 
//   {
//     analogWrite(MOTOR_RIGHT_FORWARD, 0);
//     analogWrite(MOTOR_RIGHT_BACKWARD, -pwm);
//   }
// }

void setMotorPWM(int left, int right) {
  setLeftMotorPWM(left);
  setRightMotorPWM(right);
}

void motorAction(int function) {
  switch (function) {
    case 0:
      setMotorPWM(0, 0);
      Serial.println("motors off");
      break;
    case 1:
      setMotorPWM(63, 63);
      Serial.println("forward 25%");
      break;
    case 2:
      setMotorPWM(127, 127);
      Serial.println("forward 50%");
      break;
    case 3:
      setMotorPWM(255, 255);
      Serial.println("forward 100%");
      break;
    case 4:
      setMotorPWM(-63, -63);
      Serial.println("reverse 25%");
      break;
    case 5:
      setMotorPWM(-127, -127);
      Serial.println("reverse 50%");
      break;
    case 6:
      setMotorPWM(-195, -195);
      Serial.println("reverse 75%");
      break;
    case 7:
      setMotorPWM(-63, 63);
      Serial.println("spin left 25%");
      break;
    case 8:
      setMotorPWM(-127, 127);
      Serial.println("spin left 50%");
      break;
    case 9:
      setMotorPWM(63, -63);
      Serial.println("spin right 25%");
      break;
    case 10:
      setMotorPWM(127, -127);
      Serial.println("spin right 50%");
      break;
    case 11:
      setMotorPWM(0, 63);
      Serial.println("pivot left 25%");
      break;
    case 12:
      setMotorPWM(63, 0);
      Serial.println("pivot right 25%");
      break;
    case 13:
      setMotorPWM(63, 127);
      Serial.println("curve left");
      break;
    case 14:
      setMotorPWM(127, 63);
      Serial.println("curve right");
      break;
    case 15:
      setMotorPWM(195, 127);
      Serial.println("big curve right");
      break;
    case 16:
      setMotorPWM(127, 195);
      Serial.println("big curve left");
      break;
    default:
      setMotorPWM(0, 0);
      break;
  }
}
