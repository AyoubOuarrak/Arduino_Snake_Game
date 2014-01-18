//!Snake game on Arduino
/*!
*  using arduino and the old nokia
*  3310 display (PCD8544)
*
*  Author :    Ouarrak Ayoub
*  Version:    v1.1
*
* (PCD8544)
*
* pin1 -> VDD(3.3V)
* pin2 -> SCK
* pin3 -> SDIN
* pin4 -> D/C
* pin5 -> SCE
* pin6 -> GND
* pin7 -> VOUT(-> 1uF capacitor -> GND)
* pin8 -> RESET
*
* Arduino
*
* 3  -> SCK   (2)
* 4  -> SDIN  (3)
* 5  -> D/C   (4)
* 6  -> RESET (6)
* 7  -> SCE   (5)
* 8  -> Speaker/buzzer
* 9  -> led1
* 10 -> led2
* A1 -> Button (Pause)
* A2 -> Button (Down)
* A3 -> Button (Left)
* A4 -> Button (Up)
* A5 -> Butoon (Right)
*
* Install the new library:
* windows ->   C:\users\[Name]\Documents\Arduino\libraries
* Mac     ->   /Users/[name]/Documents/Arduino/libraries
*/

//! Library:  https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//! display object using pin 3,4,5,7,6
Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 7, 6);
//! costants
#define LEFT         1
#define RIGHT        2
#define UP           3
#define DOWN         4
//! screen risolution
#define MAX_WIDTH   84
#define MAX_HEIGHT  48

int x = 5;
int y = 5;
//! position of the head's snake
int xHeadSnake;
int yHeadSnake;
//! position of the fruit
int xFruit = 0;
int yFruit = 0;
int point  = 0;
int points = 10;
boolean left   = false;
boolean right  = true;
boolean up     = false;
boolean down   = false;
boolean pause  = true;
int speakerPin = 8;
int dr = 0;
int dc = 1;
//! this vector contain the individual part (x) of the snake; cols[0], row[0] is the head's snake
int snake_cols[260];
//! this vector contain the individual part (y) of the snake;
int snake_row[260];
int snake_lenght = 10;
int level = 0;
int time  = 21;
int lifes = 5;
//! input sensors (button)
int pauseBtm, upBtm, downBtm, rightBtm, leftBtm;
//! software-reset of arduino
void(* reset)(void) = 0;

