
//ESP32 Stuff
#include <Arduino.h>
#include "PN5180iClass.h"

#include <string>
#include <memory>
#include <stdexcept>

//Specify pin numbers
#define LED_PIN 2

using namespace std;

// Pins
const uint8_t PN5180_NSS  = 5;   //  NSS
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

template<typename ... Args>
string format(const string& format, Args ... args) {
  int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
  if( size_s <= 0 ) { throw runtime_error( "Error during formatting." ); }
  auto size = static_cast<size_t>( size_s );
  unique_ptr<char[]> buf( new char[ size ] );
  snprintf( buf.get(), size, format.c_str(), args ... );
  return string( buf.get(), buf.get() + size - 1 );
}

void setup() { 
  //Open serial connection
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  reader.begin();
  delay(50);
  if (!reader.setupRF()) {
    //Failed to initialize RF
    exit(1);
  }
   digitalWrite(LED_PIN, LOW);
}

void loop() {
  iClassErrorCode rc = reader.ActivateAll();

  if(rc == ICLASS_EC_OK) {
    digitalWrite(LED_PIN, HIGH);
    uint32_t psID;
    if (genCardPseudoID(&psID)) {
      Serial.println(format("200; iClass card found; %08x", psID).c_str());
    } else {
      Serial.println("500; iClass card found; Failed to generate psuedo identifier hash");
    }
    
    
  } else {
    digitalWrite(LED_PIN, LOW);
    //Serial.println("No card detected.");
  }

  delay(150);
}


