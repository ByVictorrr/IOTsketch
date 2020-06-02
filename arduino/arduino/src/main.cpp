#include <Arduino.h>
#include <ArduinoJson.h>
#include "IOTbot.h"

#define USB_SERIAL Serial

#define IS_FIRST_UPLOAD false
#define SSID "ATTmhSCTEa"
#define WPA "6505764388"
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"

#define RX 10 
#define TX 43
#define DEBUG_PIN 2

// First Upload declaration
IOTbot iotBot(RX, TX, DEBUG_PIN , SSID, WPA, USERNAME, PASSWORD, IS_FIRST_UPLOAD);

// Not first upload declaration
IOTbot iotbot(RX, TX, DEBUG_PIN);


void setup(){
  USB_SERIAL.begin(9600);
  iotBot.begin(9600);
}

void loop() {
    static String message;
    //========= read commands ==========//
    if((message = iotBot.recieve()) != null_str){
      // A command has been sent from the esp board
      iotBot.send("I have received your message");
    }else{
      // A command has not been sent from the esp board
      iotBot.send("I have not gotten a message");
    }
    delay(1500);
}