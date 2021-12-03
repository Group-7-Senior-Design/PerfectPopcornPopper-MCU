
#include <Wire.h>
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
unsigned int popCt = 0;

/***************************************************************************/
/*  END Varius FSM and Timing Variables
/***************************************************************************/
void(* resetFunc)(void) = 0;

void setup(){
 

    // the adc prescale set to 32 allows for 28microsecond adc call
    // s2 ,s1 ,s0
    // 1  ,0  ,1 => 32 prescalar
    // 1  ,0  ,0 => 16 prescalar
    sbi(ADCSRA,ADEN);
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    sbi(ADCSRA,ADPS0) ;

    
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
    }


    //TIMER CHECKS
    milliSeconds = millis();

    //IF THREE MINUTES END
    if(milliSeconds > (startTime + (threeMinutes * runs))){
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

    //IF fifteen seconds since last pop END and popcorn has at least been cooking for 1 min
    if(milliSeconds > ( startTime + (oneMinute * runs))){
        if(milliSeconds > ( fifteenSeconds + popDetectedAt ){
            total = 0;
            initCt = 0;
            popCt = 0;
            //INCREMENT VARIABLES
            runs ++;
            startTime = millis();
            //RESTART
            loop();
        }
    }
    
}
