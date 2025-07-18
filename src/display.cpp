/*
 * OLED Display Modul Implementierung
 * 0,96" SSD1306 128x64 I2C OLED Display
 */

#include "display.h"
#include "sensors.h"
#include "gps_module.h" 
#include "rtc_module.h"

// ==============================================
// GLOBALE VARIABLEN
// ==============================================

Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

static uint8_t currentPage = 0;
static unsigned long lastPageChange = 0;

// ==============================================
// DISPLAY GRUNDFUNKTIONEN
// ==============================================

bool initDisplay() {
  DEBUG_PRINTLN(F("Initialisiere OLED Display..."));
  
  // SSD1306 Display initialisieren
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    DEBUG_PRINTLN(F("FEHLER: SSD1306 OLED nicht gefunden!"));
    DEBUG_PRINTLN(F("Prüfe I2C Verkabelung:"));
    DEBUG_PRINTLN(F("- VCC → 3.3V oder 5V"));
    DEBUG_PRINTLN(F("- GND → GND"));
    DEBUG_PRINTLN(F("- SDA → Pin 20 (Arduino Mega)"));
    DEBUG_PRINTLN(F("- SCL → Pin 21 (Arduino Mega)"));
    return false;
  }
  
  // Display löschen und Startmeldung anzeigen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Umweltkontroll-"));
  display.println(F("system"));
  display.println();
  display.println(F("Initialisiere..."));
  display.display();
  
  DEBUG_PRINTLN(F("OLED Display erfolgreich initialisiert"));
  return true;
}

void clearDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
}

void updateDisplay() {
  // Seite alle 2 Sekunden wechseln
  if (millis() - lastPageChange >= OLED_UPDATE_INTERVAL) {
    nextDisplayPage();
    lastPageChange = millis();
  }
}

// ==============================================
// DISPLAY SEITEN
// ==============================================

void displayPage1_Status() {
  clearDisplay();
  displayTitle("1. SYSTEM STATUS");
  
  // Aktuelle Zeit von RTC abrufen mit Debug
  RTCData currentTime;
  char timeStr[20];
  bool rtcSuccess = readRTCData(&currentTime);
  
  DEBUG_PRINT(F("RTC Success: "));
  DEBUG_PRINT(rtcSuccess);
  DEBUG_PRINT(F(", isValid: "));
  DEBUG_PRINT(currentTime.isValid);
  DEBUG_PRINT(F(", Hour: "));
  DEBUG_PRINTLN(currentTime.hour);
  
  if (rtcSuccess && currentTime.isValid) {
    snprintf(timeStr, sizeof(timeStr), "Zeit: %02d:%02d:%02d", 
             currentTime.hour, currentTime.minute, currentTime.second);
  } else {
    strcpy(timeStr, "Zeit: --:--:--");
  }
  displayText(0, timeStr);
  
  // System-Info (einheitliche Abstände)
  displayText(1, "RAM: OK");
  displayText(2, "SD: OK"); 
  displayText(3, "Sensoren: 6/7");
  
  display.display();
}

void displayPage2_Temperature() {
  clearDisplay();
  displayTitle("2. TEMPERATUR");
  
  // DHT11 Werte lesen (einheitliche Abstände)
  float temperature, humidity;
  if (readDHTSensor(&temperature, &humidity)) {
    displayValue(0, "Temp:", temperature, "C");
    displayValue(1, "Luft:", humidity, "%");
  } else {
    displayText(0, "DHT11: FEHLER");
  }
  
 
  
  display.display();
}

void displayPage3_Environment() {
  clearDisplay();
  displayTitle("3. UMGEBUNG");
  
  // Lichtsensor (einheitliche Abstände)
  float lightPercent = getLightPercent();
  displayValue(0, "Licht:", lightPercent, "%");
  
  // Radioaktivität
  int radiation = getRadiationCount();
  displayValue(1, "Radiat:", radiation, "cps");
  
  display.display();
}

void displayPage4_Gas() {
  clearDisplay();
  displayTitle("4. GAS-SENSOREN");
  
  // 3 wichtigste Gas-Sensoren (einheitliche Abstände)
  int gasSensors[9];
  readAllGasSensors(gasSensors);
  
  displayValue(0, "MQ2:", gasSensors[0], "");    // Methan/LPG
  displayValue(1, "MQ7:", gasSensors[5], "");    // CO
  displayValue(2, "MQ135:", gasSensors[8], "");  // Luftqualität
  
  // Durchschnitt aller Sensoren
  int average = 0;
  for (int i = 0; i < 9; i++) {
    average += gasSensors[i];
  }
  average /= 9;
  displayValue(3, "Avg:", average, "");
  
  display.display();
}

void displayPage5_Audio() {
  clearDisplay();
  displayTitle("5. MIKROFONE");
  
  // Mikrofon-Werte lesen (einheitliche Abstände)
  int micValues[2];
  readAllMicrophones(micValues);
  
  displayValue(0, "Klein:", micValues[0], "");
  displayValue(1, "Gross:", micValues[1], "");
  
  display.display();
}

void displayPage6_GPS() {
  clearDisplay();
  displayTitle("6. GPS POSITION");
  
  // Zeige immer die letzten bekannten, gültigen GPS-Daten an
  extern GPSData currentGPSData; // aus gps_module.cpp

  DEBUG_PRINT(F("Display GPS: isValid: "));
  DEBUG_PRINT(currentGPSData.isValid);
  DEBUG_PRINT(F(", Sats: "));
  DEBUG_PRINTLN(currentGPSData.satellites);

  if (currentGPSData.isValid) {
    displayText(0, "GPS: AKTIV");
    char latStr[16], lonStr[16], satStr[16];
    snprintf(latStr, sizeof(latStr), "Lat: %.4f", currentGPSData.latitude);
    snprintf(lonStr, sizeof(lonStr), "Lon: %.4f", currentGPSData.longitude);
    snprintf(satStr, sizeof(satStr), "Sats: %d", currentGPSData.satellites);
    displayText(1, latStr);
    displayText(2, lonStr);
    displayText(3, satStr);
  } else {
    displayText(0, "GPS: Suche...");
    displayText(1, "Lat: ---.----");
    displayText(2, "Lon: ---.----");
    char debugStr[16];
    snprintf(debugStr, sizeof(debugStr), "Dbg:--/0");
    displayText(3, debugStr);
  }
  
  display.display();
}

// ==============================================
// HILFSFUNKTIONEN
// ==============================================

void displayTitle(const char* title) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(title);
  display.drawLine(0, 10, OLED_SCREEN_WIDTH, 10, SSD1306_WHITE);
}

void displayValue(int line, const char* label, float value, const char* unit) {
  display.setCursor(0, 17 + (line * 10));  // 3 Pixel nach unten verschoben (14+3=17)
  display.print(label);
  display.print(" ");
  display.print(value, 1);
  display.print(" ");
  display.print(unit);
}

void displayText(int line, const char* text) {
  display.setCursor(0, 17 + (line * 10));  // 3 Pixel nach unten verschoben (14+3=17)
  display.print(text);
}

void nextDisplayPage() {
  currentPage++;
  if (currentPage > 5) currentPage = 0;
  
  switch (currentPage) {
    case 0: displayPage1_Status(); break;
    case 1: displayPage2_Temperature(); break;
    case 2: displayPage3_Environment(); break;
    case 3: displayPage4_Gas(); break;
    case 4: displayPage5_Audio(); break;
    case 5: displayPage6_GPS(); break;
  }
}
