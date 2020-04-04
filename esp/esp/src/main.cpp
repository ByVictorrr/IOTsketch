#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "SocketIoClient.h"
// #include "CPutil.h"
#include <ArduinoJson.h>


#define USE_SERIAL Serial
#define SSID "ATTmhSCTEa"
#define PASS "6505764388"
#define SERVER_IP "192.168.1.252"
#define SERVER_PORT 80
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"


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
  USE_SERIAL.printf("message %s\n", payload);
}
void setup_WiFi(ESP8266WiFiMulti *wifi){
  // Step 1 - give creds
  if(!wifi->addAP(SSID, PASS))
    exit(-1);
  // Step 2 - have wifi connected
  while(wifi->run() != WL_CONNECTED)
        delay(100);
  USE_SERIAL.println("Wifi connected to SSID");
}

void setup(){
  USE_SERIAL.begin(9600);
  USE_SERIAL.setDebugOutput(true);
   for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }
  // step 1 - set up wifi
  setup_WiFi(&WiFiMulti);
  // step 2 - connect to server and set handlers
  webSocket.begin(SERVER_IP, SERVER_PORT, "/socket.io/?transport=websocket");
  webSocket.on("join room", join_room);
  webSocket.on("message", message);
}

    

void loop(){
  static int loop_count=0;
  // static MSTimer loopTimer;

  //USE_SERIAL.println(loop_count++); 
  webSocket.loop();
}