/* !
*  SETUP FUNCTION
*/
void setup() {
   Serial.begin(9600);
   randomSeed(analogRead(A0));
   display.begin();
   pinMode(speakerPin, OUTPUT);
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   //! turn on display's leds
   digitalWrite(9, HIGH);
   analogWrite(10, 100);
   //! the position of the fruit is in the middle of the screen
   xFruit = (display.width()/2)+5;
   yFruit = (display.height()/2)+5;

   showInfo();
}
/* !
*  LOOP FUNCTION
*/
void loop() {
   xHeadSnake = snake_cols[0];
   yHeadSnake = snake_row[0];
   //! display the new level if i win and reset points and information
   if(point == 0 or point == points){
      level++;
      printLevel(level);
      delay(1500);
      upLevel(level);
      point = 1;
      //! points necessary to win
      points += 10;
      resetValues();
   }

   display.clearDisplay();
   //! read the value of the buttons
   readInput();
   moveSnake(pauseBtm, leftBtm, rightBtm, upBtm, downBtm);

   //! snake eat a fruit from the right or from the left
   for(int i = 0; i < 3 ; ++i) {
      //! control the position (x) of the head's snake with the postion (x) of the fruit
      if((xHeadSnake + i == xFruit) or (xHeadSnake == xFruit + i)) {
         //! control the position (y) of the head's snake with the postion (y) of the fruit
         if((yHeadSnake == yFruit) or (yHeadSnake + i == yFruit) or (yHeadSnake == yFruit + i)) {
            eatFruit();
            drawSnake();
         }
      }
      //! the snake eat the fruit from the top or from the botton
      if((yHeadSnake + i == yFruit) or (yHeadSnake == yFruit + i)) {
         //! control the position (x) of the head's snake with the postion (x) of the fruit
         if((xHeadSnake == xFruit) or (xHeadSnake + i == xFruit) or (xHeadSnake == xFruit + i)) {
            eatFruit();
            drawSnake();
         }
      }
   }
   snakeChangeDirectionLR();
   snakeChangeDirectionUD();
   //! the velocity of the movement of the snake is relative to the time
   delay(time);
}
/* !
*  RESET THE POSITION OF THE SNAKE
*/
void resetValues() {
   snake_lenght = 10;
   //! initialize the position of the snake
   for(int i = 0; i < snake_lenght-1; ++i) {
      snake_cols[i] = i;
      snake_row[i]  = (MAX_HEIGHT/2);
   }

   left  = false;
   right = true;
   up    = false;
   down  = false;
   dr    = 0;
   dc    = 1;
}
/* !
*  LEVELUP
*/
//! change the time to complete the level
void upLevel(int lev) {
   switch(lev) {
      case 1: time;      break;
      case 2: time -= 3; break;
      case 3: time -= 3; break;
      case 4: time -= 3; break;
      case 5: time -= 3; break;
      case 6: time -= 3; break;
   }
}
/* !
*  DRAW SNAKE
*/
void drawSnake() {
   //! from the level 2, add one obstacles in the middle of the screen
   if(level >= 2)
      //!MAX_WIDTH/2 = x1, 11 = y1; MAX_WIDTH/2 = x2, display.height()-11 = y
      display.drawLine(MAX_WIDTH/2, 11, MAX_WIDTH/2, display.height()-11, BLACK);

   //! from the level 3, draw two obstalces in the middle
   if(level >= 3){
      display.drawLine(11,(MAX_HEIGHT/3)-5, MAX_WIDTH-11, (MAX_HEIGHT/3)-5, BLACK);
      display.drawLine(11,MAX_HEIGHT/(1.5)+5, MAX_WIDTH-11, MAX_HEIGHT/(1.5)+5, BLACK);
   }
   //! draw the walls
   display.drawRect(0,0,MAX_WIDTH,MAX_HEIGHT,BLACK);
   //! draw snake
   for(int i = snake_lenght; i > 0; --i)
      display.drawCircle(snake_cols[i], snake_row[i], 1,  BLACK);
   //! draw the fruit (3px x 3px)
   display.fillRect(xFruit, yFruit, 3, 3, BLACK);
   //! display everthing i draw before
   display.display();
   //! change the position of the snake's part, when he change diretion
   for(int i = snake_lenght; i > 0; --i) {
      snake_row[i]  = snake_row[i-1];
      snake_cols[i] = snake_cols[i-1];
   }
   snake_row[0]  += dr;
   snake_cols[0] += dc;
}
/* !
*  WHERE THE SNAKE IS TRYING TO GO?
*/
void direc(int d){
   switch(d) {
      case 1: { left = true ; right = false; up = false; down = false; dr = 0;  dc = -1;} break;
      case 2: { left = false; right = true ; up = false; down = false; dr = 0;  dc = +1;} break;
      case 3: { left = false; right = false; up = true ; down = false; dr = -1; dc = 0; } break;
      case 4: { left = false; right = false; up = false; down = true ; dr = +1; dc = 0; } break;
   }
}
/* !
*  SHOW A MESSAGE IF THE SNAKE LOST ALL LIFES, ELSE DECREMENT THE LIFES
*/
void gameover(){
   if(lifes > 0) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(0,10);
      display.println("Lifes:");
      display.setCursor(50,10);
      display.println(lifes);
      display.setCursor(0,20);
      display.print("points: ");
      display.setCursor(50,20);
      display.print(point-1);
      display.setCursor(0, 30);
      display.print("level: ");
      display.setCursor(50,30);
      display.print(level);
      display.display();
      delay(2500);
      lifes--;
      resetValues();
   }
   else {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(15,0);
      display.println("Game Over");
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(0,20);
      display.print("points: ");
      display.setCursor(50,20);
      display.print(point-1);
      display.setCursor(0, 30);
      display.print("level: ");
      display.setCursor(50,30);
      display.print(level);
      display.display();
      delay(2500);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(0,0);
      display.println("Try again");
      display.display();
      delay(1500);
      //! reset arduino
      reset();
   }
}
/* !
*  GENERATE A SMALL SOUND
*/
void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds) {
   //funzione per generare un suono
   long delayAmount = (long)(1000000/frequencyInHertz);
   long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
   for(int x = 0; x < loopTime; ++x) {
      digitalWrite(speakerPin,HIGH);
      delayMicroseconds(delayAmount);
      digitalWrite(speakerPin,LOW);
      delayMicroseconds(delayAmount);
   }
   delay(2);
}
/* !
*  CHANGE THE DIRECTION OF THE SNAKE
*/
void moveSnake(int pauseBtm, int leftBtm, int rightBtm, int upBtm, int downBtm) {
   //! the user touched the pause button
   if(pauseBtm > 900) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.setCursor(12,0);
      display.println("Pause");
      display.setTextSize(1);
      display.setCursor(0,19);
      display.print("points: ");
      display.setCursor(50,19);
      display.print(point - 1);
      display.setCursor(0, 29);
      display.print("level: ");
      display.setCursor(50,29);
      display.print(level);
      display.setCursor(0,39);
      display.print("lifes: ");
      display.setCursor(50,39);
      display.print(lifes + 1);
      display.display();
      //! wait 3s and return to the game again
      delay(3000);
   }
   //! the left button was pressed and the snake is not going to left? the snake can turn left
   if(leftBtm > 900 and right == false)
      if ((xHeadSnake > 0 or xHeadSnake <= display.width()-xHeadSnake))
         direc(LEFT);
   //! the right button was pressed and the snake is not going to right? the snake can turn right
   if(rightBtm > 900 and left == false)
      if ((xHeadSnake > 0 or xHeadSnake <= display.width()-xHeadSnake))
         direc(RIGHT);
   //! the up button was pressed and the snake is not going to up? the snake can turn up
   if(upBtm > 900 and down == false)
      if((yHeadSnake > 0 or yHeadSnake <= display.height()-yHeadSnake))
         direc(UP);
   //! the down button was pressed and the snake is not going to down? the snake can turn down
   if(downBtm > 900 and up == false)
      if((yHeadSnake > 0 or yHeadSnake <= display.height()-yHeadSnake))
         direc(DOWN);
}
/* !
*  PRINT CURRENCT LEVEL
*/
void printLevel(int lvl) {
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(BLACK);
   display.setCursor(0, 0);
   display.print("Level:");
   display.setTextSize(2);
   display.setCursor(0, 30);
   display.print(lvl);
   display.display();
}
/* !
*  SNAKE CHANGED THE DIRECTION (LEFT OR RIGHT)
*/
void snakeChangeDirectionUD() {
   if(up == true) {
      //! snake touched the top wall
      if(yHeadSnake == 1)
         gameover();

      if(level >= 3)
         //! snake touched one of the two horizzontal obstacles (from the bottom)
         if(xHeadSnake > 11 and xHeadSnake < MAX_WIDTH-11)
            if(yHeadSnake == (MAX_HEIGHT/3) - 5 or yHeadSnake == (MAX_HEIGHT/1.5) + 5)
               gameover();
      //! snake is alive
      if(yHeadSnake > 1)
         drawSnake();
   }
   //! snake chage direction (down)
   if(down == true) {
      //! snake touched the bottom wall
      if(yHeadSnake == MAX_HEIGHT-1)
         gameover();

      if(level >= 3)
         //! snake touched one of the two horizzontal obstacles (from the top)
         if(xHeadSnake > 11 and xHeadSnake < MAX_WIDTH-11)
            if(yHeadSnake == (MAX_HEIGHT/3) - 5 or yHeadSnake == (MAX_HEIGHT/1.5) + 5)
               gameover();
      //! snake is alive
      if(yHeadSnake < MAX_HEIGHT-1)
         drawSnake();
   }
}
/* !
*  SNAKE CHANGED THE DIRECTION (UP OR DOWN)
*/
void snakeChangeDirectionLR() {
   //! snake change direction (left)
   if(left == true) {
      //! the snake touched the left wall
      if(xHeadSnake == 1)
         gameover();

      if(level >= 2)
         //! the snake touched the center ostacle (from the right)
         if(xHeadSnake == MAX_WIDTH/2 and yHeadSnake > 11 and yHeadSnake < display.height()-11)
            gameover();
      //! snake is alive
      if(xHeadSnake > 1)
         drawSnake();
   }
   //! snake chage direction (right)
   if(right == true) {
      //! snake touched the right wall
      if(xHeadSnake == MAX_WIDTH-1)
         gameover();

      if(level >= 2)
         //! snake touched the obstacle in the center (from the left part)
         if(xHeadSnake == MAX_WIDTH/2 and yHeadSnake > 11 and yHeadSnake < display.height()-11)
            gameover();
      //! snake is alive
      if(xHeadSnake < MAX_WIDTH-1)
         drawSnake();
   }
}
/* !
*  SNAKE EAT FRUIT
*/
void eatFruit() {
   point++;
   beep(speakerPin, 2000, 10);
   xFruit = random(2, 79);
   yFruit = random(2, 45);
   snake_lenght += 2;
}
/* !
*  READ THE BUTTON VALUE
*/
void readInput() {
   upBtm    = analogRead(A2);
   leftBtm  = analogRead(A5);
   downBtm  = analogRead(A4);
   rightBtm = analogRead(A3);
   pauseBtm = analogRead(A1);
}
/* !
*  SHOW INFO
*/
void showInfo() {
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(BLACK);
   //! write in the position (0,0) of the screen
   display.setCursor(0,0);
   display.println("Snake v1.1");
   display.setCursor(0,20);
   display.print("Developed by: Ouarrak Ayoub");
   //! show the information in the screen
   display.display();
   delay(3500);
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(BLACK);
   display.setCursor(0,0);
   display.println("Enjoy");
   display.display();
   delay(2000);
}