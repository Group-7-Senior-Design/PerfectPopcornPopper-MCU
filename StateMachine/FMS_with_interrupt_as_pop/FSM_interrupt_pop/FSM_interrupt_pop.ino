
/***************************************************************************/
/*   START     DISPLAY CODE          */
/***************************************************************************/
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
//declare and initialize library
Adafruit_SSD1306 display(128, 64, &Wire, -1);
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
  display.println(F("Connected!"));
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
  display.println(F("Optimizing Popcorn..."));
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
/***************************************************************************/
/*   START     BLUETOOTH CODE          */
/***************************************************************************/

// //including basic arduino headers
// #include <Arduino.h>
// //necessary Adafruit BLE headers
// #include <SPI.h>
// #include "Adafruit_BluefruitLE_SPI.h"
// //#include "BLEConfig.h"
//
////Bluetooth setup vars
////this is the size of the read buffer for incoming data
//#define BUFSIZE             128 
////this setting enables debug output
//#define VERBOSE_MODE        true 
////clear all GATT services/chars if set to 1
//#define GATT_CLEAR_ALL    0 
////setting this flag to 1 will halt initialization until an end device connects
//#define WAIT_FOR_CONN   0 
//
////SPI Pin settings tailored to ATMega328p/pb
////Arbitrarily selected pins- can be any GPIO 
////5 //WORKS //0 //PD0 - 30
//#define BLUEFRUIT_SPI_CS  8
////6 //WORKS //1 //PD1 - 31
//#define BLUEFRUIT_SPI_IRQ 7
////7 //WORKS //7 //optional, set as -1 if unused -> PD7 - 11
//#define BLUEFRUIT_SPI_RST 4
//
////hardware specific to the 328p SMD - MUST BE THESE 
////0//13 //PB5 - 17
//#define BLUEFRUIT_SPI_SCK    13
////1//12 //PD5 - 16
//#define BLUEFRUIT_SPI_MISO   12
////2//11 //PD6 - 15
//#define BLUEFRUIT_SPI_MOSI   11
//
//#define FACTORYRESET_ENABLE 0
//
// // hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
// Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
// //service/char IDs
// int32_t serviceID;
// int32_t charID;
// //for ease of use with method calls
// int POP = 1;
// int THIRTY_SEC = 2;
// int DONE = 3;
//
//
// /******************TESTING VARIABLES BEGIN ******************/
// //BUTTONS FOR TESTING
// int buttonOne = A0 ;
//
// //LED TESTING ON BREADBOARD
// int good = 6;//8; //8 on 328p
// int error = 9; //9 on 328p
// //int receivedLED = 26;
// /******************TESTING VARIABLES END ******************/
//
//
// //Setup the Hardware and BLE stuff
// void bleSetup() 
// {
//   //setting up LEDs for testing
// //  pinMode(error, OUTPUT);
// //  pinMode(good, OUTPUT);
// //  
//   bool init = BLE_initialize();
//
// //  if(!init)
// //  {
// //    digitalWrite(error, HIGH);
// //    while(1);
// //  }
// //  digitalWrite(good, HIGH);
// //    
// }
// //Setup the Hardware and BLE stuff
// bool BLE_initialize(void) 
// {  
//   //check if connected to the BLE module - and pass in the debug variable
//   if(!ble.begin(VERBOSE_MODE))
//   {
//      //if cannot connect to ble module, no point in continuing as it will not work
//      //this is typically due to a mistake in hardware such as wiring
//      return false;
//   }
//   //factory reset to kill any previous data stashed within the device 
//   if(FACTORYRESET_ENABLE)
//   {      
//     //if cannot factory reset - issue msg to screen and halt execution with empty while loop 
//     if(!ble.factoryReset())
//     {
//       return false;
//     }
//   }
//   //change the name of the BLE device    
//   if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Perfect Popcorn Popper")) ) {
//     return false;
//   }
//
//   //estabalish the GATT Service and GATT characteristics
//   boolean success;
//
//   //if flag set to true, first clear all pre-existing gatt services and characteristics
//   if(GATT_CLEAR_ALL)
//   {
//     success = ble.sendCommandCheckOK(F("AT+GATTCLEAR"));
//     if(!success) 
//     {
//       //could not clear old GATT services
//       return false;
//     }
//   }
//   //establish service
//   success = ble.sendCommandWithIntReply(F("AT+GATTADDSERVICE=UUID128=00-11-00-11-44-55-66-77-88-99-AA-BB-CC-DD-EE-FF"), &serviceID);
//   if(!success) 
//   {
//     //could not establish main service, no comms may be completed
//     return false;
//   }
//   //establish characteristic that is attached to the service just established
//   success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x0002, PROPERTIES=0x12, MIN_LEN=1, MAX_LEN=20, VALUE=5"), &charID);
//   if(!success)
//   {
//     //could not establish main characteristic under the service, no comms may be completed
//      return false;
//   }
//   //issue ble reset to solidify changes made to the GATT services and characteristics
//   ble.reset();
//
//   if(WAIT_FOR_CONN)
//   {
//     //wait for an incoming connection  
//     while(!ble.isConnected())
//     {
//       //wait for a connection
//       showConnecting();
//     }
//   } 
//    showConnected();
//    //setup completed - wait a second
//    delay(1000);
//    return true;
// }
//
//
// void sendDataApp(String dataOut, int32_t characteristic)
// {
//   //prepare the buffer and instruction for sending data
//   ble.print( F("AT+GATTCHAR=") );
//   ble.print( characteristic );
//   ble.print( F(",") );
//   ble.println(dataOut);
//
// }
// //method for sending a notification, the one sent depends on what int is passed in
// void BLE_send(int notification)
// {
//   //switch to determine which notification to send
//   switch(notification)
//   {
//     //increment pops
//     case 1:
//       sendDataApp("AAAA", charID);
//       break;
//     //30 seconds notification
//     case 2:
//       sendDataApp("BBBB", charID);
//       break;
//     //done notification
//     case 3:
//       sendDataApp("CCCC", charID);  
//       break;
//    
//     //default will be sending a pop
//     default:
//       sendDataApp("AAAA", charID);
//   }
//   return;
// }


