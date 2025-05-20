#include <ESP8266WiFi.h>
#include <time.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CURRENT_TIME_BUTTON   D5
#define PREVIOUS_TIME_BUTTON  D6
#define BUZZER_PIN            D7

const char* ssid     = "hotspot";
const char* password = "11111111";

String previousTime       = "N/A";
unsigned long lastCurrentPress = 0;
unsigned long lastPrevPress = 0;
bool waitingForCurrentSecondPress = false;
bool waitingForPrevSecondPress = false;

unsigned long lastBuzzerTime = 0;
const unsigned long buzzerInterval = 120000;

unsigned long lastAutoUpdateTime = 0;
const unsigned long autoUpdateInterval = 60000; // 1 minute

#define EEPROM_SIZE 64
#define EEPROM_ADDR 0

void setup() {
  Serial.begin(115200);
  pinMode(CURRENT_TIME_BUTTON, INPUT_PULLUP);
  pinMode(PREVIOUS_TIME_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  EEPROM.begin(EEPROM_SIZE);
  previousTime = readPreviousTime();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true); // Halt if display not found
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  WiFi.begin(ssid, password);
  displayMessage("Connecting WiFi...", "", false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  displayMessage("WiFi Connected!", "", false);
  delay(1000);
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
  displayMessage("Ready.", "Press a button.", false);

  lastAutoUpdateTime = millis(); // Initialize auto-update timer
}

void loop() {
  // Auto-update current time every minute
  if (millis() - lastAutoUpdateTime >= autoUpdateInterval) {
    String currentTime = getTimeString();
    displayMessage("Current Time:", currentTime, true);
    lastAutoUpdateTime = millis();
  }

  // Buzzer logic
  if (millis() - lastBuzzerTime >= buzzerInterval) {
    ringBuzzer();
    lastBuzzerTime = millis();
  }

  // Current Time Button Logic
  if (digitalRead(CURRENT_TIME_BUTTON) == LOW) {
    delay(200);
    String currentTime = getTimeString();
    displayMessage("Current Time:", currentTime, true);

    if (waitingForCurrentSecondPress && millis() - lastCurrentPress < 2000) {
      previousTime = currentTime;
      savePreviousTime(previousTime);
      displayMessage("Time Updated!", "", false);
      waitingForCurrentSecondPress = false;
    } else {
      lastCurrentPress = millis();
      waitingForCurrentSecondPress = true;
    }
    while (digitalRead(CURRENT_TIME_BUTTON) == LOW);
  }

  // Previous Time Button Logic
  if (digitalRead(PREVIOUS_TIME_BUTTON) == LOW) {
    delay(200);
    if (waitingForPrevSecondPress && millis() - lastPrevPress < 2000) {
      String nextTime = calculateNextTime(previousTime, 8);
      displayMessage("Next Time:", nextTime, true);
      waitingForPrevSecondPress = false;
    } else {
      displayMessage("Previous Time:", previousTime, true);
      lastPrevPress = millis();
      waitingForPrevSecondPress = true;
    }
    while (digitalRead(PREVIOUS_TIME_BUTTON) == LOW);
  }

  if (waitingForCurrentSecondPress && millis() - lastCurrentPress > 2000)
    waitingForCurrentSecondPress = false;

  if (waitingForPrevSecondPress && millis() - lastPrevPress > 2000)
    waitingForPrevSecondPress = false;
}

String getTimeString() {
  time_t now = time(nullptr);
  struct tm* tm_info = localtime(&now);
  char buf[6]; // HH:MM + null
  strftime(buf, sizeof(buf), "%H:%M", tm_info);
  return String(buf);
}

void displayMessage(const String& label, const String& value, bool showLarge) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(label);
  if (showLarge) {
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println(value);
  }
  display.display();
}

void ringBuzzer() {
  for (int i = 0; i < 10; i++) {
    tone(BUZZER_PIN, 2000);
    delay(200);
    noTone(BUZZER_PIN);
    delay(200);
  }
}

void savePreviousTime(const String& timeStr) {
  for (int i = 0; i < timeStr.length() && i < EEPROM_SIZE; i++) {
    EEPROM.write(EEPROM_ADDR + i, timeStr[i]);
  }
  EEPROM.write(EEPROM_ADDR + timeStr.length(), '\0');
  EEPROM.commit();
}

String readPreviousTime() {
  char buffer[EEPROM_SIZE];
  for (int i = 0; i < EEPROM_SIZE; i++) {
    buffer[i] = EEPROM.read(EEPROM_ADDR + i);
    if (buffer[i] == '\0') break;
  }
  buffer[EEPROM_SIZE - 1] = '\0';
  return String(buffer);
}

String calculateNextTime(const String& timeStr, int hoursToAdd) {
  if (timeStr == "N/A") return "N/A";

  int h, m;
  if (sscanf(timeStr.c_str(), "%d:%d", &h, &m) != 2) {
    return "Invalid";
  }

  struct tm timeinfo = {0};
  timeinfo.tm_hour = h;
  timeinfo.tm_min = m;
  timeinfo.tm_sec = 0;

  time_t t = mktime(&timeinfo);
  t += hoursToAdd * 3600;

  struct tm* newTime = localtime(&t);
  char buffer[6]; // HH:MM
  strftime(buffer, sizeof(buffer), "%H:%M", newTime);
  return String(buffer);
}