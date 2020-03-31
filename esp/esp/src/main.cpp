#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SocketIoClient.h>
#include "CPutil.h"


#define USE_SERIAL Serial
#define SSID "ATTmhSCTEa"
#define PASS "6505764388"
#define SERVER_IP "192.168.1.93"
#define SERVER_PORT 5000
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"


ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;
//===================Handlers=========================//
void event(const char * payload, size_t length) {
  USE_SERIAL.printf("got message: %s\n", payload);
}

void connect(const char * payload, size_t length) 
{
  USE_SERIAL.printf("in connect!!!!\n");
}

void fromServer(const char * payload, size_t length)
{
  USE_SERIAL.printf("in fromServer: %s\n", payload);
}
void join_room(const char * payload, size_t length){
  const char * msg = "{\'username\': \'byvictorrr\', \'password\': \'calpoly\'}";
  webSocket.emit("connect bot", msg);
}


void disconnected(const char * payload, size_t length)
{
  USE_SERIAL.printf("in disconnected: %s\n", payload);
}

//===================Handlers=========================//
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
  // step 2 - setup handler for socketio events
  webSocket.on("connect", connect);
  //webSocket.on("connected", event);
  webSocket.on("join room", join_room); 
  //webSocket.on("fromServer", fromServer);
  //webSocket.on("disconnected", disconnected); 

  // step 3 - connect to server 
  webSocket.begin(SERVER_IP, SERVER_PORT);
}

    

void loop(){
  static int loop_count=0;
  static MSTimer loopTimer;

  if(loop_count < 500){
    USE_SERIAL.println(loop_count++); 
  }
  webSocket.loop();
  if (loopTimer.done()) 
  {
    loopTimer.set(2000);
    webSocket.emit("connect bot", "{\"username\": \"byvictorrr\", \"password\": \"calpoly\"}");
  }
}