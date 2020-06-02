#ifndef IOTBOT_H_
#define IOTBOT_H_
#include <Arduino.h>
#include <ArduinoJson.h>
#include <NeoSWSerial.h>

#define null_str String('\0')

class IOTbot{
  private:
    NeoSWSerial ns;
    int debugPin;
    // Credential stuff
    bool isFirstUpload;
    String ssid, wpa, username, password;
    IOTbot();
  public:
     /**
     * Constructor for IOTbot
     * @param rx recieving pin of the mega from the NodeMCU
     * @param rx transmissing pin of the mega from the NodeMCU
     */
    IOTbot(const unsigned int rx, const unsigned int tx, const unsigned debugPin, 
          const char * ssid = nullptr, const char *wpa=nullptr, const char *username=nullptr, const char *password=nullptr,
          boolean isFirstUpload = false);
    /**
     * Function to set the baud rate for the serial communication for the nodeMCU and the mega
     * @param baud the baud rate for these rx, and tx to run at
     */
    void begin(unsigned long baud=9600);
    /**
     * Function is used to send a message to the NodeMCU 
     * @param message is the data to be sent 
     */
    void send(char * message);
    void send(String &message);
    /**
     * Used to read a message from the NodeMCU
     * @return a string read from the NodeMCU terminated with a new line or a null string
     */
    String recieve();
};
#endif