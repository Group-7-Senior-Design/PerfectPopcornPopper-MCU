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
unsigned long fifteenSeconds = 15000;
unsigned long halfSecond = 500;
unsigned int popCt = 0;


/***************************************************************************/
/*  END Varius FSM and Timing Variables
/* 
/***************************************************************************/
// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
//service/char IDs
int32_t serviceID;
int32_t charID;

//declare and initialize library
Adafruit_SSD1306 display(128, 64, &Wire, 0);

int detectPin = 2;//pins 2/PD0 and 3/PD1 

int buttonDebounce = 20;
int previousState = HIGH;
unsigned int previousPress;
volatile int buttonFlag;



/***************************************************************************/
/*  START detect Variables
/***************************************************************************/

//
//#include <arduinoFFT.h>
//#define SAMPLES 64  //SAMPLES-pt FFT. Must be a base 2 number.
//#define SAMPLING_FREQUENCY 20000 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
//#define LOW_CUTOFF_FREQUENCY SAMPLES/32
//#define HIGH_CUTOFF_FREQUENCY SAMPLES/4
//
//
///** Define for a faster ADC
// * cbi clears registor 
// */
//#ifndef cbi
//#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
//#endif
//
///** Define for a faster ADC
// * sbi sets registor 
// */
//#ifndef sbi
//#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//#endif
//
//
//arduinoFFT FFT = arduinoFFT();
// 
//unsigned int samplingPeriod;
//unsigned long long microSeconds;
// 
//double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
//double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
//
///* Initialize running average for the region being analyzed to make a
// * threshold to compare current frame against. My use of y and Y is distinct
// * in var names -- y is time domain and Y is frequency domain
// */
//const int YAvgLength = HIGH_CUTOFF_FREQUENCY - LOW_CUTOFF_FREQUENCY + 1;
//double Yavg[YAvgLength];
//
//
///* Tunable value used to evaluate if a pop is occuring by observing if this
// * percent of frequency bins exceed the running average
// */
//double popThreshold = .60;
//
///* Tunable value used to evaluate if the amplitude increase is significant
// * enough to be considered a pop.
// */
//int amplitudeThreshold = 200;
//
//
///* Variable k is used for Y average
// * k will be the current number of samples
// * in correlation to the matlab code this is i
// */
//int k = 1;
//
///*Pop tracking variables. popCount is the total pops, popFound goes to 1 if
// *the algorithm decides that a pop is occuring in the current frame,
// *popLatch increases if a pop was occuring in the last frame. 
// */
//unsigned short int popCount = 0;  
//bool popFound = 0;
//bool prevPopFound = 0;
//int8_t popLatch = 0;


/***************************************************************************/
/*  END detect Variables
/***************************************************************************/




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
//  detectSetUp();
pinMode(detectPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(detectPin), detectLoop, CHANGE);
  showString("Detect Set Up Done");
  delay(100);
  
  //Initialize BlueTooth Third
  //bluetooth initilization should utilize the display
  //No matter what after X attempts to connect move past because 
  //popcorn should be starting to pop
  bleSetup();
  showString("BLE Set Up Done");
  delay(1000);
  
  startRoutine();
  popDetectedAt = millis();
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
  if(milliSeconds > (threeMinutes * runs)){
    doneRoutine();
  }
  if(milliSeconds > fifteenSeconds + popDetectedAt ){
    doneRoutine();
  }
  
}
// LOOP END ***********************************************************************************

//BEGIN ROUTINE ***************************************************
void startRoutine(){
  //reset variables
  popCt = 0;
  popDetectedAt = millis();
  showWorking();
  runs = runs + 1;
}
//POP ROUTINE
void popDetectedRoutine(){
  //update count
  popCt = popCt + 1;
  //update display for user
  showValue(popCt);
  showValue2(popDetectedAt);
  //update phone for user
  ble.print( F("AT+GATTCHAR=") );
  ble.print( charID );
  ble.print( F(",") );
  ble.println("AAAA");
}
//FINISH ROUTINE
void doneRoutine(){
  //First update Display as it is the primary use of communication
  showDone();
  //Second update phone by bluetooth
  //prepare the buffer and instruction for sending data
  ble.print( F("AT+GATTCHAR=") );
  ble.print( charID );
  ble.print( F(",") );
  ble.println("CCCC");
  
  tune();
  
  startRoutine();
}
//END ROUTINE ***************************************************

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
void bleSetup() 
{  
  bool init = BLE_initialize();    
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
   showConnected();
   delay(1000);
   return true;
}
// END BLE CODE *******************************************************************


