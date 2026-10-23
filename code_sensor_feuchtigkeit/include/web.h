#pragma once

// Webserver (ESPAsyncWebServer) auf Port 80:
//   /                 -> aktuelle Messwerte (live) + Dateiliste
//   /api/values       -> aktuelle Messwerte als JSON
//   /api/settime      -> POST epoch=... : Uhrzeit vom Browser übernehmen
//   /download?f=...   -> CSV herunterladen
//   /delete?f=...     -> Datei löschen
//
// Achtung: Handler laufen im Async-Kontext -> kein delay(), nicht messen.

#include "main.h"

void setupWebServer();
