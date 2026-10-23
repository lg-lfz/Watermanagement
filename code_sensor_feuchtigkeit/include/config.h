#pragma once

// =====================================================================
// Zentrale Konfiguration
// =====================================================================

#include <Arduino.h>

// ---------- Zugangsdaten ----------
// Werden per build_flags aus platformio.ini gesetzt und von secrets.ini
// (nicht im Repo) überschrieben. Fallbacks nur, falls ohne Flags gebaut wird.
#ifndef SECRET_WIFI_SSID
  #define SECRET_WIFI_SSID "DEIN_WLAN"
#endif
#ifndef SECRET_WIFI_PASS
  #define SECRET_WIFI_PASS "DEIN_PASSWORT"
#endif
#ifndef SECRET_OTA_PASSWORD
  #define SECRET_OTA_PASSWORD "ota-passwort"
#endif
#ifndef SECRET_AP_PASSWORD
  #define SECRET_AP_PASSWORD "logger1234"   // mind. 8 Zeichen!
#endif

// ---------- Netzwerk / OTA ----------
constexpr const char* WIFI_SSID    = SECRET_WIFI_SSID;
constexpr const char* WIFI_PASS    = SECRET_WIFI_PASS;
constexpr const char* OTA_HOSTNAME = "esp-logger";   // auch WLAN-Hostname / mDNS
constexpr const char* OTA_PASSWORD = SECRET_OTA_PASSWORD;
constexpr uint32_t    WIFI_CONNECT_TIMEOUT_MS = 20000;   // beim Start, danach AP
constexpr uint32_t    WIFI_RETRY_INTERVAL_MS  = 60000;   // Neuverbindungsversuch

// ---------- Fallback-Accesspoint ----------
// SSID = <Prefix>-<letzte 3 Bytes der MAC>, z.B. "esp-logger-A1B2C3"
// Webseite dann unter http://192.168.4.1
constexpr const char* AP_SSID_PREFIX = "esp-logger";
constexpr const char* AP_PASSWORD    = SECRET_AP_PASSWORD;   // mind. 8 Zeichen
constexpr uint32_t    AP_START_AFTER_LOSS_MS  = 120000;  // AP nach 2 min ohne WLAN
constexpr uint32_t    AP_OFF_AFTER_CONNECT_MS = 30000;   // AP aus nach 30 s stabilem WLAN

// ---------- Zeit ----------
constexpr const char* NTP_SERVER1 = "de.pool.ntp.org";
constexpr const char* NTP_SERVER2 = "pool.ntp.org";
constexpr const char* NTP_SERVER3 = "time.cloudflare.com";
constexpr const char* TZ_INFO     = "CET-1CEST,M3.5.0,M10.5.0/3";  // MEZ/MESZ

// ---------- Intervalle ----------
constexpr uint32_t LOG_INTERVAL_MS    = 60000;  // CSV-Eintrag
constexpr uint32_t SAMPLE_INTERVAL_MS = 2000;   // Live-Werte für die Webseite

// ---------- CSV / Speicher ----------
constexpr char  CSV_SEP       = ';';    // ';' passt zu deutschem Excel
constexpr bool  DECIMAL_COMMA = true;   // 21,5 statt 21.5
constexpr float MAX_FS_USAGE  = 0.90f;  // ab 90 % Belegung älteste Datei löschen

// ---------- Feuchtesensor (resistiv, Spannungsteiler an A0) ----------
constexpr int   ADC_SAMPLES = 16;       // Mittelwert gegen Rauschen

// Sensor nur während der Messung bestromen (gegen Korrosion/Elektrolyse der
// Elektroden): Versorgung des Spannungsteilers an diesen GPIO statt an 3,3 V.
// -1 = Sensor hängt dauerhaft an 3,3 V.
constexpr int      MOIST_POWER_PIN   = D5;   // GPIO14
constexpr uint32_t MOIST_SETTLE_MS   = 10;   // Wartezeit nach dem Einschalten

// Kalibrierung: A0-Rohwert bei trockenem Sensor (Luft) und in Wasser.
// Auf der Webseite den Rohwert ablesen und hier eintragen.
// Funktioniert in beide Richtungen (nass > trocken oder nass < trocken).
constexpr int MOIST_RAW_DRY = 1023;
constexpr int MOIST_RAW_WET = 300;

// ---------- Messwerte ----------
// Pro Messwert: CSV-Spaltenname, Anzeigename, Einheit, Nachkommastellen.
// Neue Werte hier eintragen und in sensors.cpp -> readSensors() befüllen.
constexpr const char* CSV_HEADER_VALUES[] = { "Feuchte_%", "A0_Rohwert" };
constexpr const char* VALUE_LABELS[]      = { "Feuchte",   "A0 Rohwert" };
constexpr const char* VALUE_UNITS[]       = { "%",         ""           };
constexpr int         VALUE_DECIMALS[]    = { 1,           1            };  // Rohwert = Mittelwert, daher 1 Stelle

constexpr int NUM_VALUES = sizeof(CSV_HEADER_VALUES) / sizeof(CSV_HEADER_VALUES[0]);

static_assert(sizeof(VALUE_LABELS)   / sizeof(VALUE_LABELS[0])   == NUM_VALUES, "VALUE_LABELS passt nicht zu CSV_HEADER_VALUES");
static_assert(sizeof(VALUE_UNITS)    / sizeof(VALUE_UNITS[0])    == NUM_VALUES, "VALUE_UNITS passt nicht zu CSV_HEADER_VALUES");
static_assert(sizeof(VALUE_DECIMALS) / sizeof(VALUE_DECIMALS[0]) == NUM_VALUES, "VALUE_DECIMALS passt nicht zu CSV_HEADER_VALUES");

static_assert(sizeof(SECRET_AP_PASSWORD) - 1 >= 8, "AP-Passwort muss mindestens 8 Zeichen haben");
