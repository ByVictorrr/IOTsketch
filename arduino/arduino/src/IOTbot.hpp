#ifndef IOTBOT_H_
#define IOTBOT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <NeoSWSerial.h>
#define null_str String('\0')

class IOTbot{
  private:
    NeoSWSerial ns;
    HardwareSerial *debug;
  public:
    /**
     * Constructor for IOTbot
     * @param rx recieving pin of the mega from the NodeMCU
     * @param rx transmissing pin of the mega from the NodeMCU
     */
    IOTbot(const unsigned int rx, const unsigned int tx, boolean isFirstUpload,
           const char * ssid, const char *wpa, const char *username, const char *password)
      : ns(rx, tx), debug(&Serial)
    {
      // Arduino recieve wont work if its not any of these numbers
      if(rx!=10 || rx!=11 || rx!=12 || rx!=13 || rx!=14 ||
         rx!=15 || rx!=50 || rx!=51 || rx!=52 || rx!=53 ||
         rx!=62 || rx!=63 || rx!=64 || rx!=65 || rx!=66 ||
         rx!=67 || rx!=68 || rx!=69)
        {
          debug->println("Reciever pin cant be those prompted");
          debug->println("Choose one of the pins 10,11,12,13,14,15,50,51,52,53,62,63,64,65,66,67,68,69");
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
    }
    /**
     * Function to set the baud rate for the serial communication for the nodeMCU and the mega
     * @param baud the baud rate for these rx, and tx to run at
     */
    void begin(unsigned long baud=9600){
        ns.begin(baud);
    }
    /**
     * Function is used to send a message to the NodeMCU 
     * @param message is the data to be sent 
     */
    void send(char * message){
      ns.println(message);
    }    
    void send(String &message){
      ns.println(message);
    }
    /**
     * Used to read a message from the NodeMCU
     * @return a string read from the NodeMCU terminated with a new line or a null string
     */
    String recieve(){  
      if(ns.available() > 0){
         return ns.readStringUntil('\n');
      }
      return null_str;
    }


};
#endif