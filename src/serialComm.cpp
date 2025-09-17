#include <Arduino.h>
#include <queue>
#include <string>
#include <vector>

#include "serialComm.hpp"
#include "format.hpp"

using namespace std;


//Incomplete


class TCSerial {


    struct Message {
        string messageCode;
        vector<string> payload;
        uint checksum;
        uint id;
    };

    vector<struct Message> out_pending = vector<struct Message>();
    vector<struct Message> in_pending = vector<struct Message>();

    uint32_t message_count;


    TCSerial() {
        message_count = 0;
    }

    void post_message(string mCode, vector<string> payload) {
        struct Message m = Message {
            mCode,
            payload,
            simple_checksum(payload),
            ++message_count
        };
        write_message(m);
        
    }   

    void write_message(struct Message m) {
        Serial.println(format("%s; %lu; %lu; %s", m.messageCode, m.id, m.checksum, join(m.payload, "; ")).c_str());
    }

    unsigned int simple_checksum(const vector<string> s) {
        int sum = 0;
        for (int x = 0; x < s.size(); x++) {
            string c = s[x];
            for (int i = 0; i < c.length(); i++) {
                sum += (unsigned int) c[i];
            }
        }
        return sum;
    }

    void listen(void* param) {
        while (true) {
            if (Serial.available()) {
                String line = Serial.readStringUntil('\n');
                line.trim();
                vector<string> data = split(string(line.c_str()), ";");
                if (data.size() < 2) continue;
                string mcode = data[0];
                data.erase(data.begin());

                if (data[0] == "00A") { //"00A; ogmessagenum" Confirm message

                } else if (data[0] == "00B") { //("00B", ogmessagenum, ogchecksum) Acknowledge message recieved
                    
                } else if (data[0] == "00C") {//("00C", ogmessagenum) Request resend

                } else {
                    uint32_t id = stoul(data[0]);
                    data.erase(data.begin());
                    uint32_t checksum = stoul(data[0]);
                    data.erase(data.begin());
                    struct Message newInboundMessage = Message {
                        mcode,
                        data,
                        checksum,
                        id
                    };
                    handle_inbound_message(newInboundMessage);
                }
            }
            vTaskDelay(1);
        }
    }

    void handle_inbound_message(struct Message m) {
        in_pending.insert(in_pending.begin(), m);
        int s = simple_checksum(m.payload);
        if (s == m.checksum) {
            Serial.println(format("00B; %lu; %lu", m.id, s).c_str()); //Acknowledge message
        }
    }
};

