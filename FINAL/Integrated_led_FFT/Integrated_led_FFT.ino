
//pin numbers 23 24
//including basic arduino headers
#include <Arduino.h>
#include <arduinoFFT.h>
//necessary Adafruit BLE headers
#include "Adafruit_BluefruitLE_SPI.h"
#include "config.h"


/***************************************************************************/
/*  START DETECT VARIABLES
/***************************************************************************/

arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod = 0;
unsigned long long microSeconds = 0;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

/* Initialize running average for the region being analyzed to make a
 * Threshhold to compare current frame against. My use of y and Y is distinct
 * in var names -- y is time domain and Y is frequency domain
 */
const int YAvgLength = HIGH_CUTOFF_FREQUENCY - LOW_CUTOFF_FREQUENCY + 1;
double Yavg[YAvgLength];


/* Tunable value used to evaluate if a pop is occuring by observing if this
 * percent of frequency bins exceed the running average
 */
double popThreshhold = 0.69;

/* Tunable value used to evaluate if the amplitude increase is significant
 * enough to be considered a pop.
 */
int amplitudeThreshhold = 7500;//400;


/* Variable k is used for Y average
 * k will be the current number of samples
 * in correlation to the matlab code this is i
 */
unsigned long long k = 1;

/*Pop tracking variables. popCount is the total pops, popFound goes to 1 if
 *the algorithm decides that a pop is occuring in the current frame,
 *popLatch increases if a pop was occuring in the last frame. 
 */
unsigned short int popCount = 0;  
bool popFound = 0;
bool prevPopFound = 0;
int8_t popLatch = 0;


//TUNABLES
unsigned long long latchTime = 100;
double popThreshStart = 1.10;
int ampThreshStart = 20000;
double popThreshEnd = .62;
double popThreshMid = .62;
int ampThreshMid = 20;
int ampThreshEnd = 20;
unsigned long long fs = 40000;
unsigned long long ts = 1000000/fs;


/***************************************************************************/
/*  END DETECT VARIABLES
/* 
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
unsigned long maxTime = 210000;//the number of milliseconds in 3:30
unsigned long minTime = 155000;//this 2 min 35
unsigned long fifteenSeconds = 15000;
unsigned long halfSecond = 500;
unsigned long timeAfterPopDone = 4000;
unsigned long timeAfterPopAlmost = 700;

unsigned long long popDetectedAtLatch = 0;


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
  delay(750);
  //Initialize the Screen First
  //Screen is first incase we need to display errors in other initilizing steps
  displaySetUp();
  delay(100);
  
  //Initialize PopDetection Second
  //pop detection is second becuase it is the most important part
  detectSetUp();
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

  detectLoop();


  //turn off leds if they're on after a bit of time
  //this is used instead of delay() because we don't want the algorithm to stop pulling values
  milliSeconds = millis();
  temp = popLedOnAt + popLedDuration;
  if( milliSeconds > temp ){
    clearPop();
  }
  
  //Loop over pop detection
  
  milliSeconds = millis();
  if(milliSeconds > (startTime+maxTime)){
    doneRoutine();
  }
  else if(milliSeconds > (startTime+minTime)){
      if(milliSeconds > (timeAfterPopDone + popDetectedAt) ){
          doneRoutine();
      }
      else if(milliSeconds > (timeAfterPopAlmost + popDetectedAt)){
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
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("CCCC");
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("CCCC");
  
    
    tune();
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("CCCC");
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("CCCC");
    //prepare the buffer and instruction for sending data
    ble.print( F("AT+GATTCHAR=") );
    ble.print( charID );
    ble.print( F(",") );
    ble.println("CCCC");
    while( millis() > milliSeconds + fifteenSeconds){
      
    }
  
  startRoutine();
}
//END ROUTINE ***************************************************

/***************************************************************************/
/*   START   POP CODE          */
/***************************************************************************/

void detectSetUp(){
    // Fill Yavg with zero values
    for(int i = 0; i < YAvgLength; i++){
        Yavg[i] = 0;
    }

    // the adc prescale set to 32 allows for 28microsecond adc call
    // s2 ,s1 ,s0
    // 1  ,0  ,1 => 32 prescalar
    // 1  ,0  ,0 => 16 prescalar
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
    
}


void detectLoop(){
 
  if(micros() > (popDetectedAtLatch + latchTime)){
    
      /*Sample SAMPLES times*/
      for(int i=0; i<SAMPLES; i++)
      {
          //Returns the number of microseconds since the Arduino board began running the current script.
          microSeconds = micros();     
       
          vReal[i] = analogRead(DETECTPIN); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
          vImag[i] = 0; //Makes imaginary term 0 always
  
          /*remaining wait time between samples if necessary*/
          while(micros() < (microSeconds + ts))
          {
            //do nothing
          }
      }
      
      /*Perform FFT on samples*/
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  
      //vReal now holds all the magnitudes
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
      
      int freqBinCT = 0;//track number of high frequency bins
      int maxY = 0;//track biggiest peak
      for(int j = LOW_CUTOFF_FREQUENCY; j < HIGH_CUTOFF_FREQUENCY; j ++){
          //used to incrementing Yavg array
          int j2 = j - LOW_CUTOFF_FREQUENCY;
  
          // Find the max value for each sample set
          // add to bin count
          if( Yavg[j2] < vReal[j]){
              freqBinCT = freqBinCT + 1;
              if(maxY < vReal[j]){
                 maxY = vReal[j];
              }
          }
          
          // add to moving average on bin at a time
          Yavg[j2] = (Yavg[j2] * (k -1) / k ) + ( vReal[j] / k );
      }

      if(millis() < startTime +  50000){
          popThreshhold = popThreshStart;//1.0;
          amplitudeThreshhold = ampThreshStart;//13000;
      }else if(millis() > startTime + 135000 ){
          popThreshhold = popThreshEnd;//.73;
          amplitudeThreshhold = ampThreshEnd;
          latchTime = 500;
      }else{
          popThreshhold = popThreshMid;//.65;
          amplitudeThreshhold = ampThreshMid;//7000;
      }
      
      /**
       * note if pop detected in frame based on whether a wide-band volume
       * increase is observed
       * 
       * , and whether the amplitude was significantly
       * increased (significance set by the tunable Threshhold)
       */
      long binThresh = popThreshhold * YAvgLength;
      if (freqBinCT > binThresh){
          if (maxY > amplitudeThreshhold){
              popDetectedAtLatch = micros();
             
              popCt ++;
              popDetectedRoutine();
             
                   
          }
      }
       k = k +1;
    }
}
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
