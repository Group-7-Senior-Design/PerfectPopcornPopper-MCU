


#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 dis = Adafruit_SSD1306(128, 64, &Wire, 0);


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
/***************************************************************************/
/* START Detect Variables
/***************************************************************************/
const int SAMPLES = 2;
double amp [SAMPLES] = {}; 
int initCt = 0;
float initAverage = 0;
float thresh = 0;
float increase = 1;
unsigned long total = 0;
float currAverage = 0;
unsigned int sum = 0;
/***************************************************************************/
/* END Detect Variables
/***************************************************************************/


/***************************************************************************/
/* START Varius FSM and Timing Variables
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
/***************************************************************************/
void(* resetFunc)(void) = 0;

void setup(){
    //begin display usage
    dis.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    //clear buffer
    dis.clearDisplay();
    dis.setTextSize(2);  
    // Draw white text           
    dis.setTextColor(SSD1306_WHITE); 

    //display successful display initialization
    dis.clearDisplay();
    milliSeconds = millis();
    while(millis() < (milliSeconds + 3000)){
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
    while(millis() < (milliSeconds + 3000)){
     //do nothing 
    }
    dis.clearDisplay();   
    dis.setCursor(0,2);   
    dis.println(F("ADC/Detect"));
    dis.println(F("Initialized"));
    dis.display();
    milliSeconds = millis();

     //wait at least half second to display next message
    while(micros() < (milliSeconds + 3000)){
     //do nothing 
    }
}


void loop(){
    sum = 0;
    for(int i=0; i<SAMPLES; i++){   
     
        amp[i] = analogRead(A2); //Reads the value from analog pin 0 (A0), quantize it and save it
        sum += amp[i];
    }
 
    currAverage = (float)sum / SAMPLES;
    dis.clearDisplay();   
    dis.setCursor(0,2);   
    dis.println(F("Amp:"));
    dis.println(currAverage);
    dis.display();

}
