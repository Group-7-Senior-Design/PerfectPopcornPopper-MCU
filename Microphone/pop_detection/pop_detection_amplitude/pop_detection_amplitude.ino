

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
#define ANALOGPIN A2
const int SAMPLES = 2;
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
unsigned long fifteenSeconds = 15000;
unsigned long startTime = 0;
unsigned long popFoundAt = 0;
int state = LOW;
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

    pinMode(7, OUTPUT);
    digitalWrite(7, state);
    startTime = millis();
    Serial.begin(115200); 
}


void loop(){
    
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
    if(state == HIGH && (millis() > popFoundAt + 10)){
      state = LOW;
      digitalWrite(7, state);
    }
    if(currAverage > thresh){
        state = HIGH;
        digitalWrite(7, state);
        popFoundAt = millis();
    }
    if(state == HIGH){
      Serial.print("POP FOUND");
    }
    Serial.print("avg: ");
    Serial.println(currAverage);
    Serial.print("thresh: ");
    Serial.println(thresh);

    
}
