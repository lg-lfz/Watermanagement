#include "web.h"
#include "net.h"
#include "sensors.h"
#include "storage.h"

static AsyncWebServer server(80);

// ---------- Startseite ----------

static void handleRoot(AsyncWebServerRequest* req) {
  String html =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>ESP Logger</title>"
    "<style>"
    "body{font-family:sans-serif;margin:16px;max-width:640px}"
    ".cards{display:flex;flex-wrap:wrap;gap:12px;margin:12px 0}"
    ".card{border:1px solid #ccc;border-radius:8px;padding:12px 16px;min-width:140px}"
    ".card .l{font-size:.85em;color:#666}"
    ".card .v{font-size:1.8em;font-weight:bold}"
    ".muted{color:#888;font-size:.9em}"
    "td{padding:4px 10px}"
    "</style></head><body><h2>ESP8266 CSV-Logger</h2>"
    "<div class='cards'>";

  // Eine Kachel pro Messwert; Inhalt kommt per JavaScript
  for (int i = 0; i < NUM_VALUES; i++) {
    html += "<div class='card'><div class='l'>" + String(VALUE_LABELS[i]) + "</div>"
            "<div class='v'><span id='v" + String(i) + "'>&ndash;</span> " +
            String(VALUE_UNITS[i]) + "</div></div>";
  }
  html += "</div><p class='muted'>Zeit: <span id='t'>&ndash;</span> &middot; "
          "Messung vor <span id='age'>&ndash;</span> s<br>"
          "Netz: <span id='net'>&ndash;</span></p>"
          "<p id='tsbox' style='display:none'>"
          "<button onclick='setTime()'>Uhrzeit vom Browser übernehmen</button> "
          "<span class='muted' id='tsmsg'></span></p>";

  // Dateiliste
  size_t used, total;
  fsUsage(used, total);
  html += "<h3>Logdateien</h3><p class='muted'>Speicher: " + String(used / 1024) + " / " +
          String(total / 1024) + " KB</p><table>";

  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    String p = normPath(dir.fileName());
    if (isSafeLogPath(p)) {
      html += "<tr><td>" + p.substring(1) + "</td><td>" + String(dir.fileSize()) + " B</td>"
              "<td><a href='/download?f=" + p + "'>Download</a></td>"
              "<td><a href='/delete?f=" + p + "' onclick=\"return confirm('Löschen?')\">Löschen</a></td></tr>";
    }
  }

  // Live-Aktualisierung über /api/values
  html += "</table>"
    "<script>"
    "function upd(){fetch('/api/values').then(r=>r.json()).then(d=>{"
      "document.getElementById('t').textContent=d.time||'nicht synchronisiert';"
      "document.getElementById('age').textContent=d.age_s;"
      "document.getElementById('net').textContent=d.net;"
      "document.getElementById('tsbox').style.display=(!d.time||d.ap)?'block':'none';"
      "d.values.forEach((x,i)=>{document.getElementById('v'+i).textContent="
        "x.value===null?'–':x.value.toLocaleString('de-DE',"
        "{minimumFractionDigits:x.dec,maximumFractionDigits:x.dec});});"
    "}).catch(()=>{document.getElementById('t').textContent='keine Verbindung';});}"
    "function setTime(){fetch('/api/settime',{method:'POST',"
      "body:new URLSearchParams({epoch:Math.floor(Date.now()/1000)})})"
      ".then(r=>{document.getElementById('tsmsg').textContent=r.ok?'gesetzt':'Fehler';upd();});}"
    "upd();setInterval(upd," + String(SAMPLE_INTERVAL_MS) + ");"
    "</script></body></html>";

  req->send(200, "text/html; charset=utf-8", html);
}

// ---------- JSON-API ----------

static void handleApiValues(AsyncWebServerRequest* req) {
  String now = formatLocalTime("%d.%m.%Y %H:%M:%S");

  String json = "{\"time\":";
  json += now.length() ? "\"" + now + "\"" : String("null");
  json += ",\"net\":\"" + networkInfo() + "\"";
  json += ",\"ap\":" + String(apActive() ? "true" : "false");
  json += ",\"age_s\":" + String(haveValues ? (millis() - currentValuesMillis) / 1000 : 0);
  json += ",\"values\":[";
  for (int i = 0; i < NUM_VALUES; i++) {
    if (i) json += ",";
    float v = currentValues[i];
    json += "{\"name\":\"" + String(CSV_HEADER_VALUES[i]) +
            "\",\"unit\":\"" + String(VALUE_UNITS[i]) +
            "\",\"dec\":" + String(VALUE_DECIMALS[i]) +
            ",\"value\":" + ((haveValues && !isnan(v)) ? String(v, VALUE_DECIMALS[i]) : String("null")) + "}";
  }
  json += "]}";

  req->send(200, "application/json", json);
}

// Uhrzeit vom Browser setzen (POST epoch=<Unix-Sekunden>), z.B. im AP-Betrieb ohne Internet
static void handleSetTime(AsyncWebServerRequest* req) {
  if (!req->hasParam("epoch", true)) { req->send(400, "text/plain", "epoch fehlt"); return; }
  uint32_t epoch = strtoul(req->getParam("epoch", true)->value().c_str(), nullptr, 10);
  if (setTimeFromEpoch(epoch)) req->send(200, "text/plain", "OK");
  else                         req->send(400, "text/plain", "ungueltige Zeit");
}

// ---------- Dateien ----------

static void handleDownload(AsyncWebServerRequest* req) {
  String p = req->hasParam("f") ? req->getParam("f")->value() : "";
  if (!isSafeLogPath(p) || !LittleFS.exists(p)) {
    req->send(404, "text/plain", "Nicht gefunden");
    return;
  }
  req->send(LittleFS, p, "text/csv", true);   // true = als Download anbieten
}

static void handleDelete(AsyncWebServerRequest* req) {
  String p = req->hasParam("f") ? req->getParam("f")->value() : "";
  if (isSafeLogPath(p)) LittleFS.remove(p);
  req->redirect("/");
}

// ---------- Setup ----------

void setupWebServer() {
  server.on("/",           HTTP_GET, handleRoot);
  server.on("/api/values", HTTP_GET, handleApiValues);
  server.on("/api/settime", HTTP_POST, handleSetTime);
  server.on("/download",   HTTP_GET, handleDownload);
  server.on("/delete",     HTTP_GET, handleDelete);
  server.onNotFound([](AsyncWebServerRequest* req) { req->send(404, "text/plain", "Nicht gefunden"); });
  server.begin();
}
