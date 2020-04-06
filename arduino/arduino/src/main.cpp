#include <Arduino.h>
#include "HybridSerial.hpp"
#include <ArduinoJson.h>

#define USB_SERIAL Serial
#define COM_SERIAL hs

#define IS_FIRST_UPLOAD true

#define SSID "ATTmhSCTEa"
#define WIFI_PASS "6505764388"
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"
#define NULL String('\0');

#define RX 19
#define TX 3

HybridSerial hs(RX, TX);

void send(const char * message){
  COM_SERIAL.println(message);
}
String recieve(){  
  if(COM_SERIAL.available() > 0){
    return COM_SERIAL.readString();
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
  send(strCreds.c_str());
}

void setup() {
  USB_SERIAL.begin(9600);
  COM_SERIAL.begin(4800);
  
 }


void loop() {
 String message;
 if(IS_FIRST_UPLOAD){
    sendCreds();
  }else{
    //=== read commands - format {command: <command>}====//
    // Case 1 - nothing is received
    if((message = recieve()) == NULL){
      return;
    // Case 2 - not of json format
    }else if(deseralizeJson(json_msg, message)){
      return;
    // Case 3 - json format
    }
  }

  USB_SERIAL.println(COM_SERIAL.readString());
  delay(100);
}