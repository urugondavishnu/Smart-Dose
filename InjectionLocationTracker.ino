#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32  
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define button D5

const int countAddress = 0;
const int lastUsed1Address = 10;
const int lastUsed2Address = 20;

int count = 0;
int lastUsed1 = -1;
int lastUsed2 = -1;
String location;

unsigned long lastPressTime = 0;
bool savedDisplayed = false;

void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  EEPROM.begin(512);
  EEPROM.get(countAddress, count);
  EEPROM.get(lastUsed1Address, lastUsed1);
  EEPROM.get(lastUsed2Address, lastUsed2);

  if (count < 0 || count > 5) count = 0;
  if (lastUsed1 < -1 || lastUsed1 > 5) lastUsed1 = -1;
  if (lastUsed2 < -1 || lastUsed2 > 5) lastUsed2 = -1;

  skipLastUsed();
  updateLocation();

  // Show Last Location in BIGGER text
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Last Location:");
  display.setTextSize(2);
  display.setCursor(0, 14);
  String lastLocation;
  switch (lastUsed1) {
    case 0: lastLocation = "L Thigh"; break;
    case 1: lastLocation = "R Thigh"; break;
    case 2: lastLocation = "L Abdomen"; break;
    case 3: lastLocation = "R Abdomen"; break;
    case 4: lastLocation = "R Shoulder"; break;
    case 5: lastLocation = "L Shoulder"; break;
    default: lastLocation = "N/A"; break;
  }
  display.println(lastLocation);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  if (digitalRead(button) == LOW) {
    do {
      count = (count + 1) % 6;
    } while (count == lastUsed1 || count == lastUsed2);  // Skip last two used

    updateLocation();
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Location:");
    display.setTextSize(2);
    display.setCursor(0, 12);
    display.println(location);
    display.display();

    lastPressTime = millis();
    savedDisplayed = false;
    delay(500);
  }

  if (!savedDisplayed && millis() - lastPressTime > 7000) {
    // Save count as new lastUsed1, shift previous lastUsed1 to lastUsed2
    EEPROM.put(countAddress, count);
    EEPROM.put(lastUsed2Address, lastUsed1);
    EEPROM.put(lastUsed1Address, count);
    EEPROM.commit();

    lastUsed2 = lastUsed1;
    lastUsed1 = count;

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("Saved!");
    display.display();
    delay(1000);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Location:");
    display.setTextSize(2);
    display.setCursor(0, 12);
    display.println(location);
    display.display();

    savedDisplayed = true;
  }
}

void updateLocation() {
  switch (count) {
    case 0: location = "L Thigh"; break;
    case 1: location = "R Thigh"; break;
    case 2: location = "L Abdomen"; break;
    case 3: location = "R Abdomen"; break;
    case 4: location = "R Shoulder"; break;
    case 5: location = "L Shoulder"; break;
  }
}

void skipLastUsed() {
  if (count == lastUsed1 || count == lastUsed2) {
    do {
      count = (count + 1) % 6;
    } while (count == lastUsed1 || count == lastUsed2);
  }
}