#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

struct ForecastDay {
  String dayName;
  int temp;
  String iconType;
};

extern int currentPage;
extern bool highBrightness;
extern int currentMood;

extern String city;
extern String countryCode;
extern String apiKey;
extern String wifiSsid;
extern String wifiPass;
extern String tzString;

extern unsigned long lastWeatherUpdate;
extern float temperature;
extern float feelsLike;
extern int humidity;
extern String weatherMain;
extern String weatherDesc;

extern ForecastDay fcast[3];

extern bool inConfigMode;
extern bool offlineMode;
extern const char* ntpServer;

extern unsigned long lastSaccade;
extern unsigned long lastPageSwitch;

extern Adafruit_SH1106G display;
