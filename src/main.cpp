#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "Config.h"
#include "Globals.h"
#include "WifiManager.h"
#include "WeatherManager.h"
#include "TouchManager.h"
#include "DisplayManager.h"
#include "LogManager.h"

#ifndef SERIAL_DIAG_ONLY
#define SERIAL_DIAG_ONLY 0
#endif

void setup() {
#if SERIAL_DIAG_ONLY
  Serial.begin(115200);
  delay(3000);
  Serial.println("BOOT STARTED");
  return;
#else
  LOG_BEGIN(115200);
  LOG_WRITE("Main", "Boot sequence started");
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
  LOG_WRITE("Main", "Configuration loaded");

  if (forceConfig) {
    LOG_WRITE("Main", "Entering config mode by touch hold");
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
    LOG_ERROR("Main", "WiFi connection failed, config portal started");
    return; // Already started config portal in connectWifi()
  }
  LOG_WRITE("Main", "WiFi connected");
  
  configTime(0, 0, ntpServer);
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  getWeatherAndForecast();
  lastWeatherUpdate = millis();
  lastPageSwitch = millis();
#endif
}

void loop() {
#if SERIAL_DIAG_ONLY
  Serial.println("RUNNING");
  delay(1000);
  return;
#else
  handleWifiPortal();
  if (inConfigMode) {
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
#endif
}
