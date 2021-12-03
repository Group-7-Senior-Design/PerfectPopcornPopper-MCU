
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
//including basic arduino headers
#include <Arduino.h>
//necessary Adafruit BLE headers
#include "Adafruit_BluefruitLE_SPI.h"
#include "BLEConfig.h"
/***************************************************************************/
/* START Varius FSM and Timing Variables

/* 
/***************************************************************************/
unsigned int runs = 0; //number of times FSM started
    
unsigned long milliSeconds = 0; //used to hold the current number of milliseconds
unsigned long popDetectedAt = 0;// used to hold the number of milliseconds when the most recent pop was detected
unsigned long threeMinutes = 180000;//the number of milliseconds in three minutes
unsigned long oneMinute = 60000;
unsigned long fifteenSeconds = 15000;
unsigned long halfSecond = 500;
unsigned long startTime= 0;
unsigned int popCt = 0;


/***************************************************************************/
/*  END Varius FSM and Timing Variables
/* 
/***************************************************************************/
// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
//service/char IDs
int32_t serviceID;
int32_t charID;

//declare and initialize library
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int detectPin = 2;//pins 2/PD0 and 3/PD1 

int buttonDebounce = 20;
int previousState = HIGH;
unsigned int previousPress;
volatile int buttonFlag;
//VARIABLES AND INCLUDES END ********************************************************************



// SETUP START ***********************************************************************************
void setup() {
  //delay needed because the MCU does not restart the sketch without reprogramming unless this is here
  delay(500);
  //Initialize the Screen First
  //Screen is first incase we need to display errors in other initilizing steps
  displaySetUp();
  showString("Display Set Up Done");
  delay(100);
  
  //Initialize PopDetection Second
  //pop detection is second becuase it is the most important part
  detectSetUp();
  showString("Detect Set Up Done");
  delay(100);
  
  //Initialize BlueTooth Third
  //bluetooth initilization should utilize the display
  //No matter what after X attempts to connect move past because 
  //popcorn should be starting to pop
//  bleSetup();
  showString("BLE Set Up Done");
  delay(1000);
  
  //Initialize Buzzer Last
  //buzzer is used as a back up communicaiton over distance
  //tones should be varying and cover a wide part of the frequency spectrum
 
  startRoutine();
}
// SETUP END ***********************************************************************************

// LOOP START ***********************************************************************************
void loop() {

  if((millis() - previousPress) > buttonDebounce && buttonFlag)
  {
    previousPress = millis();
    if(digitalRead(detectPin) == LOW && previousState == HIGH)
    {
      //ledToggle =! ledToggle;
     //digitalWrite(ledPin, ledToggle);
      popDetectedAt = millis();
      popDetectedRoutine();
      
      previousState = LOW;
    }
    
    else if(digitalRead(detectPin) == HIGH && previousState == LOW)
    {
      previousState = HIGH;
    }
    buttonFlag = 0;
  }

  //Loop over pop detection
  //pop detection will set a timer between  
  
  milliSeconds = millis();
  if(milliSeconds > (startTime + (threeMinutes * runs))){
    doneRoutine();
  }
  if(milliSeconds > (startTime + (oneMinute * runs))){
      if(milliSeconds > fifteenSeconds + popDetectedAt ){
          doneRoutine();
      }
  }
 
  
  
}
// LOOP END ***********************************************************************************

//BEGIN ROUTINE ***************************************************
void startRoutine(){
  //reset variables
  popCt = 0;
  showWorking();
  runs = runs + 1;
  startTime = millis();
}
//POP ROUTINE
void popDetectedRoutine(){
  //update count
  popCt = popCt + 1;
  //update display for user
  showValue(popCt);
  showValue2(popDetectedAt);
  //update phone for user
//  ble.print( F("AT+GATTCHAR=") );
//  ble.print( charID );
//  ble.print( F(",") );
//  ble.println("AAAA");
}
//FINISH ROUTINE
void doneRoutine(){
  //First update Display as it is the primary use of communication
  showDone();
  //Second update phone by bluetooth
  //prepare the buffer and instruction for sending data
//  ble.print( F("AT+GATTCHAR=") );
//  ble.print( charID );
//  ble.print( F(",") );
//  ble.println("CCCC");
  
  tune();
  
  startRoutine();
}
//END ROUTINE ***************************************************

/***************************************************************************/
/*   START     FAKE_POP CODE          */
/***************************************************************************/
void detectSetUp(){
  pinMode(detectPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(detectPin), detectLoop, CHANGE);
}

//this is either filled with proper detectLoop and used in Loop()
//or it is used as an ISR function for pin detection mode.
void detectLoop(){
 buttonFlag = 1;
}
/***************************************************************************/
/*   END     FAKE_POP CODE          */
/***************************************************************************/

