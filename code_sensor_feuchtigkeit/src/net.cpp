#include "net.h"
#include <sys/time.h>

static bool     s_timeSynced    = false;
static bool     s_apActive      = false;
static uint32_t s_lastConnected = 0;   // millis() der letzten bestehenden Verbindung
static uint32_t s_connectedSince = 0;  // 0 = gerade nicht verbunden
static uint32_t s_lastRetry     = 0;
static String   s_apSsid;

// ===================== WLAN / Accesspoint =====================

// SSID mit den letzten 3 Bytes der MAC, z.B. "esp-logger-A1B2C3"
String apSsid() {
  if (s_apSsid.length() == 0) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char buf[40];
    snprintf(buf, sizeof(buf), "%s-%02X%02X%02X", AP_SSID_PREFIX, mac[3], mac[4], mac[5]);
    s_apSsid = buf;
  }
  return s_apSsid;
}

bool apActive() {
  return s_apActive;
}

static void startAp() {
  if (s_apActive) return;
  WiFi.mode(WIFI_AP_STA);
  bool ok = WiFi.softAP(apSsid().c_str(), AP_PASSWORD);
  s_apActive = ok;
  if (ok)
    Serial.printf("Accesspoint aktiv: %s  ->  http://%s\n",
                  apSsid().c_str(), WiFi.softAPIP().toString().c_str());
  else
    Serial.println("Accesspoint konnte nicht gestartet werden");
}

static void stopAp() {
  if (!s_apActive) return;
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  s_apActive = false;
  Serial.println("Accesspoint aus (WLAN verbunden)");
}

String networkInfo() {
  String s;
  if (WiFi.status() == WL_CONNECTED) s += "WLAN " + WiFi.localIP().toString();
  if (s_apActive) {
    if (s.length()) s += " · ";
    s += "AP " + apSsid() + " " + WiFi.softAPIP().toString();
  }
  return s.length() ? s : String("nicht verbunden");
}

void connectWifi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(OTA_HOSTNAME);
  // Neuverbindung selbst steuern (handleWifi), damit ein laufender AP
  // nicht durch ständiges Kanal-Suchen der Station gestört wird
  WiFi.setAutoReconnect(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("WLAN verbinden");
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Verbunden, IP: %s\n", WiFi.localIP().toString().c_str());
    s_lastConnected = s_connectedSince = millis();
  } else {
    Serial.println("WLAN nicht erreichbar - starte Accesspoint");
    startAp();
  }
  s_lastRetry = millis();
}

void handleWifi() {
  uint32_t now = millis();

  if (WiFi.status() == WL_CONNECTED) {
    if (s_connectedSince == 0) {
      s_connectedSince = now;
      Serial.printf("WLAN verbunden, IP: %s\n", WiFi.localIP().toString().c_str());
    }
    s_lastConnected = now;

    // AP erst abschalten, wenn die Verbindung eine Weile stabil ist
    // und niemand mehr am AP hängt
    if (s_apActive && now - s_connectedSince >= AP_OFF_AFTER_CONNECT_MS &&
        WiFi.softAPgetStationNum() == 0) {
      stopAp();
    }
    return;
  }

  // ---- nicht verbunden ----
  if (s_connectedSince != 0) {
    s_connectedSince = 0;
    Serial.println("WLAN-Verbindung verloren");
  }

  // Längere Zeit ohne WLAN -> AP öffnen
  if (!s_apActive && now - s_lastConnected >= AP_START_AFTER_LOSS_MS) {
    startAp();
  }

  // Regelmäßig neu verbinden - aber nicht, solange jemand am AP hängt,
  // denn die Suche wechselt den Kanal und würfe die AP-Clients raus
  if (now - s_lastRetry >= WIFI_RETRY_INTERVAL_MS) {
    s_lastRetry = now;
    if (!s_apActive || WiFi.softAPgetStationNum() == 0) {
      Serial.println("WLAN: neuer Verbindungsversuch");
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
  }
}

// ===================== Zeit =====================

void setupTime() {
  // Zeitzone + NTP; SNTP synchronisiert danach automatisch regelmäßig nach.
  // Ohne Internet bleibt die Zeitzone trotzdem gesetzt (für die Browser-Zeit).
  configTime(TZ_INFO, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
}

void handleTime() {
  if (!s_timeSynced && timeValid()) {
    s_timeSynced = true;
    Serial.printf("Zeit gueltig: %s\n", formatLocalTime("%d.%m.%Y %H:%M:%S").c_str());
  }
}

bool timeValid() {
  return time(nullptr) > 1700000000;  // nach Nov. 2023 => Zeit wurde gesetzt
}

bool timeSynced() {
  return s_timeSynced;
}

bool getLocalTm(struct tm& t) {
  if (!timeValid()) return false;
  time_t now = time(nullptr);
  localtime_r(&now, &t);
  return true;
}

String formatLocalTime(const char* fmt) {
  struct tm t;
  if (!getLocalTm(t)) return "";
  char buf[40];
  strftime(buf, sizeof(buf), fmt, &t);
  return String(buf);
}

bool setTimeFromEpoch(uint32_t epoch) {
  if (epoch < 1700000000UL) return false;   // offensichtlich ungültig
  struct timeval tv = { (time_t)epoch, 0 };
  settimeofday(&tv, nullptr);
  Serial.printf("Zeit vom Browser gesetzt: %s\n", formatLocalTime("%d.%m.%Y %H:%M:%S").c_str());
  return true;
}