/***************************************************************************/
/*   START     DETECTION CODE          */
/***************************************************************************/
/** Pop dedection version 1
 * 
 * authors: Zachary Turner, 
// */
//void detectSetUp() 
//{
//    // Fill Yavg with zero values
//    for(int i = 0; i < YAvgLength; i++){
//        Yavg[i] = 0;
//    }
//
//    //Period in microseconds     
//    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); 
//
//    // the adc prescale set to 32 allows for 28microsecond adc call
//    // s2 ,s1 ,s0
//    // 1  ,0  ,1 => 32 prescalar
////    // 1  ,0  ,0 => 16 pr
//
//    
//}
//
//
void detectLoop() 

{  
   buttonFlag = 1;
}
//  
//   
//    /*Sample SAMPLES times*/
//    for(int i=0; i<SAMPLES; i++)
//    {
//        //Returns the number of microseconds since the Arduino board began running the current script.
//        microSeconds = micros();     
//     
//        vReal[i] = analogRead(A4); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
//        vImag[i] = 0; //Makes imaginary term 0 always
//
//        /*remaining wait time between samples if necessary*/
//        while(micros() < (microSeconds + samplingPeriod))
//        {
//          //do nothing
//        }
//    }
//    
//    /*Perform FFT on samples*/
//    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
//    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
//
//    //vReal now holds all the magnitudes
//    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
//    
//    int freqBinCT = 0;//track number of high frequency bins
//    int maxY = 0;//track biggiest peak
//    for(int j = LOW_CUTOFF_FREQUENCY; j < HIGH_CUTOFF_FREQUENCY; j ++)
//    {
//        //used to incrementing Yavg array
//        int j2 = j - LOW_CUTOFF_FREQUENCY;
//
//        // Find the max value for each sample set
//        if(maxY < vReal[j])
//        {
//            maxY = vReal[j];
//        }
//
//        // add to bin count
//        if( Yavg[j2] < vReal[j])
//        {
//            freqBinCT = freqBinCT + 1;
//        }
//
//        // add to moving average on bin at a time
//        Yavg[j2] = (Yavg[j2] * (k -1) / k ) + ( vReal[j] / k );
//    }
//
//    
//    // Current frame measurements complete. Pop analysis logic:
//    
//    popFound = 0;   //Reset popFound every frame
//
//    /**
//     * note if pop detected in frame based on whether a wide-band volume
//     * increase is observed, and whether the amplitude was significantly
//     * increased (significance set by the tunable threshold)
//     */
//    if (freqBinCT > popThreshold * YAvgLength){
//        if (maxY > amplitudeThreshold){
//          popDetectedRoutine();
//        }
//    }
//
//    /*Latching algorithm used to avoid counting the same pop twice. If the
//    latch has not been activated, popFound is checked to set the latch if
//    a pop has been found. If a pop was found in the last frame (latch > 0)
//    the program currently waits for it to no longer be detected before
//    counting the pop. This gets an inaccurate count for clusters of pops,
//    but does a great job of not counting the same pop twice. 
//    
//    Will need modification to detect overlapping pops -- lastmaxval could
//    possibly be implemented to see if the fading high frequency amplitudes
//    go back up. 
//    Could also be modified to reject counting spikes that occur in a
//    single frame by only counting pops if the latch goes over 1. 
//    */
////    if (popLatch == 0){        //if pop was not already detected don't add it yet
////        if (popFound == 1){
////            popLatch = popLatch + 1;
////        }
////    }else{
////        if (popFound == 1){    //if pop was already detected
////            popLatch = popLatch + 1;   // note how long it's been going (unused currently), might be worth noting
////        }else { 
////            popLatch = 0;  // if pop is no longer detected add it to the count and reset latch
////            popCount = popCount + 1;
////            Serial.println("pop");
////        }
////    }
//
//
//    //prevPopFound = popFound
//
//      
//     k = k +1;
//    
//   
//}

/***************************************************************************/
/*   END     DETECTION CODE          */
/***************************************************************************/
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
