//including basic arduino headers
#include <Arduino.h>
//necessary Adafruit BLE headers
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BLEConfig.h"
#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

//option 2
// software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST 
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);



//Setup the Hardware and BLE stuff
void setup() 
{
    //while serial port has not been initialize, wait and then try again
    while(!Serial);
  
   //activate the serial port with a BAUD rate of 115200
   Serial.begin(115200);
  
   //check if connected to the BLE module - and pass in the debug variable
   if(!ble.begin(VERBOSE_MODE))
   {
      Serial.println(F("Could not connect to the BLE module, check wiring"));
   }
   
   //if BLE module connected and initialized properly
   Serial.println(F("Connected to BLE Module!!"));
  
   //factory reset to kill any previous data stashed within the device 
   if(FACTORYRESET_ENABLE)
   {
      //issue command to reset
      Serial.println(F("Performing a factory reset"));
      //if cannot factory reset - issue msg to screen and halt execution with empty while loop 
      if(!ble.factoryReset())
      {
        Serial.println(F("Couldn't factory reset"));
        while(1);
      }
   }

   //change the name of the BLE device 
   if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Perfect Popcorn Popper")) ) {
    Serial.println(F("Could not set device name?"));
   }
  
   //enable cmd echo from BLE module to ensure correct things are being read
   ble.echo(true);
  
   //get info from BLE module
   Serial.println("Requesting BLE info");
   ble.info();

   //set into CMD mode
   //Serial.println("Switching to CMD mode");
  // ble.setMode(BLUEFRUIT_MODE_DATA);

   //turn off verbose/debug info
   ble.verbose(false);

   //allow connection to finish
   while(!ble.isConnected())
   {
      //Serial.println("Awaiting Bluetooth connection...");
      delay(500);
   }

   //connection completed - wait a second
   delay(1000);

   Serial.println(F("CONNECTED"));
   Serial.println(F(" :) ")); 
}

void loop() 
{
  // Check for incoming characters from Bluefruit
  receiveDataBLE();

  //testing by pressing buttons and sending result over BLE
  //pressing first button produces "First" output
  //pressing second button produces "second" output

  //check button one to send 0xEA
  if(digitalRead(11) == HIGH)
  {
    sendDataBLE("First");
    delay(2000);
  }
  //check button one to send 0xEA
  if(digitalRead(12) == HIGH)
  {
    sendDataBLE("Second");
    delay(2000);
  }
    
}

void sendDataBLE(String dataOut)
{    
  //char buffer for sending data
  char dataSend[BUFSIZE+1];
  //create char array from passed in string
  dataOut.toCharArray(dataSend, BUFSIZE+1);

  //testing
  Serial.print(F("[Send] "));
  Serial.println(dataOut);

  //NOTE: Command is sent when \n (\r) or println is called to BLE
  
  //Set ble mode to TX 
  ble.print("AT+BLEUARTTX=");
  //send the passed in char array over bluetooth
  ble.println(dataSend);

  //make sure BLE module sent data
  if(!ble.waitForOK())
  {
    Serial.println("Could not send data");
  }    
}

void receiveDataBLE()
{
  //set BLE module mode to RX
  ble.println("AT+BLEUARTRX");
  //read incoming data
  ble.readline();
  //strcmp returns a 0 when strings are equal, BLE module buffer as OK when empty
  if (strcmp(ble.buffer, "OK") == 0) 
  {
    // no data
    return;
  }
  else
  {
    // Some data was found, its in the buffer, print to console
    Serial.print(F("[Recv] ")); 
    Serial.println(ble.buffer);
    ble.waitForOK();
  }
}
