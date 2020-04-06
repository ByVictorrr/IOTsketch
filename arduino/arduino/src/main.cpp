#include <Arduino.h>
#include "HybridSerial.hpp"
#include <ArduinoJson.h>

#define USB_SERIAL Serial
#define COM_SERIAL hs

#define FIRST_UPLOAD_INDICATOR "!!"
#define IS_FIRST_UPLOAD true

#define SSID "ATTmhSCTEa"
#define WIFI_PASS "6505764388"
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"

#define RX 19
#define TX 3

HybridSerial hs(RX, TX);

void send(const char * message){
  COM_SERIAL.println(message);
}
const char *recieve(){  
  if(COM_SERIAL.available() > 0){
    return COM_SERIAL.readString().c_str();
  }
  return NULL;
}
void sendCreds(){
  DynamicJsonDocument creds(400);
  String strCreds;
  creds["ssid"] = SSID;
  creds["wifi_pass"] = WIFI_PASS;
  creds["username"] = USERNAME;
  creds["password"] = PASSWORD;
  serializeJson(creds, strCreds);
  send(strCreds);

}

void setup() {
  USB_SERIAL.begin(9600);
  COM_SERIAL.begin(4800);
  
 }


void loop() {
 if(IS_FIRST_UPLOAD){
    send(FIRST_UPLOAD_INDICATOR);
    delay(1000);
    sendCreds();
  }else{
    // validate commands 
  }

  USB_SERIAL.println(COM_SERIAL.readString());
}