/** Pop dedection version 1
 * 
 * authors: Zachary Turner, 
 */

#include <arduinoFFT.h>
#include "pinDefine.h"
#define SAMPLES 128  //SAMPLES-pt FFT. Must be a base 2 number.
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

void setup() 
{
    pinMode(SIGNALPIN,OUTPUT);
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
    
    if(SERIAL_CONNECTED == 1){
      Serial.begin(115200); //Baud rate for the Serial Monitor
    }
    digitalWrite(SIGNALPIN, LOW);
}


void loop() 

{  
//  
   
    /*Sample SAMPLES times*/
    for(int i=0; i<SAMPLES; i++)
    {
        //Returns the number of microseconds since the Arduino board began running the current script.
        microSeconds = micros();     
     
        vReal[i] = analogRead(ANALOGPIN); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
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
              // turn the LED on (HIGH is the voltage level)
            digitalWrite(SIGNALPIN, HIGH); 
            delay(50);
            digitalWrite(SIGNALPIN, LOW);   // turn the LED on (HIGH is the voltage level)
            if(SERIAL_CONNECTED == 1){
                Serial.print(maxY);
                Serial.println("  popFound");
            }
                 
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

     if(SERIAL_CONNECTED == 1){
         Serial.println(maxY); 
     }
      
     k = k +1;
     
    /*Script stops here. Hardware reset required.*/
   
}
