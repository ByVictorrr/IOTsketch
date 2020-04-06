#include <Arduino.h>
#include <SoftwareSerial.h>
#define NOT_USED -1
/* For the mega because the software serial recieve doesnt work - so we need to use hardware serial */
class HybridSerial{

     private:
    Stream *ss; 
    Stream *hs;
   
    // maps pin of the arduino mega2560 to the hardware serial classs instance
    HardwareSerial *getHardwareSerial(int pin){
        if(pin == 0 || pin == 1){
            return &Serial;
        }else if (pin == 18 || pin == 19){
            return &Serial1;
        }else if (pin == 16 || pin == 17){
            return &Serial2;
        }else if (pin == 14 || pin == 15){
            return &Serial3;
        }
            return &Serial;
    }
  public:
   HybridSerial(int rx, int tx){
       hs = getHardwareSerial(rx);
       ss = new SoftwareSerial(rx, tx);
    }
    ~HybridSerial(){
        if (ss != nullptr){
            free(ss);
        }
    }

    void begin(const unsigned long baud){
        ((HardwareSerial*)hs)->begin(baud);
        ((SoftwareSerial*)ss)->begin(baud);
    }
    /* wrapper for hs.availble() */
   int available(){
       return ((HardwareSerial * )hs)->available();
   } 
   void println(const Printable & message){
       ((SoftwareSerial*)ss)->println(message);
   }
   void print(const Printable & message){
       ((SoftwareSerial*)ss)->print(message);
   }
    void print(const char * message){
       ((SoftwareSerial*)ss)->print(message);
   } 
    void println(const char * message){
       ((SoftwareSerial*)ss)->println(message);
   }
   
   String readString(){
       return ((HardwareSerial *)hs)->readString();
   }
};