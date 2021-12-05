
//pin numbers 23 24
//including basic arduino headers
#include <Arduino.h>
//necessary Adafruit BLE headers
#include "Adafruit_BluefruitLE_SPI.h"
#include "config.h"


/***************************************************************************/
/*  START DETECT VARIABLES
/***************************************************************************/

/***************************************************************************/
/*  END DETECT VARIABLES
/***************************************************************************/


/***************************************************************************/
/* START FSM AND TIMING VARIABLES
/***************************************************************************/
//counting variables
unsigned int popCt = 0;//number of pops found

//timing variables to set
unsigned long popLedOnAt = 0;
unsigned long milliSeconds = 0; //used to hold the current number of milliseconds
unsigned long popDetectedAt = 0;// used to hold the number of milliseconds when the most recent pop was detected
unsigned long startTime= 0;
unsigned long temp = 0; //used for calculations out side of if statement

//quick values to use
unsigned long popLedDuration = 100;
unsigned long maxTime = 180000;//the number of milliseconds in three minutes
unsigned long minTime = 160000;//this a minute and a half
unsigned long fifteenSeconds = 15000;
unsigned long halfSecond = 500;
unsigned long timeAfterPopDone = fifteenSeconds;
unsigned long timeAfterPopAlmost = 5000;//fiveseconds


/***************************************************************************/
/*  END FSM AND TIMING VARIABLES
/* 
/***************************************************************************/

/***************************************************************************/
/*  START BLUETOOTH VARIABLES
/* 
/***************************************************************************/
// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
//service/char IDs
int32_t serviceID;
int32_t charID;

/***************************************************************************/
/*  END BLUETOOTH VARIABLES
/* 
/***************************************************************************/


// SETUP START ***********************************************************************************
void setup() {
  //delay needed because the MCU does not restart the sketch without reprogramming unless this is here
  delay(500);
  //Initialize the Screen First
  //Screen is first incase we need to display errors in other initilizing steps
  displaySetUp();
  delay(100);
  
  //Initialize PopDetection Second
  //pop detection is second becuase it is the most important part
//  detectSetUp();
  delay(100);
  
  //Initialize BlueTooth Third
  //bluetooth initilization should utilize the display
  //No matter what after X attempts to connect move past because 
  //popcorn should be starting to pop
  bleSetup();
  delay(1000);

  startRoutine();
}
// SETUP END ***********************************************************************************

// LOOP START ***********************************************************************************
void loop() {
  //detectLoop();
  //Force a pop every second 
  milliSeconds = millis();
  if(milliSeconds > popDetectedAt + 1000){
    popDetectedRoutine();
  }
  //turn off leds if they're on after a bit of time
  //this is used instead of delay() because we don't want the algorithm to stop pulling values
  milliSeconds = millis();
  temp = popLedOnAt + popLedDuration;
  if(milliSeconds > temp  ){
    clearPop();
  }
  
  //Loop over pop detection
  
  milliSeconds = millis();
  temp = startTime + maxTime;
  if(milliSeconds > temp){
    doneRoutine();
  }
  temp = startTime + minTime;
  if(milliSeconds > temp){
      if(milliSeconds > (timeAfterPopDone + popDetectedAt) ){
          doneRoutine();
      }
      if(milliSeconds > (timeAfterPopAlmost + popDetectedAt)){
          almostDoneRoutine();
      }
  }
 
  
  
}
// LOOP END ***********************************************************************************

//BEGIN ROUTINE ***************************************************
void startRoutine(){
  clearDone();
  clearPop();
  clearAlmostDone();
  //reset variables
  popCt = 0;
  startTime = millis();
  
}
//POP ROUTINE
void popDetectedRoutine(){
  popDetectedAt = millis();
  //update display for user
  showPop();
  //update phone for user
  ble.print( F("AT+GATTCHAR=") );
  ble.print( charID );
  ble.print( F(",") );
  ble.println("AAAA");
}



void almostDoneRoutine(){
    milliSeconds = millis();
  
    //First update Display as it is the primary use of communication
    showAlmostDone();
  
    //Second update phone by bluetooth
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("BBBB");

}


//FINISH ROUTINE
void doneRoutine(){
  milliSeconds = millis();
  
    //First update Display as it is the primary use of communication
    showDone();
  
    //Second update phone by bluetooth
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("CCCC");
  
    
    tune();
    while( millis() > milliSeconds + fifteenSeconds){
      
    }
  
  startRoutine();
}
//END ROUTINE ***************************************************

/***************************************************************************/
/*   START   POP CODE          */
/***************************************************************************/


/***************************************************************************/
/*   END     FAKE_POP CODE          */
/***************************************************************************/

/***************************************************************************/
/*   START     BUZZER CODE          */
/***************************************************************************/

void tune(){
  tone(BUZZERPIN, 1047);

  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(BUZZERPIN);

  tone(BUZZERPIN, 1047*2);

  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(BUZZERPIN);
  tone(BUZZERPIN, 1047);
  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(BUZZERPIN);
  tone(BUZZERPIN, 1047*2);

  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(BUZZERPIN);
}
/***************************************************************************/
/*   END     BUZZER CODE          */
/***************************************************************************/



// START BLE CODE *****************************************************************************
//Setup the Hardware and BLE stuff
void bleSetup(){  
  bool init = BLE_initialize();    
}

//Setup the Hardware and BLE stuff
bool BLE_initialize(void){  
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
   delay(1000);
   return true;
}
// END BLE CODE *******************************************************************

/***************************************************************************/
/*   START     DISPLAY CODE          */
/***************************************************************************/
void displaySetUp(){
  pinMode(POPPIN,OUTPUT);
  pinMode(DONEPIN, OUTPUT);
  pinMode(ALMOSTPIN, OUTPUT);
}
/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showPop(){
  digitalWrite(POPPIN, HIGH);
  popLedOnAt = millis();
}
void clearPop(){
  digitalWrite(POPPIN, LOW);
  
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showDone(){
  digitalWrite(DONEPIN, HIGH);
}

void clearDone(){
  digitalWrite(DONEPIN, LOW);
}
/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showAlmostDone(){
  digitalWrite(ALMOSTPIN, HIGH);
}
void clearAlmostDone(){
  digitalWrite(ALMOSTPIN, LOW);
}

/***************************************************************************/
/*   END     DISPLAY CODE          */
/***************************************************************************/
