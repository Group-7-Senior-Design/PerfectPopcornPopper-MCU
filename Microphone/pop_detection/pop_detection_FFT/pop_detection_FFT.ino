/** Pop dedection version 1
 * 
 * authors: Zachary Turner, 
 */

#include <arduinoFFT.h>
#include "pinDefine.h"
#define SAMPLES 128  //SAMPLES-pt FFT. Must be a base 2 number.
#define SAMPLING_FREQUENCY 20000 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
#define LOW_CUTOFF_FREQUENCY 0 //SAMPLES/32
#define HIGH_CUTOFF_FREQUENCY 32//normally 32 SAMPLES/4


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
unsigned int ampavgSamplingPeriod;
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
double popThreshold = .65;

/* Tunable value used to evaluate if the amplitude increase is significant
 * enough to be considered a pop.
 */
int amplitudeThreshold = 1200;


/* Variable k is used for Y average
 * k will be the current number of samples
 * in correlation to the matlab code this is i
 */
double k = 1;

/*Pop tracking variables. popCount is the total pops, popFound goes to 1 if
 *the algorithm decides that a pop is occuring in the current frame,
 *popLatch increases if a pop was occuring in the last frame. 
 */
unsigned long popCount = 0;  
bool popFound = 0;
bool prevPopFound = 0;
int8_t popLatch = 0;
unsigned long maxYTot = 0;
unsigned long long ampavg = 0;
unsigned long popDetectedAt = 0;
unsigned long long popDetectedAtLatch = 0;
unsigned long startTime = 0;
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
    /*
Prescale  ADPS2,1,0  Clock (MHz)  Sampling rate (KHz)
  2       0 0 1          8            615
  4       0 1 0          4            307
  8       0 1 1          2            153
  16      1 0 0          1            76.8
  32      1 0 1         0.5           38.4 <
  64      1 1 0         0.25          19.2
  128     1 1 1         0.125         9.6 (default)
*/
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
    
    if(SERIAL_CONNECTED == 1){
      Serial.begin(115200); //Baud rate for the Serial Monitor
    }
    //digitalWrite(SIGNALPIN, LOW);
    startTime = 0;
}


void loop() 

{  
//  
   if(micros() > (popDetectedAtLatch + 100)){
    
      /*Sample SAMPLES times*/
      for(int i=0; i<SAMPLES; i++)
      {
          //Returns the number of microseconds since the Arduino board began running the current script.
          microSeconds = micros();     
       
          vReal[i] = analogRead(ANALOGPIN); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
          vImag[i] = 0; //Makes imaginary term 0 always
  
          /*remaining wait time between samples if necessary*/
          while(micros() < (microSeconds + 25))
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
     
  
          // add to bin count
          if( Yavg[j2] < vReal[j])
          {
              freqBinCT = freqBinCT + 1;
              if(maxY < vReal[j]){
                 maxY = vReal[j];
                 
              }
          }
          
          // add to moving average on bin at a time
          Yavg[j2] = (Yavg[j2] * (k -1) / k ) + ( vReal[j] / k );
      }
      /**
       * note if pop detected in frame based on whether a wide-band volume
       * increase is observed
       * 
       * , and whether the amplitude was significantly
       * increased (significance set by the tunable threshold)
       */
      long binThresh = 23;popThreshold * YAvgLength;
      if (freqBinCT > binThresh){
          if (maxY > amplitudeThreshold){
              popDetectedAtLatch = micros();
              popCount ++;
              if(SERIAL_CONNECTED == 1){
                  Serial.print("------------------POP START # ");
                  Serial.print(popCount);
                  Serial.println("------------------");
              
                  Serial.print("maxY: ");
                  Serial.print((long)maxY);
                  Serial.print("\tthresh: ");
                  Serial.println(amplitudeThreshold);
                  Serial.print("freqBinCt: ");
                  Serial.print(freqBinCT);
                  Serial.print("\t\tthresh*avg: ");
                  Serial.println(binThresh);
                  Serial.print("------------------POP END # ");
                  Serial.print(popCount);
                  Serial.println("------------------");
                
              }
                   
          }
      }
  
  
      //prevPopFound = popFound
  
       if(SERIAL_CONNECTED == 1){
           //Serial.println(maxY); 
       }
        
       k = k +1;
    }
    /*Script stops here. Hardware reset required.*/
   
}