/***************************************************************************/
/*   END     BLUETOOTH CODE          */
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


/***************************************************************************/
/* START Varius FSM and Timing Variables

/* 
/***************************************************************************/
unsigned int runs = 0; //number of times FSM started
    
unsigned long milliSeconds = 0; //used to hold the current number of milliseconds
unsigned long popDetectedAt = 0;// used to hold the number of milliseconds when the most recent pop was detected
unsigned long threeMinutes = 180000;//the number of milliseconds in three minutes
unsigned long fifteenSeconds = 15000;
unsigned long halfSecond = 500;
unsigned int popCt = 0;


/***************************************************************************/
/*  END Varius FSM and Timing Variables
/* 
/***************************************************************************/


/***************************************************************************/
/*   START     FAKE_POP CODE          */
/***************************************************************************/
int detectPin = 2;//pins 2/PD0 and 3/PD1 

int buttonDebounce = 20;
int previousState = HIGH;
unsigned int previousPress;
volatile int buttonFlag;
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
void startRoutine(){
  //reset variables
  popCt = 0;
  popDetectedAt = millis();
  showWorking();
  runs = runs + 1;
}

void popDetectedRoutine(){
  //update count
  popCt = popCt + 1;
  //update display for user
  showValue(popCt);
  showValue2(popDetectedAt);
  //update phone for user
//  BLE_send(POP);
}
void doneRoutine(){
  //First update Display as it is the primary use of communication
  showDone();
  //Second update phone by bluetooth
//  BLE_send(DONE);
  //Lastly start playing buzzer tones
  tune();
  
  
  startRoutine();
}
void setup() {
  //Initialize the Screen First
  //Screen is first incase we need to display errors in other initilizing steps
  displaySetUp();
  showString("Display Set Up Done");
  delay(5000);
  
  //Initialize PopDetection Second
  //pop detection is second becuase it is the most important part
  detectSetUp();
  showString("Detect Set Up Done");
  delay(5000);
  
  //Initialize BlueTooth Third
  //bluetooth initilization should utilize the display
  //No matter what after X attempts to connect move past because 
  //popcorn should be starting to pop
//  bleSetup();
  showString("BLE Set Up Done");
  delay(5000);
  
  //Initialize Buzzer Last
  //buzzer is used as a back up communicaiton over distance
  //tones should be varying and cover a wide part of the frequency spectrum
 
  startRoutine();
  popDetectedAt = millis();
}


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
  if(milliSeconds > (threeMinutes * runs)){
    doneRoutine();
  }
  if(milliSeconds > fifteenSeconds + popDetectedAt ){
    doneRoutine();
  }
  
}
