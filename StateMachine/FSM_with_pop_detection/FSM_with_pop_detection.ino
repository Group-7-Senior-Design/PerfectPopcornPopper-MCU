



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
/*   START     DISPLAY CODE          */
/***************************************************************************/
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
//declare and initialize library
Adafruit_SSD1306 display(128, 64, &Wire, 0);
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

 //including basic arduino headers
 #include <Arduino.h>
 //necessary Adafruit BLE headers
 #include <SPI.h>
 #include "Adafruit_BluefruitLE_SPI.h"
 //#include "BLEConfig.h"

//Bluetooth setup vars
//this is the size of the read buffer for incoming data
#define BUFSIZE             128 
//this setting enables debug output
#define VERBOSE_MODE        true 
//clear all GATT services/chars if set to 1
#define GATT_CLEAR_ALL    0 
//setting this flag to 1 will halt initialization until an end device connects
#define WAIT_FOR_CONN   0 

//SPI Pin settings tailored to ATMega328p/pb
//Arbitrarily selected pins- can be any GPIO 
//5 //WORKS //0 //PD0 - 30
#define BLUEFRUIT_SPI_CS  8
//6 //WORKS //1 //PD1 - 31
#define BLUEFRUIT_SPI_IRQ 7
//7 //WORKS //7 //optional, set as -1 if unused -> PD7 - 11
#define BLUEFRUIT_SPI_RST 4

//hardware specific to the 328p SMD - MUST BE THESE 
//0//13 //PB5 - 17
#define BLUEFRUIT_SPI_SCK    13
//1//12 //PD5 - 16
#define BLUEFRUIT_SPI_MISO   12
//2//11 //PD6 - 15
#define BLUEFRUIT_SPI_MOSI   11

#define FACTORYRESET_ENABLE 0

 // hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
 Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
 //service/char IDs
 int32_t serviceID;
 int32_t charID;
 //for ease of use with method calls
 int POP = 1;
 int THIRTY_SEC = 2;
 int DONE = 3;


 /******************TESTING VARIABLES BEGIN ******************/
 //BUTTONS FOR TESTING
 int buttonOne = A0 ;

 //LED TESTING ON BREADBOARD
 int good = 6;//8; //8 on 328p
 int error = 9; //9 on 328p
 //int receivedLED = 26;
 /******************TESTING VARIABLES END ******************/


 //Setup the Hardware and BLE stuff
 void bleSetup() 
 {
   //setting up LEDs for testing
 //  pinMode(error, OUTPUT);
 //  pinMode(good, OUTPUT);
 //  
   bool init = BLE_initialize();

 //  if(!init)
 //  {
 //    digitalWrite(error, HIGH);
 //    while(1);
 //  }
 //  digitalWrite(good, HIGH);
 //    
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
       showConnecting();
     }
   } 
    showConnected();
    //setup completed - wait a second
    delay(1000);
    return true;
 }


 void sendDataApp(String dataOut, int32_t characteristic)
 {
   //prepare the buffer and instruction for sending data
   ble.print( F("AT+GATTCHAR=") );
   ble.print( characteristic );
   ble.print( F(",") );
   ble.println(dataOut);

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
/*   START     POP DETECTION CODE          */
/***************************************************************************/
/** Pop dedection version 1
 * 
 * authors: Zachary Turner, 
 */

#include <arduinoFFT.h>
 
#define SAMPLES 64  //SAMPLES-pt FFT. Must be a base 2 number.
#define SAMPLING_FREQUENCY 20000 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
#define LOW_CUTOFF_FREQUENCY SAMPLES/32
#define HIGH_CUTOFF_FREQUENCY SAMPLES/4


/** Define for a faster ADC
 * cbi clears registor 
 */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

/** Define for a faster ADC
 * sbi sets registor 
 */
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long long microSeconds;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

/* Initialize running average for the region being analyzed to make a
 * threshold to compare current frame against. My use of y and Y is distinct
 * in var names -- y is time domain and Y is frequency domain
 */
const int YAvgLength = HIGH_CUTOFF_FREQUENCY - LOW_CUTOFF_FREQUENCY + 1;
double Yavg[YAvgLength];


/* Tunable value used to evaluate if a pop is occuring by observing if this
 * percent of frequency bins exceed the running average
 */
double popThreshold = .60;

/* Tunable value used to evaluate if the amplitude increase is significant
 * enough to be considered a pop.
 */
int amplitudeThreshold = 200;


/* Variable k is used for Y average
 * k will be the current number of samples
 * in correlation to the matlab code this is i
 */
int k = 1;

/*Pop tracking variables. popCount is the total pops, popFound goes to 1 if
 *the algorithm decides that a pop is occuring in the current frame,
 *popLatch increases if a pop was occuring in the last frame. 
 */
unsigned short int popCount = 0;  
bool popFound = 0;
bool prevPopFound = 0;
int8_t popLatch = 0;

void detectSetUp() 
{
    // Fill Yavg with zero values
    for(int i = 0; i < YAvgLength; i++){
        Yavg[i] = 0;
    }

    //Period in microseconds     
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); 

    // the adc prescale set to 32 allows for 28microsecond adc call
    // s2 ,s1 ,s0
    // 1  ,0  ,1 => 32 prescalar
    // 1  ,0  ,0 => 16 prescalar
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    sbi(ADCSRA,ADPS0) ;
}


