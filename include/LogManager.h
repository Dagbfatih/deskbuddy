#pragma once

#include <Arduino.h>

namespace LogManager {
void begin(unsigned long baud = 115200);
void write(const String& source, const String& message);
void write(const char* source, const char* message);
void error(const String& source, const String& message);
void error(const char* source, const char* message);
}

#ifndef WRITE_LOGS
#define WRITE_LOGS 0
#endif

#if WRITE_LOGS
#define LOG_BEGIN(baud) do { LogManager::begin(baud); } while (0)
#define LOG_WRITE(source, message) do { LogManager::write((source), (message)); } while (0)
#define LOG_ERROR(source, message) do { LogManager::error((source), (message)); } while (0)
#else
#define LOG_BEGIN(baud) do { } while (0)
#define LOG_WRITE(source, message) do { } while (0)
#define LOG_ERROR(source, message) do { } while (0)
#endif
