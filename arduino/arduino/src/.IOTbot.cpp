#include "IOTbot.h"
#include <Arduino.h>
#include <ArduinoJson.h>

/**
 * Constructor for IOTbot
 * @param rx recieving pin of the mega from the NodeMCU
 * @param rx transmissing pin of the mega from the NodeMCU
 */
IOTbot::IOTbot(const unsigned int rx, const unsigned int tx,
               const char * ssid, const char *wpa, const char *username, const char *password,
               boolean isFirstUpload)
  : ns(new NeoSWSerial(rx, tx)), debug(&Serial)
{
  if(ns == nullptr){
    debug->println("Dynamic memory error");
  }
  debug->begin(rx);
  // Arduino recieve wont work if its not any of these numbers
  if(rx!=10 || rx!=11 || rx!=12 || rx!=13 || rx!=14 ||
      rx!=15 || rx!=50 || rx!=51 || rx!=52 || rx!=53 ||
      rx!=62 || rx!=63 || rx!=64 || rx!=65 || rx!=66 ||
      rx!=67 || rx!=68 || rx!=69)
    {
      debug->println("Reciever pins can only be those shown below");
      debug->print("10,11,12,13,14,15,50,51");
      debug->println(",52,53,62,63,64,65,66,67,68,69");
    }
  // First time uploading your credentials to the NodeMCU
  if(isFirstUpload)
  {
    DynamicJsonDocument creds(400);
    String strCreds;
    creds["ssid"] = ssid;
    creds["wpa"] = wpa;
    creds["username"] = username;
    creds["password"] = password;
    serializeJson(creds, strCreds);
    send(strCreds);
  }
  debug->println("hi");
}
/**
 * Destructor for IOTbot, clears dynamic memory allocated
 */
IOTbot::~IOTbot(){
  if(ns!=nullptr){
    free(ns);
  }
}
/**
 * Function to set the baud rate for the serial communication for the nodeMCU and the mega
 * @param baud the baud rate for these rx, and tx to run at
 */
void IOTbot::begin(unsigned long baud){
    ns->begin(baud);
}
/**
 * Function is used to send a message to the NodeMCU 
 * @param message is the data to be sent 
 */
void IOTbot::send(char * message){
  ns->println(message);
}    
void IOTbot::send(String &message){
  ns->println(message);
}
/**
 * Used to read a message from the NodeMCU
 * @return a string read from the NodeMCU terminated with a new line or a null string
 */
String IOTbot::recieve(){  
  if(ns->available() > 0){
      return ns->readStringUntil('\n');
  }
  return null_str;
}