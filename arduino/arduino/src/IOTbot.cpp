#include <Arduino.h>
#include <ArduinoJson.h>
#include <NeoSWSerial.h>
#include "IOTbot.h"


     /**
     * Constructor for IOTbot
     * @param rx recieving pin of the mega from the NodeMCU
     * @param rx transmissing pin of the mega from the NodeMCU
     */
    IOTbot::IOTbot(const unsigned int rx, const unsigned int tx, const unsigned debugPin, 
          const char * ssid, const char *wpa, const char *username, const char *password,
          boolean isFirstUpload)
          : ns(rx, tx), debugPin(debugPin), ssid(ssid), wpa(wpa), username(username), password(password),
          isFirstUpload(isFirstUpload)
    {
      pinMode(debugPin, OUTPUT);
      // For credentials

      // Arduino recieve wont work if its not any of these numbers
      if(!(rx==10 || rx==11 || rx==12 || rx==13 || rx==14 ||
         rx==15 || rx==50 || rx==51 || rx==52 || rx==53 ||
         rx==62 || rx==63 || rx==64 || rx==65 || rx==66 ||
         rx==67 || rx==68 || rx==69))
        {
          /*
          Serial.println("Reciever pins can only be those shown below");
          Serial.print("10,11,12,13,14,15,50,51");
          Serial.println(",52,53,62,63,64,65,66,67,68,69");
          */
         // keeps blinking to notify the user that it cant be used
         while(1){
          digitalWrite(debugPin, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(1000);                       // wait for a second
          digitalWrite(debugPin, LOW);    // turn the LED off by making the voltage LOW
          delay(1000);                       // wait for a second
         }
        }
     }
    /**
     * Function to set the baud rate for the serial communication for the nodeMCU and the mega
     * Also it send the credentials to the esp board
     * @param baud the baud rate for these rx, and tx to run at
     */
    void IOTbot::begin(unsigned long baud=9600){
      ns.begin(baud);
      // First time uploading your credentials to the NodeMCU
      if(this->isFirstUpload)
      {
        DynamicJsonDocument creds(500);
        String strCreds = "";
        // strCreds.reserve(400);
        creds["username"] = this->username;
        creds["password"] = this->password;
        creds["wifi_pass"] = this->wpa;
        creds["ssid"] = this->ssid;
        serializeJson(creds, strCreds);
        Serial.println(strCreds);
        send(strCreds);
        delay(5000);
      }

    }
    /**
     * Function is used to send a message to the NodeMCU 
     * @param message is the data to be sent 
     */
    void IOTbot::send(char * message){
      ns.println(message + '\n');
    }    
    void IOTbot::send(String &message){
      ns.print(message+'\n');
    }
    /**
     * Used to read a message from the NodeMCU
     * @return a string read from the NodeMCU terminated with a new line or a null string
     */
    String IOTbot::recieve(){  
      if(ns.available() > 0){
         return ns.readStringUntil('\n');
      }
      return null_str;
    }

