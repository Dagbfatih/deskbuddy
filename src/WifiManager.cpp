#include "WifiManager.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "Globals.h"
#include "Config.h"

Preferences prefs;
WebServer configServer(80);

void loadConfig() {
  prefs.begin("deskbuddy", true);
  wifiSsid    = prefs.getString("ssid", "");
  wifiPass    = prefs.getString("pass", "");
  apiKey      = prefs.getString("apikey", "");
  city        = prefs.getString("city", "");
  countryCode = prefs.getString("country", "");
  tzString    = prefs.getString("tz", "");
  prefs.end();
  if (wifiSsid.isEmpty()) {
    wifiSsid    = "edison science corner";
    wifiPass    = "eeeeeeee";
    apiKey      = "45fcf5807a5920e2006c2b8a077d423f";
    city        = "Idukki";
    countryCode = "IN";
    tzString    = "IST-5:30";
  } else {
    if (apiKey.isEmpty())  apiKey = "45fcf5807a5920e2006c2b8a077d423f";
    if (city.isEmpty())    city = "Idukki";
    if (countryCode.isEmpty()) countryCode = "IN";
    if (tzString.isEmpty()) tzString = "IST-5:30";
  }
}

void saveConfig(const String& s, const String& p, const String& ak,
                const String& cty, const String& ctry, const String& tz) {
  prefs.begin("deskbuddy", false);
  prefs.putString("ssid", s);
  prefs.putString("pass", p);
  prefs.putString("apikey", ak);
  prefs.putString("city", cty);
  prefs.putString("country", ctry);
  prefs.putString("tz", tz);
  prefs.end();
}

void handleConfigRoot() {
  prefs.begin("deskbuddy", true);
  String sSsid = prefs.getString("ssid", "");
  String sApik = prefs.getString("apikey", "");
  String sCity = prefs.getString("city", "Idukki");
  String sCtry = prefs.getString("country", "IN");
  String sTz   = prefs.getString("tz", "IST-5:30");
  prefs.end();

  String html = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>DeskBuddy Config</title>
<style>
body{font-family:sans-serif;max-width:420px;margin:30px auto;padding:24px;background:#0c1929;color:#e8f4fc;}
h1{color:#5ba3f5;margin-bottom:8px;}
input{width:100%;padding:10px;margin:6px 0;border:1px solid #2d4a6f;border-radius:6px;box-sizing:border-box;background:#1a2d47;color:#e8f4fc;}
input:focus{outline:none;border-color:#5ba3f5;}
button{width:100%;padding:12px;background:#3498db;color:#fff;border:none;border-radius:6px;font-size:16px;cursor:pointer;margin-top:16px;}
button:hover{background:#2980b9;}
.btn-offline{background:#e74c3c; margin-top:10px;}
.btn-offline:hover{background:#c0392b;}
label{display:block;margin-top:14px;color:#8ab4e8;font-size:14px;}
.section{margin-top:20px;padding-top:16px;border-top:1px solid #1e3a5f;}
.section-title{color:#5ba3f5;font-size:13px;margin-bottom:8px;}
</style></head><body>
<h1>DeskBuddy Setup</h1>
<form action="/save" method="POST">
<label>WiFi SSID</label><input name="ssid" placeholder="Your WiFi name" value=")rawliteral";
  html += sSsid;
  html += R"rawliteral(">
<label>WiFi Password</label><input name="pass" type="password" placeholder="WiFi password">
<div class="section"><div class="section-title">Weather (OpenWeatherMap)</div>
<label>API Key</label><input name="apikey" placeholder="API key" value=")rawliteral";
  html += sApik;
  html += R"rawliteral(">
<label>City</label><input name="city" placeholder="e.g. London" value=")rawliteral";
  html += sCity;
  html += R"rawliteral(">
<label>Country Code</label><input name="country" placeholder="e.g. IN, US, GB" value=")rawliteral";
  html += sCtry;
  html += R"rawliteral(">
</div>
<div class="section"><div class="section-title">Time</div>
<label>Timezone</label><input name="tz" placeholder="e.g. IST-5:30, EST5EDT" value=")rawliteral";
  html += sTz;
  html += R"rawliteral(">
</div>
<button type="submit">Save &amp; Reboot</button>
<button type="button" class="btn-offline" onclick="window.location.href='/offline'">Skip WiFi (Offline Mode)</button>
</form></body></html>)rawliteral";
  configServer.send(200, "text/html", html);
}

void handleConfigSave() {
  if (!configServer.hasArg("ssid") || configServer.arg("ssid").length() == 0) {
    configServer.send(400, "text/plain", "SSID required");
    return;
  }
  String s   = configServer.arg("ssid");
  String p   = configServer.arg("pass");
  String ak  = configServer.arg("apikey");
  String cty = configServer.arg("city");
  String ctr = configServer.arg("country");
  String tz  = configServer.arg("tz");
  prefs.begin("deskbuddy", true);
  if (ak.isEmpty())  ak  = prefs.getString("apikey", "45fcf5807a5920e2006c2b8a077d423f");
  if (cty.isEmpty()) cty = prefs.getString("city", "Idukki");
  if (ctr.isEmpty()) ctr = prefs.getString("country", "IN");
  if (tz.isEmpty())  tz  = prefs.getString("tz", "IST-5:30");
  prefs.end();
  saveConfig(s, p, ak, cty, ctr, tz);
  configServer.send(200, "text/html",
    "<html><body style='font-family:sans-serif;background:#0c1929;color:#e8f4fc;padding:40px;'>"
    "<h2 style='color:#5ba3f5'>Saved!</h2><p>Rebooting in 2 seconds...</p></body></html>");
  delay(2000);
  ESP.restart();
}

void handleConfigOffline() {
  inConfigMode = false;
  offlineMode = true;
  configServer.send(200, "text/html",
    "<html><body style='font-family:sans-serif;background:#0c1929;color:#e8f4fc;padding:40px;'>"
    "<h2 style='color:#e74c3c'>Offline Mode</h2><p>Robot is starting without internet...</p></body></html>");
  delay(1000);
  WiFi.softAPdisconnect(true);
}

void startConfigPortal() {
  inConfigMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(CONFIG_AP_SSID, CONFIG_AP_PASS);
  configServer.on("/", handleConfigRoot);
  configServer.on("/save", HTTP_POST, handleConfigSave);
  configServer.on("/offline", HTTP_GET, handleConfigOffline);
  configServer.begin();
  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(0, 0);
  display.print("Config mode\n\nConnect to:\n");
  display.print(CONFIG_AP_SSID);
  display.print("\n\nThen open 192.168.4.1\nOr HOLD touch 3s");
  display.display();
}

void connectWifi() {
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart < 15000)) {
    delay(200);
  }
  if (WiFi.status() != WL_CONNECTED) {
    startConfigPortal();
  }
}

void handleWifiPortal() {
  if (inConfigMode) {
    configServer.handleClient();
  }
}
