
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

bool genCardPseudoID(uint32_t *pseudoID) {
    iClassErrorCode rc;

    uint8_t raw[8]; //raw response
    rc = reader.Identify(raw);
    if (rc != ICLASS_EC_OK) return false;

    uint32_t hash = 0;
    for (int i = 0; i < 8; i++) {
        hash = hash * 31 + raw[i];
    }
    *pseudoID = hash;
    return true;
}

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
  double timeout = 10.0;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    timeout -=.5;
    Serial.print(".");
  }
  if (timeout > 0) {
    Serial.print("Connected\nIP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Unable to connect to WiFi: connection timed out.");
  }
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
    uint32_t psID;
    if (genCardPseudoID(&psID)) {
      Serial.print("200; iClass card found; ");
      Serial.println(psID);
    } else {
      Serial.println("500; iClass card found; Failed to generate psuedo identifier hash");
    }
    
    
  } else {
    digitalWrite(LED_PIN, LOW);
    //Serial.println("No card detected.");
  }

  delay(500);
}



