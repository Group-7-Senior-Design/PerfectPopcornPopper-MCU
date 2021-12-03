#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
//necessary Adafruit BLE headers
#include "Adafruit_BluefruitLE_SPI.h"
#include <Adafruit_SSD1306.h>
#include "config.h"
//including basic arduino headers






//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_SSD1306 dis = Adafruit_SSD1306(128, 64, &Wire, 0);
// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST 
////service/char IDs
//int32_t serviceID;
//int32_t charID;
int buzzer = 5;

/***************************************************************************/
/* START Detect Variables
/***************************************************************************/
unsigned long long samplingPeriod;
unsigned long long microSeconds;
 
double amp [SAMPLES] = {}; 
int initCt = 0;
float initAverage = 0;
float thresh = 0;
float increase = 10;
unsigned long total = 0;
float currAverage = 0;
unsigned int sum = 0;
/***************************************************************************/
/* END Detect Variables
/***************************************************************************/


/***************************************************************************/
/* START Varius FSM and Timing Variables
/***************************************************************************/
unsigned int runs = 1; //number of times FSM started
unsigned long startTime = 0;
unsigned long milliSeconds = 0; //used to hold the current number of milliseconds
unsigned long popDetectedAt = 0;// used to hold the number of milliseconds when the most recent pop was detected
unsigned long threeMinutes = 180000;//the number of milliseconds in three minutes
unsigned long fifteenSeconds = 15000;
unsigned long halfSecond = 500;
unsigned int popCt = 0;

/***************************************************************************/
/*  END Varius FSM and Timing Variables
/***************************************************************************/
void(* resetFunc)(void) = 0;

void setup(){
    //begin display usage
    dis.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    dis.clearDisplay();
    dis.setTextSize(2);         
    dis.setTextColor(SSD1306_WHITE); 

    //display successful display initialization
    dis.clearDisplay();
    milliSeconds = millis();
    while(micros() < (milliSeconds + 2000)){
     //do nothing 
    }
    dis.setCursor(0,2);   
    dis.println(F("Display"));
    dis.println(F("Initialized!!"));
    dis.display();
    //mark the time of display for timing
    milliSeconds = millis();
   

    // the adc prescale set to 32 allows for 28microsecond adc call
    // s2 ,s1 ,s0
    // 1  ,0  ,1 => 32 prescalar
    // 1  ,0  ,0 => 16 prescalar
    sbi(ADCSRA,ADEN);
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    sbi(ADCSRA,ADPS0) ;

    //wait at least half second to display next message
    while(millis() < (milliSeconds + 2000)){
     //do nothing 
    }
    dis.clearDisplay();   
    dis.setCursor(0,2);   
    dis.println(F("ADC/Detect"));
    dis.println(F("Initialized"));
    dis.display();
    milliSeconds = millis();

     //wait to display next message
    while(millis() < (milliSeconds + 2000)){
     //do nothing 
    }
//
//    //check if connected to the BLE module - and pass in the debug variable
//    if(!ble.begin(VERBOSE_MODE)){
//       //if cannot connect to ble module, no point in continuing as it will not work
//       //this is typically due to a mistake in hardware such as wiring
//        dis.clearDisplay();   
//        dis.setCursor(0,2);   
//        dis.println(F("BLE ERR"));
//        dis.println(F("CHECK WIRES"));
//        dis.display();
//        milliSeconds = millis();
//
//         //wait to display next message
//        while(millis() < (milliSeconds + 2000)){
//         //do nothing 
//        }
//    }
//  
//    //factory reset to kill any previous data stashed within the device 
//    if(FACTORYRESET_ENABLE){      
//      //if cannot factory reset - issue msg to screen and halt execution with empty while loop 
//      if(!ble.factoryReset()){
//        dis.clearDisplay();   
//        dis.setCursor(0,2);   
//        dis.println(F("BLE ERR"));
//        dis.println(F("FACT RES"));
//        dis.display();
//        milliSeconds = millis();
//
//         //wait to display next message
//        while(millis() < (milliSeconds + 2000)){
//         //do nothing 
//        }
//      }
//    }
//  
//    //change the name of the BLE device    
//    if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Perfect Popcorn Popper")) ) {
//      
//    }
//  
//    //estabalish the GATT Service and GATT characteristics
//    boolean success;
//  
//    //if flag set to true, first clear all pre-existing gatt services and characteristics
//    if(GATT_CLEAR_ALL){
//      success = ble.sendCommandCheckOK(F("AT+GATTCLEAR"));
//      if(!success){
//        //could not clear old GATT services
//        dis.clearDisplay();   
//        dis.setCursor(0,2);   
//        dis.println(F("BLE ERR"));
//        dis.println(F("GATT SERV"));
//        dis.display();
//        milliSeconds = millis();
//
//         //wait to display next message
//        while(millis() < (milliSeconds + 2000)){
//         //do nothing 
//        }
//      }
//    }
//    //establish service
//    success = ble.sendCommandWithIntReply(F("AT+GATTADDSERVICE=UUID128=00-11-00-11-44-55-66-77-88-99-AA-BB-CC-DD-EE-FF"), &serviceID);
//    if(!success){
//      //could not establish main service, no comms may be completed
//      dis.clearDisplay();   
//      dis.setCursor(0,2);   
//      dis.println(F("BLE ERR"));
//      dis.println(F("MAIN SERV"));
//      dis.display();
//        milliSeconds = millis();
//
//         //wait to display next message
//        while(millis() < (milliSeconds + 2000)){
//         //do nothing 
//        }
//    }
//    //establish characteristic that is attached to the service just established
//    success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x0002, PROPERTIES=0x12, MIN_LEN=1, MAX_LEN=20, VALUE=5"), &charID);
//    if(!success){
//      //could not establish main characteristic under the service, no comms may be completed
//        dis.clearDisplay();   
//        dis.setCursor(0,2);   
//        dis.println(F("BLE ERR"));
//        dis.println(F("MAIN CHAR"));
//        dis.display();
//    }
//    //issue ble reset to solidify changes made to the GATT services and characteristics
//    ble.reset();
//  
//    if(WAIT_FOR_CONN){
//      //wait for an incoming connection  
//      while(!ble.isConnected()){
//        //wait for a connection
//        dis.clearDisplay();   
//        dis.setCursor(0,2);   
//        dis.println(F("BLE"));
//        dis.println(F("Connecting"));
//        dis.display();
//        milliSeconds = millis();
//
//         //wait to display next message
//        while(millis() < (milliSeconds + 2000)){
//         //do nothing 
//        }
//      }
//    }
//    

    dis.clearDisplay();   
    dis.setCursor(0,2);   
    dis.println(F("BLE"));
    dis.println(F("Initialized"));
    dis.display();
    milliSeconds = millis();
     //wait to display next message
    while(millis() < (milliSeconds + 2000)){
     //do nothing 
    }




    
    startTime = millis();
}


