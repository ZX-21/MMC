#include <Arduino.h>

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

const int neighboringCells[4][2] = {
  { -1, 0 },
  { 0, 1 },
  { 1, 0 },
  { 0, -1 },
};

const byte neighboringWalls[4][2] = {
  { 0, 0 },
  { 0, 1 },
  { 1, 0 },
  { 0, 0 }
};

template<byte ROWS, byte COLUMNS>
class MouseMaze {
private:
  // vertical walls array
  boolean verticalWalls[ROWS][COLUMNS + 1];//สำหรับเก็บค่าตำแหน่งของกำแพงแนวตั้ง ถ้า array นั้นมีกำแพงจะมีค่าเป็น TRUE

  // horizontal walls array
  boolean horizontalWalls[ROWS + 1][COLUMNS];//สำหรับเก็บค่าตำแหน่งของกำแพงแนวนอน ถ้า array นั้นมีกำแพงจะมีค่าเป็น TRUE

public:
  // value array
  byte values[ROWS][COLUMNS];

  byte mouseRow;      // ใช้สำหรับเก็บค่าตำแหน่งของหุ่นยนต์พิกัด x หรือ ROW
  byte mouseColumn;   // ใช้สำหรับเก็บค่าตำแหน่งของหุ่นยนต์พิกัด y หรือ COLUMN
  byte mouseHeading;  // ใช้สำหรับเก็บค่าทิศทางปัจจุบันของหุ่นยนต์

  byte targetRow;
  byte targetColumn;

  //Constructor method (called when the maze is created) สร้างกำแพงเริ่มต้นรอบ ๆ สนามทั้งหมด
  MouseMaze() {
    //initialize verticalWalls (add exterior walls)
    for (byte i = 0; i < ROWS; i++) {
      for (byte j = 0; j < COLUMNS + 1; j++) {
        if (j == 0 || j == COLUMNS) {
          verticalWalls[i][j] = true;
        }
      }
    }

    //initialize horizontalWalls (add exterior walls)
    for (byte i = 0; i < ROWS + 1; i++) {
      for (byte j = 0; j < COLUMNS; j++) {
        if (i == 0 || i == ROWS) {
          horizontalWalls[i][j] = true;
        }
      }
    }
  }

  void AddCostAtNorth(){
    // Initialize Cost กำหนดให้ทุกเซลล์เริ่มต้นด้วย 255
    for (byte i = 0; i < ROWS; i++) {
      for (byte j = 0; j < COLUMNS; j++) {
        values[i][j] = 255;
      }
    }

    // Set target cell
    values[targetRow][targetColumn] = 0;

    //Add Cost at NORTH
     for (byte i = 0; i < ROWS; i++) {
        for (byte j = 0; j < COLUMNS; j++) {
          if (values[i][j] < 255) {
              int neighboringCellRow = i + -1; // i = targetRow ทิศเหนือของเป้าหมายค่าพิกัด ROW = i + -1 และ Column = j + 0
              int neighboringCellColumn = j + 0; // j = targetColumn
              values[neighboringCellRow][neighboringCellColumn] = values[i][j] + 1; //เพิ่มค่าจากเป้าหมาย + 1 (เดิมจุดเป้าหมาย = 0)
          }
        }
     }
  }

  void solveFloodFill() {

    // Initialize Cost กำหนดให้ทุกเซลล์เริ่มต้นด้วย 255
    for (byte i = 0; i < ROWS; i++) {
      for (byte j = 0; j < COLUMNS; j++) {
        values[i][j] = 255;
      }
    }

    // Set target cell
    values[targetRow][targetColumn] = 0;

    // The Solving
    bool continueSolving = true;

    while (continueSolving) {

      continueSolving = false;

      for (byte i = 0; i < ROWS; i++) {
        for (byte j = 0; j < COLUMNS; j++) {
          if (values[i][j] < 255) {
            for (byte k = 0; k < 4; k++) { //อัพเดทค่าของเซลล์รอบ ๆ เซลล์ปัจจุบันโดยเริ่มตรวจจาก NORTH, EAST, SOUTH, WEST โดย K = 0 8nv N, E = 1, S = 2, W = 3
              int neighboringCellRow = i + neighboringCells[k][0];
              int neighboringCellColumn = j + neighboringCells[k][1];

              byte neighboringWallRow = i + neighboringWalls[k][0]; //อัพเดทกำแพงในแต่ละทิศรอบ ๆ เซลล์ปัจจุบันโดยตรวจจากNORTH, EAST, SOUTH, WEST โดย K = 0 คือ N, E = 1, S = 2, W = 3 
              byte neighboringWallColumn = j + neighboringWalls[k][1];

              bool wallExists = false;

              if (k == NORTH || k == SOUTH)
                wallExists = horizontalWalls[neighboringWallRow][neighboringWallColumn]; //ตรวจสอบว่า Cell ทิศเหนือและทิศใต้เป็นกำแพงหรือไม่ ถ้าเป็นกำแพงให้ค่า wallExists = TRUE ซึ่งหมายถึงไม่ต้องอัพเดตค่า Cost
              else  // Must be loking at an EAST or WEST wall
                wallExists = verticalWalls[neighboringWallRow][neighboringWallColumn];//ตรวจสอบว่า Cell ทิศตะวันออกและทิศตะวันตกเป็นกำแพงหรือไม่ ถ้าเป็นกำแพงให้ค่า wallExists = TRUE ซึ่งหมายถึงไม่ต้องอัพเดตค่า Cost
              if (values[neighboringCellRow][neighboringCellColumn] == 255 && !wallExists) { //ถ้า cell รอบ ๆ มีค่าเท่ากับ 255 และไม่มีกำแพงให้เพิ่มค่าของ cell + 1 
                values[neighboringCellRow][neighboringCellColumn] = values[i][j] + 1;
                continueSolving = true;  // If update, then continueSolving
              }
            }
          }
        }
      }
    }
  }

