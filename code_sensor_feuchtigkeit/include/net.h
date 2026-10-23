#pragma once

// WLAN-Verbindung, Fallback-Accesspoint und Uhrzeit (NTP / Browser)

#include "main.h"

// ---------- WLAN ----------
// Verbindet mit dem WLAN; klappt das nicht innerhalb von WIFI_CONNECT_TIMEOUT_MS,
// wird ein eigener Accesspoint geöffnet (SSID: <AP_SSID_PREFIX>-<MAC-Ende>).
void connectWifi();

// In loop() aufrufen: Neuverbindung, AP ein-/ausschalten
void handleWifi();

bool   apActive();
String apSsid();
String networkInfo();   // z.B. "WLAN 192.168.1.50" oder "AP esp-logger-A1B2C3 192.168.4.1"

// ---------- Zeit ----------
void setupTime();

// In loop() aufrufen; meldet einmalig die erste gültige Zeit
void handleTime();

bool timeValid();                  // true, sobald NTP oder Browser die Zeit gesetzt hat
bool timeSynced();                 // wie timeValid(), aber erst nach handleTime()
bool getLocalTm(struct tm& t);
String formatLocalTime(const char* fmt);   // "" wenn keine gültige Zeit

// Uhrzeit von außen setzen (Unix-Zeit in Sekunden, UTC), z.B. vom Browser
bool setTimeFromEpoch(uint32_t epoch);
