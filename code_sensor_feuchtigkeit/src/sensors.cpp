#include "sensors.h"

float    currentValues[NUM_VALUES];
uint32_t currentValuesMillis = 0;
bool     haveValues = false;

static uint32_t s_lastSample = 0;

// Rohwert linear zwischen den Kalibrierpunkten auf 0-100 % abbilden
static float rawToMoisturePercent(float raw) {
  float pct = (raw - MOIST_RAW_DRY) * 100.0f / (MOIST_RAW_WET - MOIST_RAW_DRY);
  return constrain(pct, 0.0f, 100.0f);
}

static float readMoistureRaw() {
  if (MOIST_POWER_PIN >= 0) {
    digitalWrite(MOIST_POWER_PIN, HIGH);
    delay(MOIST_SETTLE_MS);
  }

  uint32_t sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) {
    sum += analogRead(A0);
    delay(2);
  }

  if (MOIST_POWER_PIN >= 0) digitalWrite(MOIST_POWER_PIN, LOW);
  return (float)sum / ADC_SAMPLES;   // 0 ... 1023
}

void setupSensors() {
  if (MOIST_POWER_PIN >= 0) {
    pinMode(MOIST_POWER_PIN, OUTPUT);
    digitalWrite(MOIST_POWER_PIN, LOW);
  }
}

void readSensors(float* values) {
  float raw = readMoistureRaw();
  values[0] = rawToMoisturePercent(raw);
  values[1] = raw;
}

void updateCurrentValues() {
  float v[NUM_VALUES];
  readSensors(v);

  noInterrupts();
  for (int i = 0; i < NUM_VALUES; i++) currentValues[i] = v[i];
  currentValuesMillis = millis();
  haveValues = true;
  interrupts();
}

void handleSensors() {
  if (!haveValues || millis() - s_lastSample >= SAMPLE_INTERVAL_MS) {
    s_lastSample = millis();
    updateCurrentValues();
  }
}
