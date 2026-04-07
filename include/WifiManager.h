#pragma once
#include <Arduino.h>

void loadConfig();
void saveConfig(const String& s, const String& p, const String& ak,
                const String& cty, const String& ctry, const String& tz);
void handleConfigRoot();
void handleConfigSave();
void startConfigPortal();
void connectWifi();
void handleWifiPortal();