  byte findNextMove() {

    byte valueBestNeighbor = 255;
    byte desiredHeading = 0; //ทิศเหนือ

    for (byte k = 0; k < 4; k++) {

      // ตรวจสอบค่า cost รอบ ๆ ของ cell ปัจจุบันที่หุ่นยนต์อยู่ โดยวนจากทิศ N, E, S, W ตามลำดับ
      int neighboringCellRow = mouseRow + neighboringCells[k][0];
      int neighboringCellColumn = mouseColumn + neighboringCells[k][1];

      //อัพเดทกำแพงในแต่ละทิศรอบ ๆ เซลล์ปัจจุบันโดยตรวจจากNORTH, EAST, SOUTH, WEST โดย K = 0 คือ N, E = 1, S = 2, W = 3 
      byte neighboringWallRow = mouseRow + neighboringWalls[k][0];
      byte neighboringWallColumn = mouseColumn + neighboringWalls[k][1];

      // ตรวจค่า cell รอบ ๆ ตำแหน่งที่หุ่นยนต์อยู่
      bool wallExists = false;
      //ตรวจสอบกำแพงทิศเหนือและทิศใต้
      if (k == NORTH || k == SOUTH)
        wallExists = horizontalWalls[neighboringWallRow][neighboringWallColumn];
      else  // //ตรวจสอบกำแพงทิศตะวันออกและทิศตก
        wallExists = verticalWalls[neighboringWallRow][neighboringWallColumn];

      // ถ้าค่าที่อยู่ปัจจุบันของหุ่นยนต์น้อยกว่าค่ารอบ ๆ และไม่เจอกำแพงในทิศนั้น ให้อัพเดท
      if (values[neighboringCellRow][neighboringCellColumn] < valueBestNeighbor && !wallExists
          || values[neighboringCellRow][neighboringCellColumn] == valueBestNeighbor && !wallExists && k == mouseHeading) {//กรณีมีค่าเท่ากันมากกว่า 1 ช่อง ให้เลือกช่องที่ไม่ต้องเปลี่ยนทิศการหมุน กรณีเท่ากันแต่ต้องเปลี่ยนทิศทางทั้งคู่จะเลือกค่าที่เปลี่ยนก่อน
        valueBestNeighbor = values[neighboringCellRow][neighboringCellColumn];//เปลี่ยนเป็นค่าที่น้อยที่สุดจากทั้ง 4 ทิศ
        desiredHeading = k;//อัพเดทค่าทิศทางใหม่ของหุ่นยนต์ โดยค่าที่ได้จะเป็นเลข 0 - 3 โดย k = 0 คือ NORTH, 1 = EAST, 2 = SOUTH, 3 = WEST 
      }
    }

    return desiredHeading;
  }

  void addWalls(byte Direction) {

    switch (Direction) {
      case NORTH:
        horizontalWalls[mouseRow][mouseColumn] = true;
        break;
      case EAST:
        verticalWalls[mouseRow][mouseColumn + 1] = true;
        break;
      case SOUTH:
        horizontalWalls[mouseRow + 1][mouseColumn] = true;
        break;
      case WEST:
        verticalWalls[mouseRow][mouseColumn] = true;
        break;
    }
  }

  void printMaze() {
    for (byte i = 0; i < 2 * ROWS + 1; i++) {
      for (byte j = 0; j < 2 * COLUMNS + 1; j++) {
        //Add Horizontal Walls
        if (i % 2 == 0 && j % 2 == 1) {
          if (horizontalWalls[i / 2][j / 2] == true) {
            Serial.print(" ---");
          } else {
            Serial.print("    ");
          }
        }

        //Add Vertical Walls
        if (i % 2 == 1 && j % 2 == 0) {
          if (verticalWalls[i / 2][j / 2] == true) {
            Serial.print("|");
          } else {
            Serial.print(" ");
          }
        }

        //Add Flood Fill Values
        if (i % 2 == 1 && j % 2 == 1) {
          if ((i - 1) / 2 == mouseRow && (j - 1) / 2 == mouseColumn) {
            if (mouseHeading == NORTH) {
              Serial.print(" ^ ");
            } else if (mouseHeading == EAST) {
              Serial.print(" > ");
            } else if (mouseHeading == SOUTH) {
              Serial.print(" v ");
            } else if (mouseHeading == WEST) {
              Serial.print(" < ");
            }
          } else {
            byte value = values[(i - 1) / 2][(j - 1) / 2];
            if (value >= 100) {
              Serial.print(value);
            } else {
              Serial.print(" ");
              Serial.print(value);
            }
            if (value < 10) {
              Serial.print(" ");
            }
          }
        }
      }
      Serial.print("\n");
    }
    Serial.print("\n");
  }


};
