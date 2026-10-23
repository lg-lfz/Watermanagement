#pragma once

// Messwerte erfassen

#include "main.h"

// Zuletzt gemessene Werte. Nur aus loop() heraus aktualisieren
// (updateCurrentValues); Webserver-Handler lesen nur.
extern float    currentValues[NUM_VALUES];
extern uint32_t currentValuesMillis;
extern bool     haveValues;

void setupSensors();

// Liest alle Sensoren in values[0..NUM_VALUES-1]. Blockiert kurz (delay) ->
// nicht aus Async-Webserver-Handlern aufrufen!
void readSensors(float* values);

// Misst neu und übernimmt die Werte in currentValues
void updateCurrentValues();

// In loop() aufrufen; misst alle SAMPLE_INTERVAL_MS
void handleSensors();
