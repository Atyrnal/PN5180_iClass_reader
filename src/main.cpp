
//ESP32 Stuff
#include <Arduino.h>
#include <WiFi.h>
#include "PN5180.h"


//Code stuff
#include <string>
#include "secrets.hpp"

//Specify pin numbers
PN5180 pn5180(/* NSS=*/ 5, /* BUSY=*/ 17, /* RST=*/ 16); // ESP32
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

  //Initalize RFID reader
  Serial.println("Initializing PN5180 RFID reader");  
  pn5180.begin();
  delay(100);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  // Create an 8-byte integer array to store UID of any detected tag
  uint8_t uid[8];
  
  // Call the getInventory() method
  if (pn5180.getInventory(uid)) {
	  
	// If tag was found, print its ID to the serial monitor
    Serial.print("Tag UID: ");
	// Format each byte as HEX, padded with leading zeroes if required
    for (int i = 7; i >= 0; i--) {
      if (uid[i] < 0x10) Serial.print("0");
      Serial.print(uid[i], HEX);
    }
    Serial.println();
  } 
  // No tag in range
  else {
    Serial.println("No tag detected.");
  }
  delay(500);
}


