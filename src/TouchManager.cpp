#include "TouchManager.h"
#include "Config.h"
#include "Globals.h"

int tapCounter = 0;
unsigned long lastTapTime = 0;
bool lastPinState = false;
unsigned long pressStartTime = 0;
bool isLongPressHandled = false;

void initTouch() {
  pinMode(TOUCH_PIN, INPUT);
}

void handleTouch() {
  bool currentPinState = digitalRead(TOUCH_PIN);
  unsigned long now = millis();
  
  if (currentPinState && !lastPinState) {
    pressStartTime = now;
    isLongPressHandled = false;
  } else if (currentPinState && lastPinState) {
    if ((now - pressStartTime > LONG_PRESS_TIME) && !isLongPressHandled) {
      lastPageSwitch = now;
      if (currentPage == 0) {
        currentMood++;
        if (currentMood > MOOD_SUSPICIOUS) currentMood = 0;
        lastSaccade = 0;  // Trigger move
      } else if (currentPage == 1) currentPage = 3;
      else if (currentPage == 2) currentPage = 4;
      isLongPressHandled = true;
    }
  } else if (!currentPinState && lastPinState) {
    if ((now - pressStartTime < LONG_PRESS_TIME) && !isLongPressHandled) {
      tapCounter++;
      lastTapTime = now;
    }
  }
  
  lastPinState = currentPinState;
  
  if (tapCounter > 0) {
    if (now - lastTapTime > DOUBLE_TAP_DELAY) {
      lastPageSwitch = now;
      if (tapCounter == 2) {
        highBrightness = !highBrightness;
        display.setContrast(highBrightness ? 255 : 1);
        display.display();
      } else if (tapCounter == 1) {
        if (currentPage == 3) currentPage = 1;
        else if (currentPage == 4) currentPage = 2;
        else {
          currentPage++;
          if (currentPage > 2) currentPage = 0;
        }
      }
      tapCounter = 0;
    }
  }
}
