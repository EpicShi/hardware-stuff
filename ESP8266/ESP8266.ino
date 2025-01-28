#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

#define DPIN 2
#define SOIL_PIN A0
#define DTYPE DHT11


DHT dht(DPIN, DTYPE);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void connectToWifi() {
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

String sendPacket(String dataPacket) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(dataPacket);
    http.end();

    return "HTTP Response code: " + httpResponseCode;
  } else {
    return "WiFi Disconnected";
  }
}

time_t getCurrentTime() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  return epochTime;
}

void setup() {
  Serial.begin(115200);

  connectToWifi();

  dht.begin();

  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void loop() {
  time_t time = getCurrentTime();
  float tc = dht.readTemperature(false);
  float hu = dht.readHumidity();
  int mt = map(analogRead(SOIL_PIN), 0, 1023, 0, 100);

  String packet = "{\"time\":\"" + String(time) + "\",\"humidity\":\"" + String(hu) + "\",\"temperature\":\"" + String(tc) + "\",\"soil_moisture\":\"" + String(mt) + "\",\"uuid\":\"" + UUID + "\"}";
  String response = sendPacket(packet);

  Serial.print(packet);
  Serial.println("");

  delay(500);
}