#pragma once

// Dateisystem (LittleFS) und CSV-Logging

#include "main.h"

void setupStorage();

// In loop() aufrufen; schreibt alle LOG_INTERVAL_MS eine Zeile (nur mit gültiger Zeit)
void handleLogging();

// Schreibt sofort eine CSV-Zeile mit frischen Messwerten
void writeLogLine();

void   fsUsage(size_t& used, size_t& total);
String normPath(const String& name);        // "datei" -> "/datei"
bool   isSafeLogPath(const String& path);   // nur /log_*.csv, kein ".."
String fmtFloat(float v, int decimals = 2); // NaN -> "", ggf. Dezimalkomma