/***************************************************************************/
/*   START     BUZZER CODE          */
/***************************************************************************/

int buzzer = 5;


void tune(){
  tone(buzzer, 1047);

  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(buzzer);

  tone(buzzer, 1047*2);

  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(buzzer);
  tone(buzzer, 1047);
  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(buzzer);
  tone(buzzer, 1047*2);

  milliSeconds = millis();
  while(millis() < (halfSecond)+ milliSeconds){
   
  }
  noTone(buzzer);
}
/***************************************************************************/
/*   END     BUZZER CODE          */
/***************************************************************************/



// START BLE CODE *****************************************************************************
//Setup the Hardware and BLE stuff
//void bleSetup() 
//{  
//  bool init = BLE_initialize();    
//}
//
////Setup the Hardware and BLE stuff
//bool BLE_initialize(void) 
//{  
//  //check if connected to the BLE module - and pass in the debug variable
//  if(!ble.begin(VERBOSE_MODE))
//  {
//     //if cannot connect to ble module, no point in continuing as it will not work
//     //this is typically due to a mistake in hardware such as wiring
//     return false;
//  }
//
//  //factory reset to kill any previous data stashed within the device 
//  if(FACTORYRESET_ENABLE)
//  {      
//    //if cannot factory reset - issue msg to screen and halt execution with empty while loop 
//    if(!ble.factoryReset())
//    {
//      return false;
//    }
//  }
//
//  //change the name of the BLE device    
//  if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Perfect Popcorn Popper")) ) {
//    return false;
//  }
//
//  //estabalish the GATT Service and GATT characteristics
//  boolean success;
//
//  //if flag set to true, first clear all pre-existing gatt services and characteristics
//  if(GATT_CLEAR_ALL)
//  {
//    success = ble.sendCommandCheckOK(F("AT+GATTCLEAR"));
//    if(!success) 
//    {
//      //could not clear old GATT services
//      return false;
//    }
//  }
//  //establish service
//  success = ble.sendCommandWithIntReply(F("AT+GATTADDSERVICE=UUID128=00-11-00-11-44-55-66-77-88-99-AA-BB-CC-DD-EE-FF"), &serviceID);
//  if(!success) 
//  {
//    //could not establish main service, no comms may be completed
//    return false;
//  }
//  //establish characteristic that is attached to the service just established
//  success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x0002, PROPERTIES=0x12, MIN_LEN=1, MAX_LEN=20, VALUE=5"), &charID);
//  if(!success)
//  {
//    //could not establish main characteristic under the service, no comms may be completed
//     return false;
//  }
//  //issue ble reset to solidify changes made to the GATT services and characteristics
//  ble.reset();
//
//  if(WAIT_FOR_CONN)
//  {
//    //wait for an incoming connection  
//    while(!ble.isConnected())
//    {
//      //wait for a connection
//    }
//  }
//  
//   //setup completed - wait a second
//   showConnected();
//   delay(1000);
//   return true;
//}
// END BLE CODE *******************************************************************

/***************************************************************************/
/*   START     DISPLAY CODE          */
/***************************************************************************/
/*
 * 
 * clear the current display
 * set text size and color
 */
void displaySetUp(){
  //begin display usage
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.display();
  //clear buffer
  display.clearDisplay();
  display.setTextSize(2);  
  // Draw white text           
  display.setTextColor(SSD1306_WHITE); 

}
/*
 * prepare the screen
 * clear the current display
 * set text size and color
 * set cursor position
 */
void prePrint(){
  //clear the display
  display.clearDisplay();
  // Normal 1:1 pixel scale
  // Start at top-left corner       
  display.setCursor(0,10);         
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showConnecting(){
  prePrint();
  display.println(F("Connecting..."));
  display.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showConnected(){
  prePrint();
  display.println(F("Bluetooth"));
  display.println(F("is on!"));
  display.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showWorking(){
  prePrint();
  //populate buffer
  display.println(F("Optimizing"));
  display.println(F("Popcorn..."));
  //push to display
  display.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showDone(){
  prePrint();
  display.println(F("Done!"));
  display.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showAlmostDone(){
  prePrint();
  display.println(F("Almost"));
  display.println(F("Done!"));
  display.display();
}
/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showValue(int v){
  prePrint();
  display.print(F("Pops Ct: "));
  display.println(v);
  display.display();
}
void showString(String v){
  prePrint();
  display.println(v);
  display.display();
}
/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showValue2(int v){
  display.print(F("Time: "));
  display.println(v);
  display.display();
}

/***************************************************************************/
/*   END     DISPLAY CODE          */
/***************************************************************************/
