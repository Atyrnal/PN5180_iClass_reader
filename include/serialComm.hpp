
#include <string>

using namespace std;

class TCSerial : public TCSerial {
    public:
        struct OutMessage;
        TCSerial();
        void post_message(OutMessage m);
    private:
        void write_message(OutMessage m);
        unsigned int simple_checksum(string s);
        void listen();
};