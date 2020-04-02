#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SocketIoClient.h>
#include "CPutil.h"


#define USE_SERIAL Serial
#define SSID "ATTmhSCTEa"
#define PASS "6505764388"
#define SERVER_IP "192.168.1.252"
#define SERVER_PORT 80
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  USE_SERIAL.println((char*)payload);

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			bool b;
      if((b = webSocket.sendTXT("ConnectText")) == true){
			  USE_SERIAL.printf("[WSc] sent!");
      }else{
			  USE_SERIAL.printf("[WSc] not sent!");
      }
    
		}
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

      if(strcmp("42[\"join room\",\"hi\"]", (char*)payload)==0){
        USE_SERIAL.println("about to send text");
        webSocket.sendTXT("[\"connect bot\",{\"username\': \'byvictorr\', \'password\': \'calpoly\'}]");
      }
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
        case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
    }

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
  // step 2 - connect to server
  webSocket.begin(SERVER_IP, SERVER_PORT, "/socket.io/?transport=websocket");
  webSocket.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	//webSocket.setReconnectInterval(SERVER_PORT);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.setExtraHeaders("user_id: 4\r\n");
  //webSocket.enableHeartbeat(15000, 3000, 2);

  // step 3 - connect to server 
}

    

void loop(){
  static int loop_count=0;
  static MSTimer loopTimer;

  //USE_SERIAL.println(loop_count++); 
  webSocket.loop();
}
