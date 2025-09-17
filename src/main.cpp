
//ESP32 Stuff
#include <Arduino.h>
#include <WiFi.h>
#include "PN5180iClass.h"


//Code stuff
#include <string>
#include "secrets.hpp"
#include <ctime>
#include "format.hpp"

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


uint32_t last_scan = 0x0;
time_t last_scan_time;
uint32_t last3[3] = { 0x0, 0x0, 0x0 };

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

void on_scan(uint32_t id) {
  Serial.println(format("20B; %010lu; ", id).c_str());
}

void setup() { 
  //Open serial connection
  Serial.begin(115200);
  Serial.println("10A; Serial opened");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //WiFi connections
  char* ssid = WIFI_SSID;
  char* password = WIFI_PASSWORD;

  Serial.println(format("30A; connecting to %s", ssid).c_str());
  WiFi.begin(ssid, password);
  double timeout = 10.0;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    timeout -=.5;
  }
  if (timeout > 0) {
    Serial.println(format("30B; connected; %s", WiFi.localIP().toString()).c_str());
  } else {
    Serial.println("30C; no connection");
    //exit(1);
  }
  digitalWrite(LED_PIN, LOW);

  //RFID Reader
  reader.begin();
  delay(50);

  if (!reader.setupRF()) {
    Serial.println("10C; RFID Failed");
    exit(1);
  }
  last_scan_time = (time_t) 0;
  for (int i = 0; i < 3; i++) {
    Serial.println("10B; RFID Ready");
  }
}



void loop() {
  iClassErrorCode rc = reader.ActivateAll();

  if(rc == ICLASS_EC_OK) {
    digitalWrite(LED_PIN, HIGH);
    uint32_t psID;
    if (genCardPseudoID(&psID)) {
      Serial.println(format("20A; iClass card found; %08x", psID).c_str());
      bool threematch = true;
      for (int i = 2; i > 0; i--) {
        last3[i] = last3[i-1];
        if (last3[i] != psID) threematch = false;
      }
      last3[0] = psID;
      time_t now;
      time(&now);
      if (threematch && difftime(now, last_scan_time) > 3.0 && (last_scan != psID || difftime(now, last_scan_time) > 15.0)) {
        last_scan = psID;
        time(&last_scan_time);
        on_scan(psID); 
      }
    } else {
      Serial.println("20C; iClass card found; Failed to generate psuedo identifier hash");
    }

    
    
  } else {
    digitalWrite(LED_PIN, LOW);
    //Serial.println("No card detected.");
  }

  delay(50);
}



