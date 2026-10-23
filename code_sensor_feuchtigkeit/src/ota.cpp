#include "ota.h"

void setupOta() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    bool fw = ArduinoOTA.getCommand() == U_FLASH;
    Serial.println(fw ? "OTA: Firmware" : "OTA: Dateisystem");
    if (!fw) LittleFS.end();   // Dateisystem wird überschrieben -> vorher aushängen
  });
  ArduinoOTA.onEnd([]() { Serial.println("\nOTA fertig, Neustart"); });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
    Serial.printf("OTA: %u%%\r", done * 100 / total);
  });
  ArduinoOTA.onError([](ota_error_t e) { Serial.printf("OTA-Fehler %u\n", e); });

  ArduinoOTA.begin();
}

void handleOta() {
  ArduinoOTA.handle();
}
