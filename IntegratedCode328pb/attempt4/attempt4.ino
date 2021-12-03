#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
//including basic arduino headers
#include <Arduino.h>
//necessary Adafruit BLE headers
#include "Adafruit_BluefruitLE_SPI.h"
#include "BLEConfig.h"

// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


Adafruit_SSD1306 dis(128, 64, &Wire, 0);
//service/char IDs
int32_t serviceID;
int32_t charID;
//for ease of use with method calls
int POP = 1;
int THIRTY_SEC = 2;
int DONE = 3;





unsigned long milliSeconds = 0;

/******************TESTING VARIABLES BEGIN ******************/
//BUTTONS FOR TESTING
int buttonOne = A0 ;

//LED TESTING ON BREADBOARD
int good = 6;//8; //8 on 328p
int error = 9; //9 on 328p
//int receivedLED = 26;
/******************TESTING VARIABLES END ******************/


//Setup the Hardware and BLE stuff
void setup() 
{
  dis.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  dis.display();
  dis.clearDisplay();
  dis.setTextSize(2);         
  dis.setTextColor(SSD1306_WHITE); 
  bool init = BLE_initialize();

  if(!init)
  {
    digitalWrite(error, HIGH);
    while(1);
  }
  digitalWrite(good, HIGH);
  
 

    
}

int count = 0;
void loop() 
{  

    //display successful display initialization
    dis.clearDisplay();   
    dis.setCursor(0,2);   
    dis.print(F("POP"));
    dis.display();
    //    //mark the time of display for timing
    count = (count + 1) % 3;
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("AAAA");


    milliSeconds = millis();
     //wait to display next message
    while(millis() < (milliSeconds + 1000)){
     //do nothing 
    }
 
   
}





//Setup the Hardware and BLE stuff
bool BLE_initialize(void) 
{  
  //check if connected to the BLE module - and pass in the debug variable
  if(!ble.begin(VERBOSE_MODE))
  {
     //if cannot connect to ble module, no point in continuing as it will not work
     //this is typically due to a mistake in hardware such as wiring
     return false;
  }

  //factory reset to kill any previous data stashed within the device 
  if(FACTORYRESET_ENABLE)
  {      
    //if cannot factory reset - issue msg to screen and halt execution with empty while loop 
    if(!ble.factoryReset())
    {
      return false;
    }
  }

  //change the name of the BLE device    
  if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Perfect Popcorn Popper")) ) {
    return false;
  }

  //estabalish the GATT Service and GATT characteristics
  boolean success;

  //if flag set to true, first clear all pre-existing gatt services and characteristics
  if(GATT_CLEAR_ALL)
  {
    success = ble.sendCommandCheckOK(F("AT+GATTCLEAR"));
    if(!success) 
    {
      //could not clear old GATT services
      return false;
    }
  }
  //establish service
  success = ble.sendCommandWithIntReply(F("AT+GATTADDSERVICE=UUID128=00-11-00-11-44-55-66-77-88-99-AA-BB-CC-DD-EE-FF"), &serviceID);
  if(!success) 
  {
    //could not establish main service, no comms may be completed
    return false;
  }
  //establish characteristic that is attached to the service just established
  success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x0002, PROPERTIES=0x12, MIN_LEN=1, MAX_LEN=20, VALUE=5"), &charID);
  if(!success)
  {
    //could not establish main characteristic under the service, no comms may be completed
     return false;
  }
  //issue ble reset to solidify changes made to the GATT services and characteristics
  ble.reset();

  if(WAIT_FOR_CONN)
  {
    //wait for an incoming connection  
    while(!ble.isConnected())
    {
      //wait for a connection
    }
  }
  
   //setup completed - wait a second
   delay(1000);
   return true;
}


//method for sending a notification, the one sent depends on what int is passed in
void BLE_send(int notification)
{
  //switch to determine which notification to send
  switch(notification)
  {
    //increment pops
    case 1:
      sendDataApp("AAAA", charID);
      break;
    //30 seconds notification
    case 2:
      sendDataApp("BBBB", charID);
      break;
    //done notification
    case 3:
      sendDataApp("CCCC", charID);  
      break;
    
    //default will be sending a pop
    default:
      sendDataApp("AAAA", charID);
  }
  return;
}


void sendDataApp(String dataOut, int32_t characteristic)
{
  //prepare the buffer and instruction for sending data
  ble.print( F("AT+GATTCHAR=") );
  ble.print( characteristic );
  ble.print( F(",") );
  ble.println(dataOut);
  return;
}
