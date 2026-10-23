/*
 * ESP8266 (NodeMCU v2) CSV-Logger mit NTP-Zeitstempel (Analogsensor an A0)
 * --------------------------------------------------------------------------
 * - WLAN + NTP (MEZ/MESZ), Fallback-AP, Browser-Zeit       -> net.cpp
 * - Bodenfeuchte (resistiv) an A0, Live-Werte alle 2 s     -> sensors.cpp
 * - CSV pro Tag in LittleFS (/log_YYYY-MM-DD.csv),
 *   älteste Datei wird bei ~90 % Belegung gelöscht          -> storage.cpp
 * - Webseite mit Live-Werten, Dateiliste, JSON-API          -> web.cpp
 * - OTA-Update über WLAN                                    -> ota.cpp
 *
 * Einstellungen: include/config.h, Zugangsdaten: secrets.ini
 */

#include "main.h"
#include "net.h"
#include "sensors.h"
#include "storage.h"
#include "web.h"
#include "ota.h"

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();

  setupStorage();
  setupSensors();
  connectWifi();
  setupTime();
  setupWebServer();
  setupOta();
}

void loop() {
  handleOta();
  handleWifi();
  handleTime();
  handleSensors();
  handleLogging();
}
