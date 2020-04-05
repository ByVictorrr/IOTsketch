#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "SocketIoClient.h"
#include <SoftwareSerial.h>
#include <EEPROM.h>
// #include "CPutil.h"
#include <ArduinoJson.h>


#define FIRST_UPLOAD_INDICATOR "{\"ssid" // has to be synced with other sketch
#define USB_SERIAL Serial
#define COM_SERIAL ss
#define MAX 35 // at 50 it gives a stack trace
#define SERVER_IP "192.168.1.252"
#define SERVER_PORT 80

#define NULL String('\0')

#define GET_SSID "ssid"
#define GET_WIFI_PASS "wifi_pass"
#define GET_USERNAME "username"
#define GET_PASSWORD "password"
 

DynamicJsonDocument creds(400);
SoftwareSerial ss(D2, D3);
ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;


//=====================SOCKETIO=======================//
void join_room(const char * payload, size_t length){
  DynamicJsonDocument message(200);
  String messageStr;
  message["username"] = creds[GET_USERNAME];
  message["password"] = creds[GET_PASSWORD];
  serializeJson(message, messageStr);
  webSocket.emit("connect bot", messageStr.c_str()); 
}
void message(const char * payload, size_t length){
  USB_SERIAL.printf("message %s\n", payload);
}
void setup_WiFi(ESP8266WiFiMulti *wifi){
  // Step 1 - give creds
  if(!wifi->addAP(creds[GET_SSID], creds[GET_WIFI_PASS])){
    USB_SERIAL.println("wrong ssid and password");
    String message = creds[GET_PASSWORD];
    USB_SERIAL.println(message);
    String message1=creds[GET_SSID];
    USB_SERIAL.println(message1);
    return;
  }
  // Step 2 - have wifi connected
  while(wifi->run() != WL_CONNECTED)
        delay(100);
  USB_SERIAL.println("Wifi connected to SSID");
}

//=====================COMM with arduino=======================//
String recieve(){
  if(COM_SERIAL.available() > 0){
    return COM_SERIAL.readString();
  }
  return NULL;
}
    
// Writes to the mega board
void write(String message){
  COM_SERIAL.println(message);
}

String read_EEPROM(int size, int base_addr){
  String data;
  for(int i = base_addr; i<base_addr+size; i++){
    USB_SERIAL.println("reading");
    data+=char(EEPROM.read(i));
  }
  return data;
}
void write_EEPROM(String data, int size, int base_addr){
  const char *c_data = data.c_str();
  for(int i = base_addr; i<base_addr+size; i++){
    EEPROM.write(i, c_data[i]);
  }
}

//=====================================================//

void setup(){
  USB_SERIAL.begin(9600);
  COM_SERIAL.begin(4800);
  EEPROM.begin(512);

  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  USB_SERIAL.setDebugOutput(false);
  for(uint8_t t = 4; t > 0; t--) {
          USB_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USB_SERIAL.flush();
          delay(1000);
      }
  // step 1 - load variables stored in EEPROM
  
  creds[GET_SSID]=read_EEPROM(MAX, 0);
  creds[GET_WIFI_PASS]=read_EEPROM(MAX, MAX);
  creds[GET_USERNAME]=read_EEPROM(MAX, 2*MAX);
  creds[GET_PASSWORD]=read_EEPROM(MAX, 3*MAX);
  setup_WiFi(&WiFiMulti);


  // Step 2 - socketio stuff
  webSocket.begin(SERVER_IP, SERVER_PORT, "/socket.io/?transport=websocket");
  webSocket.on("join room", join_room);
  webSocket.on("message", message);
}


void loop(){

  String message;
  message.reserve(MAX);
  USB_SERIAL.print("begginging");
  USB_SERIAL.println(message=recieve());
  USB_SERIAL.print("end");
  if(message.substring(0,strlen(FIRST_UPLOAD_INDICATOR)) == FIRST_UPLOAD_INDICATOR){
    DynamicJsonDocument write_creds(400);
    // case 1 - first time uploading
    USB_SERIAL.println("here");
    USB_SERIAL.println("passed msg recieve");
    deserializeJson(write_creds, message.c_str());
    // first clear the eeprom
    for (int i = 0; i < EEPROM.length(); i++){
      EEPROM.write(i, 0);
    }
    // TODO: parse messages in DYnamic json objects
    write_EEPROM(write_creds[GET_SSID], strlen(write_creds[GET_SSID]), 0);
    write_EEPROM(write_creds[GET_WIFI_PASS], strlen(write_creds[GET_WIFI_PASS]), MAX);
    write_EEPROM(write_creds[GET_USERNAME], strlen(write_creds[GET_USERNAME]), 2*MAX);
    write_EEPROM(write_creds[GET_PASSWORD], strlen(write_creds[GET_PASSWORD]), 3*MAX);
    EEPROM.commit();
    // case 2 - else treat everything normally 
    }else{
      // USB_SERIAL.println("else statement");
    }
  webSocket.loop();
}