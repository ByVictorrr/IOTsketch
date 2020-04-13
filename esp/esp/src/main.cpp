#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "SocketIoClient.h"
#include <SoftwareSerial.h>
#include <EEPROM.h>
// #include "CPutil.h"
#include <ArduinoJson.h>


#define FIRST_UPLOAD_INDICATOR "{\"ssid" 
#define USB_SERIAL Serial
#define COM_SERIAL ss
#define MAX 20 // at 50 it gives a stack trace
#define SERVER_IP "192.168.1.252"
#define SERVER_PORT 80

#define null_str String('\0')

#define GET_SSID "ssid"
#define GET_WIFI_PASS "wifi_pass"
#define GET_USERNAME "username"
#define GET_PASSWORD "password"

#define TX D2
#define RX D1
 


DynamicJsonDocument creds(400);
SoftwareSerial ss(RX, TX);
ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;


//=====================SOCKETIO=======================//
void join_room(const char * payload, size_t length){
  DynamicJsonDocument message(200);
  String messageStr;
  // String username = message["username"] = creds[GET_USERNAME];
  // String pass = message["password"] = creds[GET_PASSWORD];
  String username = message["username"] = "byvictorrr";
  String pass = message["password"] = "calpoly";
  USB_SERIAL.println("joining room with");
  serializeJson(message, messageStr);
  webSocket.emit("connect bot", messageStr.c_str()); 
}
// recieved a msg
void message(const char * payload, size_t length){
  USB_SERIAL.printf("message %s\n", payload);
  // send to arduino
  COM_SERIAL.println(payload);
}
void setup_WiFi(ESP8266WiFiMulti *wifi){
  // Step 1 - give creds
    String message = creds[GET_WIFI_PASS];
    USB_SERIAL.println(message);
    String message1=creds[GET_SSID];
    USB_SERIAL.println(message1);
    
  if(!wifi->addAP(creds[GET_SSID], creds[GET_WIFI_PASS])){
    USB_SERIAL.println("wrong ssid and password");
    String message = creds[GET_WIFI_PASS];
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
  String data;
  data.reserve(100);
  if(COM_SERIAL.available() > 0){
    data = COM_SERIAL.readStringUntil('\n');
    return data;
  }
  return null_str;
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
  COM_SERIAL.begin(9600);
  EEPROM.begin(512);

  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);
  digitalWrite(TX, LOW);
  USB_SERIAL.setDebugOutput(false);
  for(uint8_t t = 4; t > 0; t--) {
          USB_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USB_SERIAL.flush();
          delay(1000);
      }

  // step 1 - load variables stored in EEPROM
  //creds[GET_SSID]=read_EEPROM(MAX, 0);
  creds[GET_SSID]= "ATTmhSCTEa";
  creds[GET_WIFI_PASS]="6505764388";
  //creds[GET_WIFI_PASS]=read_EEPROM(MAX, MAX);
  String username = creds[GET_USERNAME]=read_EEPROM(MAX, 2*MAX);
  String pass = creds[GET_PASSWORD]=read_EEPROM(MAX, 3*MAX);
  USB_SERIAL.println(username);
  USB_SERIAL.println(pass);
  USB_SERIAL.println(EEPROM.length());
  setup_WiFi(&WiFiMulti);


  // Step 2 - socketio stuff
  webSocket.begin(SERVER_IP, SERVER_PORT, "/socket.io/?transport=websocket");
  webSocket.on("join room", join_room);
  webSocket.on("message", message);
}

void first_upload(DynamicJsonDocument &json_msg){
    for (int i = 0; i < EEPROM.length(); i++){
      EEPROM.write(i, 0);
    }
    write_EEPROM(json_msg[GET_SSID], strlen(json_msg[GET_SSID]), 0);
    write_EEPROM(json_msg[GET_WIFI_PASS], strlen(json_msg[GET_WIFI_PASS]), MAX);
    write_EEPROM(json_msg[GET_USERNAME], strlen(json_msg[GET_USERNAME]), 2*MAX);
    write_EEPROM(json_msg[GET_PASSWORD], strlen(json_msg[GET_PASSWORD]), 3*MAX);
    EEPROM.commit(); 
}

void loop(){

  String message;
  DynamicJsonDocument json_msg(400);
  message.reserve(MAX);

  // Case 1 - data has been received
  if((message=recieve()) != null_str){
    // Case 2 - regular message from arduino(general message)
    if(deserializeJson(json_msg, message)){
      message = "\"" + message+"\"";
      USB_SERIAL.println(message);
      webSocket.emit("message", message.c_str());
    // Case 3 - data of the write format
    }else{
      // Case 4 - first upload
      if(message.substring(0,strlen(FIRST_UPLOAD_INDICATOR)) == FIRST_UPLOAD_INDICATOR){
        USB_SERIAL.println("in first upload");
        first_upload(json_msg);
      }
    }
  }
  webSocket.loop();
}