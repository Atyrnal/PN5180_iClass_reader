
//ESP32 Stuff
#include <Arduino.h>
#include <WiFi.h>
#include "PN5180iClass.h"


//Code stuff
#include <string>
#include "secrets.hpp"
#include <bitset>

//Specify pin numbers
#define LED_PIN 2

#ifndef WIFI_SSID
#define WIFI_SSID "wifi_ssid"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "wifi_password"
#endif

#ifndef WIFI_SECURITY
#define WIFI_SECURITY "WPA2"
#endif

using namespace std;

// Pins
const uint8_t PN5180_NSS  = 5;   // CS / NSS
const uint8_t PN5180_BUSY = 17;  // BUSY
const uint8_t PN5180_RST  = 16;  // RESET

PN5180iClass reader(PN5180_NSS, PN5180_BUSY, PN5180_RST);

void setup() { 
  //Open serial connection
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //WiFi connections
  char* ssid = WIFI_SSID;
  char* password = WIFI_PASSWORD;

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected\nIP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, LOW);

  reader.begin();
  delay(50);

  if (!reader.setupRF()) {
    while (1) { delay(500); }
  }
  Serial.println("Reader Ready");
}

void loop() {
  iClassErrorCode rc = reader.ActivateAll();

  if(rc == ICLASS_EC_OK) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("iClass card detected in the field!");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("No card detected.");
  }

  delay(500);
}



