# IOTsketch (Welcome CPE123)

## Prerequisites
* Hardware connection
* VScode
* PlatformIO
  * Install Libraries from PlatformIO: ArduinoJson, SocketioClient, EspSoftwareSerial, SoftwareSerial, WebSockets, and NeoSWSerial

## 1. Getting Started
* Once VScode and PlatformIO is installed.
* clone this repo: git clone https://github.com/byvictorrr/IOTsketch
* Open the esp folder with VScode as a PlatformIO project.
* Locate the src folder and find main.cpp
  * Look for SERVER_IP and SERVER_PORT Macros and change them to whatever CPE123 specifies
* Finally upload (on the side panel click on the alien icon and click upload and monitor)

## 2. Registering and First upload
* You will want to go to the website provided by Dr. Smith and register for an account.
* Open the arduino folder with VScode, enter your credentials in main.cpp
  * For SSID - thats the wifi name
  * For WPA - password for the network
  * For USERNAME - this is the name you used to register your account
  * For PASSWORD - password you used to register your account
  * For IS_FIRST_UPLOAD - put true (after the first sketch you wont need these credentials)
  
* Finally its time to upload your sketch, on the side panel click on the alien icon and click upload and monitor

* After this step your credentials should be saved on the esp's eeprom, so when you reboot it/unplug it everything should be saved
 
## 3. Connecting to the server
* On your computer go to the IP address gave out by Dr. Smith (make sure your logged in) and go to "Controll bot"

![controll-bot](https://i.imgur.com/9UqwoXA.png)

* Once you see the bot connected to your room (this is your username), you can start sending commands from the arduino to the server directly and vice versa.

**NOTE: The Server wont let the ESP connect unless the client has connected already**

## 4. IOTbot API ( arduino/arduino/IOTbot.h )
0. IOTbot(rx, tx, debugPin, ssid, wpa, username, password, isFirstUpload): the first 3 parms are required, but the others are used on the first upload to store the credentials on the esp.  
1. send(string/c style string): send a message to the server
2. recieve(): get a message from the server





