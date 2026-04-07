#include "Globals.h"
#include "Config.h"

int currentPage = 0;
bool highBrightness = true;
int currentMood = MOOD_NORMAL;

String city;
String countryCode;
String apiKey;
String wifiSsid;
String wifiPass;
String tzString;

unsigned long lastWeatherUpdate = 0;
float temperature = 0.0;
float feelsLike = 0.0;
int humidity = 0;
String weatherMain = "Loading";
String weatherDesc = "Wait...";

ForecastDay fcast[3];

bool inConfigMode = false;
bool offlineMode = false;
const char* ntpServer = "pool.ntp.org";

unsigned long lastSaccade = 0;
unsigned long lastPageSwitch = 0;

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
