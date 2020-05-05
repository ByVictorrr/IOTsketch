#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "SocketIoClient.h"
#include <SoftwareSerial.h>
#include <EEPROM.h>
// #include "CPutil.h"
#include <ArduinoJson.h>
#include <Scheduler.h>

#define FIRST_UPLOAD_INDICATOR "{\"username"
#define USB_SERIAL Serial
#define COM_SERIAL ss
#define MAX 20 // at 50 it gives a stack trace
#define SERVER_IP "192.168.1.252"
#define SERVER_PORT 80

#define null_str String('\0')
#define LED D3

#define GET_SSID "ssid"
#define GET_WIFI_PASS "wifi_pass"
#define GET_USERNAME "username"
#define GET_PASSWORD "password"

#define TX D2
#define RX D1
#define SSID_LOC  0
#define WPA_LOC MAX
#define USR_NAME_LOC 2*MAX
#define PASS_LOC 3*MAX


DynamicJsonDocument creds(400);
SoftwareSerial ss(RX, TX);
ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;


//=====================SOCKETIO=======================//
void join_room(const char * payload, size_t length){
  DynamicJsonDocument u_p_creds(200);
  String messageStr;
  u_p_creds["username"] = creds["username"];
  u_p_creds["password"] = creds["password"];
  serializeJson(u_p_creds, messageStr);
  webSocket.emit("connect bot", messageStr.c_str()); 
}
// recieved a msg
void message(const char * payload, size_t length){
  USB_SERIAL.printf("message %s\n", payload);
  // send to arduino
  COM_SERIAL.println(payload);
  digitalWrite(LED, HIGH);
}
void disconnect(const char * payload, size_t length){
  webSocket.disconnect();
  USB_SERIAL.println("disconnecting");
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
  char input;
  if(COM_SERIAL.available() > 0){
    data = COM_SERIAL.readStringUntil('\n');
    return data;
  }
  return null_str;
}
    
// Writes to the mega board
void write(String message){
  COM_SERIAL.print(message+'\n');
}

void write_EEPROM(char addr,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(addr+i,data[i]);
  }
  EEPROM.write(addr+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}

String read_EEPROM(int size, int base_addr){
  String data;
  for(int i = base_addr; i<base_addr+size; i++){
    USB_SERIAL.println("reading");
    data+=(char)EEPROM.read(i);
  }
  return data;
}
String read_EEPROM(char addr)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(addr);
  while(k != '\0' && len<MAX)   //Read until null character
  {    
    k=EEPROM.read(addr+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}


//=====================================================//

void setup(){
  USB_SERIAL.begin(9600);
  COM_SERIAL.begin(9600);
  EEPROM.begin(512);

  pinMode(LED, OUTPUT);
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
  
  creds[GET_SSID]=read_EEPROM(SSID_LOC);
  creds[GET_WIFI_PASS] = read_EEPROM(WPA_LOC);
  creds[GET_USERNAME] = read_EEPROM(USR_NAME_LOC);
  creds[GET_PASSWORD] = read_EEPROM(PASS_LOC);
  setup_WiFi(&WiFiMulti);
  // Step 2 - socketio stuff
  webSocket.begin(SERVER_IP, SERVER_PORT, "/socket.io/?transport=websocket");
  webSocket.on("join room", join_room);
  webSocket.on("message", message);
  webSocket.on("disconnect", disconnect);
}

void first_upload(DynamicJsonDocument &json_msg){
    for (int i = 0; i < EEPROM.length(); i++){
      EEPROM.put(i, 0);
    }
    EEPROM.commit();
    write_EEPROM(SSID_LOC, json_msg[GET_SSID]);
    write_EEPROM(WPA_LOC, json_msg[GET_WIFI_PASS]);
    write_EEPROM(USR_NAME_LOC, json_msg[GET_USERNAME]);
    write_EEPROM(PASS_LOC, json_msg[GET_PASSWORD]);
}

void loop(){

  String message;
  DynamicJsonDocument json_msg(400);
  message.reserve(MAX);

  // Case 1 - data has been received
  if((message=recieve()) != null_str){

    USB_SERIAL.println(message);
    // Case 2 - regular message from arduino(general message)
    if(deserializeJson(json_msg, message)){
      message = "\"" + message+"\"";
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