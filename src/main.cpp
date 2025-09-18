
//ESP32 Stuff
#include <Arduino.h>
#include "PN5180iClass.h"

#include <string>

#include <ctime>
#include "format.hpp"


//Specify pin numbers
#define ONBOARD_LED 2
#define IO_LED 15

using namespace std;

// Pins
const uint8_t PN5180_NSS  = 5;   //  NSS
const uint8_t PN5180_BUSY = 17;  // BUSY
const uint8_t PN5180_RST  = 16;  // RESET

PN5180iClass reader(PN5180_NSS, PN5180_BUSY, PN5180_RST);


uint64_t last_scan = 0x0;
time_t last_scan_time;
uint64_t last_detected = 0x0;

bool genCardPseudoID(uint64_t *pseudoID) {
    iClassErrorCode rc;

    uint8_t raw[8]; //raw response
    rc = reader.Identify(raw);

    if (rc != ICLASS_EC_OK) return false;

    uint64_t hash = raw[0];
    for (int i = 1; i < 8; i++) {
      hash = hash << 8;
      hash += raw[i];
    }
    *pseudoID = hash;
    Serial.println(format("20D; %02x%02x%02x%02x%02x%02x%02x%02x", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7]).c_str());
    return true;
} //0609600230

void on_scan(uint64_t id) {
  Serial.println(format("20B; %020llu; ", id).c_str());
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
  Serial.println("10A; Serial opened");
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(IO_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);
  digitalWrite(IO_LED, HIGH);

  //RFID Reader
  reader.begin();
  delay(50);
  if (!reader.setupRF()) {
    Serial.println("10C; RFID Failed");
    exit(1);
  }
  last_scan_time = (time_t) -5;
  for (int i = 0; i < 3; i++) {
    Serial.println("10B; RFID Ready");
  }
  digitalWrite(IO_LED, LOW);
}



void loop() {
  iClassErrorCode rc = reader.ActivateAll();

  if(rc == ICLASS_EC_OK) {
    digitalWrite(ONBOARD_LED, HIGH);
    digitalWrite(IO_LED, HIGH);
    uint64_t psID;
    if (genCardPseudoID(&psID)) {
      Serial.println(format("20A; iClass card found; %016llx", psID).c_str());
      bool match = (psID == last_detected);
      last_detected = psID;
      time_t now;
      time(&now);
      if (match && difftime(now, last_scan_time) > 5.0 && (last_scan != psID || difftime(now, last_scan_time) > 15.0)) {
        last_scan = psID;
        time(&last_scan_time);
        on_scan(psID); 
      }
    
    } else {
      Serial.println("20C; iClass card found; Failed to generate psuedo identifier hash");
    }

    
    
  } else {
    digitalWrite(ONBOARD_LED, LOW);
    digitalWrite(IO_LED, LOW);
    //Serial.println("No card detected.");
  }

  delay(50);
}


