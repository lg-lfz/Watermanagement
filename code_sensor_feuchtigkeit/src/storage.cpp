#include "storage.h"
#include "net.h"
#include "sensors.h"

static uint32_t s_lastLog = 0;

void setupStorage() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS nicht lesbar - formatiere...");
    LittleFS.format();
    LittleFS.begin();
  }
}

void fsUsage(size_t& used, size_t& total) {
  FSInfo info;
  LittleFS.info(info);
  used  = info.usedBytes;
  total = info.totalBytes;
}

String normPath(const String& name) {
  return name.startsWith("/") ? name : "/" + name;
}

bool isSafeLogPath(const String& p) {
  return p.startsWith("/log_") && p.endsWith(".csv") && p.indexOf("..") < 0;
}

String fmtFloat(float v, int decimals) {
  if (isnan(v)) return "";            // fehlender Messwert -> leeres Feld
  String s = String(v, decimals);
  if (DECIMAL_COMMA) s.replace('.', ',');
  return s;
}

// Löscht die älteste Logdatei, solange das Dateisystem zu voll ist
static void deleteOldestIfFull() {
  size_t used, total;
  fsUsage(used, total);
  while (total > 0 && (float)used / total > MAX_FS_USAGE) {
    String oldest = "";
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      String p = normPath(dir.fileName());
      if (isSafeLogPath(p) && (oldest == "" || p < oldest)) oldest = p;
    }
    if (oldest == "") return;  // nichts mehr zum Löschen
    Serial.printf("Speicher voll - loesche %s\n", oldest.c_str());
    LittleFS.remove(oldest);
    fsUsage(used, total);
  }
}

void writeLogLine() {
  struct tm t;
  if (!getLocalTm(t)) {
    Serial.println("Keine gueltige Zeit - Eintrag uebersprungen");
    return;
  }

  char path[32], date[11], clock[9];
  strftime(path,  sizeof(path),  "/log_%Y-%m-%d.csv", &t);
  strftime(date,  sizeof(date),  "%Y-%m-%d", &t);
  strftime(clock, sizeof(clock), "%H:%M:%S", &t);

  deleteOldestIfFull();

  bool isNew = !LittleFS.exists(path);
  File f = LittleFS.open(path, "a");
  if (!f) {
    Serial.printf("Konnte %s nicht oeffnen\n", path);
    return;
  }

  if (isNew) {
    f.print("Datum"); f.print(CSV_SEP); f.print("Uhrzeit");
    for (int i = 0; i < NUM_VALUES; i++) { f.print(CSV_SEP); f.print(CSV_HEADER_VALUES[i]); }
    f.print("\r\n");
  }

  updateCurrentValues();   // frisch messen, Live-Anzeige gleich mit aktualisieren

  String line = String(date) + CSV_SEP + clock;
  for (int i = 0; i < NUM_VALUES; i++) line += CSV_SEP + fmtFloat(currentValues[i]);
  line += "\r\n";
  f.print(line);
  f.close();

  Serial.print(path); Serial.print(" <- "); Serial.print(line);
}

void handleLogging() {
  if (timeSynced() && millis() - s_lastLog >= LOG_INTERVAL_MS) {
    s_lastLog = millis();
    writeLogLine();
  }
}
