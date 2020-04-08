#include <Arduino.h>
#include <ArduinoJson.h>
#include <NeoSWSerial.h>

#define USB_SERIAL Serial
#define COM_SERIAL ns

#define IS_FIRST_UPLOAD false

#define SSID "ATTmhSCTEa"
#define WIFI_PASS "6505764388"
#define USERNAME "byvictorrr"
#define PASSWORD "calpoly"
#define NULL String('\0')


#define RX 10
#define TX 45

class NodeMCU{
  private:
    NeoSWSerial ns;
  public:
    NodeMCU(unsigned const int rx, unsigned const int tx)
      : ns(rx, tx)
    {

    }
    void send(const char * message){
      COM_SERIAL.println(message);
      delay(1000);
    }
    void send(const String &message){
      ns.println(message);
    }
    String &recieve(){  
      String data;
      if(COM_SERIAL.available() > 0){
        data = COM_SERIAL.readStringUntil('\n');
      }else{
        data = NULL;
      }
      return data;
    }


};

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
  COM_SERIAL.begin(9600);
 }


void loop() {
 if(IS_FIRST_UPLOAD){
    sendCreds();
  }else{
    static String message;
    DynamicJsonDocument json_msg(400);
    //========= read commands ==========//
    // Case 1 - nothing is received
    if((message = recieve()) != NULL){
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
    send(message);
    //==================================//
  }
  //delay(2000);
}