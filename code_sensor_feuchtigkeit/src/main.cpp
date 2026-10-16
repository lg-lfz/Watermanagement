#include "main.h"

// ---------- WLAN ----------
const char* ssid = "LG-LFZ-IOT";
const char* password = "ztoP847QLnBcx081!o?";

// ---------- MQTT ----------
const char* mqtt_server = "172.29.30.20";
const int mqtt_port = 1883;
const char* mqtt_user = "broker";
const char* mqtt_password = "smartLG26!";

// ---------- Topics ---------- Hier entsprechend benennen
const char* topic1 = "SmartLG/sensoren/agripv/bodenfeuchte";
/*const char* topic2  = "SmartLG/sensoren/agripv/wert2";
const char* topic3 = "SmartLG/sensoren/agripv/wert3";
const char* topic4 = "SmartLG/sensoren/agripv/wert4";*/

// ---------- Sensor ----------
Adafruit_BME280 bme;

// ---------- MQTT Client ----------
WiFiClient espClient;
PubSubClient client(espClient);

// ---------- Buffer (wichtig für ESP8266!) ----------
char buffer[16];

// ---------- WLAN ----------
void setup_wifi() {
  Serial.println();
  Serial.print("Verbinde WLAN: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WLAN verbunden!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ---------- MQTT ----------
void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Verbinde MQTT...");

    String clientId = "ESP8266_BME280_";
    clientId += String(ESP.getChipId(), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println(" verbunden!");
    } else {
      Serial.print(" Fehler rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin(D2, D1);  // ESP8266 I2C Pins

  /*if (!bme.begin(0x76) && !bme.begin(0x77)) {
    Serial.println("BME280 nicht gefunden!");
    while (true) delay(10);
  }

  Serial.println("BME280 OK");*/

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  if (!client.connected()) {
    reconnect_mqtt();
  }

  client.loop();

  static unsigned long lastMsg = 0;

  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    float messwert = analogRead(A0);
    float bodenfeuchte = (0.4651 * messwert) - 106.98;

    /*float wert1 = 2;
    float wert1 = 3;
    float wert1 = 4;*/

    // ---------- Wert1 Senden ----------
    dtostrf(bodenfeuchte, 1, 2, buffer);
    client.publish(topic1, buffer, true);

    /* ----------Wert2 Senden----------
    dtostrf(wert2, 1, 2, buffer);
    client.publish(topic2, buffer, true);

    // ---------- Wert3 Senden ----------
    dtostrf(wert3, 1, 2, buffer);
    client.publish(topic3, buffer, true);

    // ---------- Wert4 Senden ----------
    dtostrf(wert4, 1, 2, buffer);
    client.publish(topic4, buffer, true);
    */

      Serial.print("Bodenfeuchte: ");
    Serial.print(bodenfeuchte);
    //Serial.print(" wert2 "); Serial.print(wert2);
    //Serial.print(" wert3: "); Serial.print(wert3);
    //Serial.print(" wert4: "); Serial.println(wert4);
  }
}