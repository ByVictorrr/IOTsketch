#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "SocketIoClient.h"
#include <SoftwareSerial.h>
// #include "CPutil.h"
#include <ArduinoJson.h>


#define FIRST_UPLOAD_INDICATOR "!!" // has to be synced with other sketch
#define USB_SERIAL Serial
#define COM_SERIAL ss
#define SSID "ATTmhSCTEa"
#define PASS "6505764388"
#define SERVER_IP "192.168.1.252"
#define SERVER_PORT 80
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"



SoftwareSerial ss(D2, D3);
ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;


void join_room(const char * payload, size_t length){
  DynamicJsonDocument message(200);
  String messageStr;
  message["username"] = USERNAME;
  message["password"] = PASSWORD;
  serializeJson(message, messageStr);
  webSocket.emit("connect bot", messageStr.c_str()); 
}
void message(const char * payload, size_t length){
  USB_SERIAL.printf("message %s\n", payload);
}
void setup_WiFi(ESP8266WiFiMulti *wifi){
  // Step 1 - give creds
  if(!wifi->addAP(SSID, PASS))
    exit(-1);
  // Step 2 - have wifi connected
  while(wifi->run() != WL_CONNECTED)
        delay(100);
  USB_SERIAL.println("Wifi connected to SSID");
}

void setup(){
  USB_SERIAL.begin(9600);
  USB_SERIAL.setDebugOutput(true);
  COM_SERIAL.begin(4800);
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  USB_SERIAL.setDebugOutput(false);
  for(uint8_t t = 4; t > 0; t--) {
          USB_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USB_SERIAL.flush();
          delay(1000);
      }
  // step 1 - set up wifi
  webSocket.begin(SERVER_IP, SERVER_PORT, "/socket.io/?transport=websocket");
  webSocket.on("join room", join_room);
  webSocket.on("message", message);
}

// Reads from the mega board
char *recieve(){
  if(COM_SERIAL.available() > 0){
    return (char *)(COM_SERIAL.readString().c_str());
  }
  return NULL;
}
    
// Writes to the mega board
void write(String message){
  COM_SERIAL.println(message.c_str());
}

void loop(){

  char *message;
  if((message=recieve()) != NULL){
    // means we got somethings

    /* case 1 - first time uploading */
    // use eeprom to store values

  }

  webSocket.loop();
}
