#pragma once

// OTA-Update über WLAN (ArduinoOTA), startet auch mDNS (<OTA_HOSTNAME>.local)

#include "main.h"

void setupOta();
void handleOta();   // in loop() aufrufen
