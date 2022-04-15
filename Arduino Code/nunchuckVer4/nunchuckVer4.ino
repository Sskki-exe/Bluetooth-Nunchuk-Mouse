#include <Wire.h>
#include <BleMouse.h>
#include "nunchuk.h"
BleMouse bleMouse("Nunchuk Mouse","Sskki",69);

// Clip ground, 3v then white then blue
// Hold boot then press en

float x;
float y;
float xRaw;
float yRaw;
// Dead zones high and low
#define xDzL -12
#define xDzH 8
#define yDzL -8
#define yDzH 3
// Physical limits of joystick
#define xLow -97
#define xHigh 98
#define yLow -92
#define yHigh 85
// Moving and scrolling speeds
#define mSped 10
#define sSped 1

bool isScrolling = false;

int modeI = 0;
#define modeCount 3

void setup() {
    Wire.begin();
    nunchuk_init();
    bleMouse.begin();
}
void loop() {
    if (nunchuk_read()) { // Read Nunchuk data
        xRaw = nunchuk_joystickX();
        yRaw = nunchuk_joystickY();
    }
    if(bleMouse.isConnected()) {
        xyUpdate();
        /*
         * Scrolling
         */
        if (isScrolling){
          if(abs(x)>abs(y)){ // Horizontal Scrolling
            if(x>0){
              bleMouse.move(0,0,0,1);
            } else if(x<0){
              bleMouse.move(0,0,0,-1);
            }
            delay(100/abs(x));
          } else if(abs(x)<abs(y)){ // Vertical Scrolling
            if(y>0){
              bleMouse.move(0,0,1,0);
            } else if(y<0){
              bleMouse.move(0,0,-1,0);
            }
            delay(100/abs(y));
          }
        } else { // Moving
          bleMouse.move(x,y);
        }
        /*
         * Change Mode Button
         */
        if(nunchuk_buttonC()){
          modeI = modeI + 1;
          if (modeI == modeCount){modeI = 0;}
          if (modeI == 1){ // Mode 1 is scrolling
            isScrolling = true;
          } else {
            isScrolling = false;
          }
          // Release all buttons
          bleMouse.release(MOUSE_LEFT);
          bleMouse.release(MOUSE_RIGHT);
          bleMouse.release(MOUSE_MIDDLE);
          delay(500);
        }
        /*
         * Action Button
         */
        switch (modeI){
          case 0:
            if(nunchuk_buttonZ()){
              bleMouse.press(MOUSE_LEFT);
            } else {
              bleMouse.release(MOUSE_LEFT);
            }
            break;
          case 1:
            if(nunchuk_buttonZ()){
              bleMouse.press(MOUSE_RIGHT);
              isScrolling = false;
            } else {
              bleMouse.release(MOUSE_RIGHT);
              isScrolling = true;
            }
            break;
          case 2:
            if(nunchuk_buttonZ()){
              bleMouse.press(MOUSE_MIDDLE);
            } else {
              bleMouse.release(MOUSE_MIDDLE);
            }
            break;
        }
    }
    delay(10);
}
void xyUpdate() {
  if(xDzL<xRaw and xDzH>xRaw){ // Deadzone
    x = 0;
  } else {
    if (isScrolling){ // Scrolling
      x = map(xRaw,xLow,xHigh,-sSped,sSped);
    } else { // Move
      x = map(xRaw,xLow,xHigh,-mSped,mSped);
    }        
  }
  if(yDzL<yRaw and yDzH>yRaw){ // Deadzone
    y = 0;
  } else {
    if (isScrolling){ // Scrolling
      y = map(yRaw,yLow,yHigh,-sSped,sSped);
    } else { // Move
      y = map(yRaw,yLow,yHigh,mSped,-mSped);
    }
  }
}
