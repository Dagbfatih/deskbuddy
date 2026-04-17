#include "WeatherManager.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "Globals.h"
#include "Config.h"
#include "LogManager.h"

void updateMoodBasedOnWeather() {
  int m = MOOD_NORMAL;
  if (weatherMain == "Clear") m = MOOD_HAPPY;
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") m = MOOD_SAD;
  else if (weatherMain == "Thunderstorm") m = MOOD_SURPRISED;
  else if (weatherMain == "Clouds") m = MOOD_NORMAL;
  else if (temperature > 25) m = MOOD_EXCITED;
  else if (temperature < 5) m = MOOD_SLEEPY;
  currentMood = m;
}

void getWeatherAndForecast() {
  if (WiFi.status() != WL_CONNECTED) {
    LOG_ERROR("Weather", "Skipped update because WiFi is not connected");
    return;
  }
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  int weatherCode = http.GET();
  if (weatherCode == 200) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      temperature = doc["main"]["temp"].as<double>();
      feelsLike = doc["main"]["feels_like"].as<double>();
      humidity = doc["main"]["humidity"].as<int>();
      weatherMain = doc["weather"][0]["main"].as<String>();
      weatherDesc = doc["weather"][0]["description"].as<String>();
      if (weatherDesc.length() > 0) {
        weatherDesc[0] = toupper(weatherDesc[0]);
      }
      updateMoodBasedOnWeather();
      LOG_WRITE("Weather", "Current weather updated");
    } else {
      LOG_ERROR("Weather", String("Failed to parse current weather JSON: ") + error.c_str());
    }
  } else {
    LOG_ERROR("Weather", String("Current weather HTTP error: ") + weatherCode);
  }
  http.end();
  
  url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  int forecastCode = http.GET();
  if (forecastCode == 200) {
    String payload = http.getString();
    JsonDocument fo;
    DeserializationError error = deserializeJson(fo, payload);

    if (!error) {
      struct tm t;
      getLocalTime(&t);
      int today = t.tm_wday;
      const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
      int indices[3] = { 7, 15, 23 };
      for (int i = 0; i < 3; i++) {
        int idx = indices[i];
        fcast[i].temp = fo["list"][idx]["main"]["temp"].as<int>();
        fcast[i].iconType = fo["list"][idx]["weather"][0]["main"].as<String>();
        int nextDayIndex = (today + i + 1) % 7;
        fcast[i].dayName = days[nextDayIndex];
      }
      LOG_WRITE("Weather", "Forecast updated");
    } else {
      LOG_ERROR("Weather", String("Failed to parse forecast JSON: ") + error.c_str());
    }
  } else {
    LOG_ERROR("Weather", String("Forecast HTTP error: ") + forecastCode);
  }
  http.end();
}