void loop(){

    //DEBUG VIEW
    dis.clearDisplay();   
    dis.setCursor(0,2);   
    dis.print(F("POP:"));
    dis.println(popCt);
    dis.print(F("AMP:"));
    dis.println((int)currAverage);
    dis.print(F("THRESH:"));
    dis.println((int)thresh);
    dis.display();
    //end DEBUG VIEW
    
    sum = 0;
    for(int i=0; i<SAMPLES; i++){
        amp[i] = analogRead(ANALOGPIN); //Reads the value from analog pin 0 (A0), quantize it and save it
        sum += amp[i];
    }
    if( millis() < (startTime + fifteenSeconds) ){
        total += (float)sum / SAMPLES;
        initCt ++;
    }else{
        currAverage = (float)sum/ SAMPLES;
    }

    //compute average
    initAverage = (float) total/initCt;
    if ( initAverage < 0){
      initAverage *= -1;
    }
    //compute threshold
    thresh = initAverage + increase;

    if(currAverage > (thresh)){
        popCt = popCt + 1;
         //wait to display next message
        popDetectedAt = millis();
        dis.clearDisplay();   
        dis.setCursor(0,2);   
        dis.print(F("POP"));
        dis.println(popCt);
        dis.display();
//          //update phone for user
//        ble.print( F("AT+GATTCHAR=") );
//        ble.print( charID );
//        ble.print( F(",") );
//        ble.println("AAAA");
    }


    //TIMER CHECKS
    milliSeconds = millis();

    //IF THREE MINUTES END
    if(milliSeconds > (startTime + (threeMinutes * runs))){
              //First update Display as it is the primary use of communication
        dis.clearDisplay();   
        dis.setCursor(0,2);   
        dis.println(F("Done!"));
        dis.display();
        
        //Second update phone by bluetooth
        //prepare the buffer and instruction for sending data
    //    ble.print( F("AT+GATTCHAR=") );
    //    ble.print( charID );
    //    ble.print( F(",") );
    //    ble.println("CCCC");
      
        //PLAY BUZZER
        tone(buzzer, 1047);
        milliSeconds = millis();
        while(millis() < (halfSecond)+ milliSeconds){}
        noTone(buzzer);
        tone(buzzer, 1047*2);
        milliSeconds = millis();
        while(millis() < (halfSecond)+ milliSeconds){}
        noTone(buzzer);
        tone(buzzer, 1047);
        milliSeconds = millis();
        while(millis() < (halfSecond)+ milliSeconds){}
        noTone(buzzer);
        tone(buzzer, 1047*2);
        milliSeconds = millis();
        while(millis() < (halfSecond)+ milliSeconds){}
        noTone(buzzer);
    
        //RESET VARIABLES 
        
        total = 0;
        initCt = 0;
        popCt = 0;
        //INCREMENT VARIABLES
        runs ++;
        startTime = millis();
        //RESTART
        loop();
    }

    //IF fifteen seconds since last pop END
    if(milliSeconds > fifteenSeconds + popDetectedAt ){
//      doneRoutine();
    }
    
}