void detectLoop(){  
   
    /*Sample SAMPLES times*/
    for(int i=0; i<SAMPLES; i++)
    {
        //Returns the number of microseconds since the Arduino board began running the current script.
        microSeconds = micros();     
     
        vReal[i] = analogRead(A0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always

        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
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
    for(int j = LOW_CUTOFF_FREQUENCY; j < HIGH_CUTOFF_FREQUENCY; j ++)
    {
        //used to incrementing Yavg array
        int j2 = j - LOW_CUTOFF_FREQUENCY;

        // Find the max value for each sample set
        if(maxY < vReal[j])
        {
            maxY = vReal[j];
        }

        // add to bin count
        if( Yavg[j2] < vReal[j])
        {
            freqBinCT = freqBinCT + 1;
        }

        // add to moving average on bin at a time
        Yavg[j2] = (Yavg[j2] * (k -1) / k ) + ( vReal[j] / k );
    }

    
    // Current frame measurements complete. Pop analysis logic:
    
    popFound = 0;   //Reset popFound every frame

    /**
     * note if pop detected in frame based on whether a wide-band volume
     * increase is observed, and whether the amplitude was significantly
     * increased (significance set by the tunable threshold)
     */
    if (freqBinCT > popThreshold * YAvgLength){
        if (maxY > amplitudeThreshold){
            popFound = 1;  
        }
    }

    /*Latching algorithm used to avoid counting the same pop twice. If the
    latch has not been activated, popFound is checked to set the latch if
    a pop has been found. If a pop was found in the last frame (latch > 0)
    the program currently waits for it to no longer be detected before
    counting the pop. This gets an inaccurate count for clusters of pops,
    but does a great job of not counting the same pop twice. 
    
    Will need modification to detect overlapping pops -- lastmaxval could
    possibly be implemented to see if the fading high frequency amplitudes
    go back up. 
    Could also be modified to reject counting spikes that occur in a
    single frame by only counting pops if the latch goes over 1. 
    */
//    if (popLatch == 0){        //if pop was not already detected don't add it yet
//        if (popFound == 1){
//            popLatch = popLatch + 1;
//        }
//    }else{
//        if (popFound == 1){    //if pop was already detected
//            popLatch = popLatch + 1;   // note how long it's been going (unused currently), might be worth noting
//        }else { 
//            popLatch = 0;  // if pop is no longer detected add it to the count and reset latch
//            popCount = popCount + 1;
//            Serial.println("pop");
//        }
//    }


    //prevPopFound = popFound
    Serial.println(maxY);     //Print out the most dominant frequency.
     k = k +1;
     
    /*Script stops here. Hardware reset required.*/
   
}


/***************************************************************************/
/*   END     POP DETECTION CODE          */
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
  BLE_send(POP);
}
void doneRoutine(){
  //First update Display as it is the primary use of communication
  showDone();
  //Second update phone by bluetooth
  BLE_send(DONE);
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
  bleSetup();
  showString("BLE Set Up Done");
  delay(5000);
  
  //Initialize Buzzer Last
  //buzzer is used as a back up communicaiton over distance
  //tones should be varying and cover a wide part of the frequency spectrum
 
  startRoutine();
  popDetectedAt = millis();
}


void loop() {
  
  detectLoop();
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
