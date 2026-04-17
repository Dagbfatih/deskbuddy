#include "LogManager.h"

namespace LogManager {

void begin(unsigned long baud) {
  Serial.begin(baud);
  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000)) {
    delay(10);
  }
}

void write(const String& source, const String& message) {
  Serial.print("[");
  Serial.print(source);
  Serial.print("] ");
  Serial.println(message);
}

void write(const char* source, const char* message) {
  write(String(source), String(message));
}

void error(const String& source, const String& message) {
  write(source, String("ERROR: ") + message);
}

void error(const char* source, const char* message) {
  error(String(source), String(message));
}

}
