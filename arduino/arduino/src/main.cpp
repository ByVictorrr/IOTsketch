#include <Arduino.h>
#include <ArduinoJson.h>
#include "IOTbot.hpp"

#define USB_SERIAL Serial

#define IS_FIRST_UPLOAD false
#define SSID "ATTmhSCTEa"
#define WPA "6505764388"
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"
#define RX 10
#define TX 45

IOTbot iotBot(RX, TX, IS_FIRST_UPLOAD, SSID, WPA, USERNAME, PASSWORD);
void setup(){
  USB_SERIAL.begin(9600);
  iotBot.begin(115200);
}

void loop() {

    static String message;
    DynamicJsonDocument json_msg(400);
    //========= read commands ==========//
    // Case 1 - nothing is received
    if((message = iotBot.recieve()) != null_str){
      // Case 2 - not of json format
      if(!deserializeJson(json_msg, message)){
        USB_SERIAL.println("message not of json format");
      // Case 3 - command in json format, but doesnt have the right key
      }else if(!json_msg.containsKey("command")){
        USB_SERIAL.println("message of not of the form {\"command\": <command>}");
      }else{
        USB_SERIAL.println("message of the write form");
      }
        USB_SERIAL.println(message);
    }
    //======== write commands ============//
    json_msg["msg"] = "arduino";
    serializeJson(json_msg, message);
    iotBot.send(message);
    //==================================//
}