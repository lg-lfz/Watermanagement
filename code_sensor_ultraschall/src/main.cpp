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
const char* topic1 = "SmartLG/sensoren/agripv/ultraschall";
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

int echoPin = 7;//Kontrolle vom Arduino-Board!
int trigPin = 6;//Kabelkontrolle, Kontrolle der Pins! 
int entfernung;

void setup(){
  
  Serial.begin(9600);
}

void loop(){
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin,LOW);
  entfernung = pulseIn(echoPin,HIGH)/58.0;
  Serial.print("Entfernung in cm: ");
  Serial.println(entfernung);
}
