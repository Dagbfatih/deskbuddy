#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "Config.h"
#include "Globals.h"
#include "WifiManager.h"
#include "WeatherManager.h"
#include "TouchManager.h"
#include "DisplayManager.h"

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  initTouch();
  display.begin(0x3C, true);
  display.setTextColor(SH110X_WHITE);

  // Hold touch for 3 sec at boot to force config mode
  bool forceConfig = false;
  for (unsigned long t = millis(); millis() - t < CONFIG_HOLD_MS; ) {
    if (digitalRead(TOUCH_PIN)) { forceConfig = true; break; }
    delay(80);
  }

  loadConfig();

  if (forceConfig) {
    startConfigPortal();
    return;
  }

  initDisplay();
  playBootAnimation();

  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(40, 30);
  display.print("connecting");
  display.display();
  
  connectWifi();
  
  if (WiFi.status() != WL_CONNECTED) {
    return; // Already started config portal in connectWifi()
  }
  
  configTime(0, 0, ntpServer);
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  getWeatherAndForecast();
  lastWeatherUpdate = millis();
  lastPageSwitch = millis();
}

void loop() {
  handleWifiPortal();

  static unsigned long skipHoldStart = 0;
  static bool wasTouchedConfig = false;

  if (inConfigMode) {
    bool isTouched = digitalRead(TOUCH_PIN);
    if (isTouched && !wasTouchedConfig) {
      skipHoldStart = millis();
    }
    if (isTouched && (millis() - skipHoldStart > 3000)) {
      // Offline mode trigger from hardware button
      inConfigMode = false;
      offlineMode = true;
      WiFi.softAPdisconnect(true);
      display.clearDisplay();
      display.setFont(NULL);
      display.setCursor(20, 30);
      display.print("Offline Mode");
      display.display();
      delay(1000);
      lastPageSwitch = millis();
      currentPage = 0;
    }
    wasTouchedConfig = isTouched;
    return;
  }

  unsigned long now = millis();
  handleTouch();
  
  if (!offlineMode) {
    if (now - lastWeatherUpdate > 600000) {
      getWeatherAndForecast();
      lastWeatherUpdate = now;
    }

    if (currentPage < 3 && now - lastPageSwitch > PAGE_INTERVAL) {
      currentPage++;
      if (currentPage > 2) currentPage = 0;
      lastPageSwitch = now;
      lastSaccade = 0;
    }
  } else {
    // If offline, lock to Emo Page (0)
    currentPage = 0;
  }

  display.clearDisplay();
  switch (currentPage) {
    case 0: drawEmoPage(); break;
    case 1: drawClock(); break;
    case 2: drawWeatherCard(); break;
    case 3: drawWorldClock(); break;
    case 4: drawForecastPage(); break;
  }
  display.display();
}
