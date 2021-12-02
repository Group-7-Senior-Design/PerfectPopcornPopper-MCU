

#include <SdFat.h>
#include <SPI1.h>
#include <TMRpcm.h>

SdFat SD;

#define SD_ChipSelectPin 4  

TMRpcm audio;   // create an audio object for use in this sketch 

//set pin numbers
const int recButtonPin = 1;    
const int stopButtonPin = 7;
const int armedLedPin = A5;      
const int sdErrorLedPin = A4;

//BLE IRQ digital = 1
//BLE RST digital = 7 // reset is spaced out a bit on the header
//display data is A4// this is the outside one on the end
//display clk is A5
void setup() {
  
  pinMode(recButtonPin, INPUT);
  pinMode(stopButtonPin, INPUT);
  pinMode(armedLedPin, OUTPUT);
  pinMode(sdErrorLedPin, OUTPUT);
  
  // set initial LED state
  digitalWrite(armedLedPin, LOW);

  
  pinMode(12,OUTPUT);  
  
  //Serial.begin(115200);
  
  if (!SD.begin(SD_ChipSelectPin)) { 
    digitalWrite(sdErrorLedPin, HIGH); 
    //Serial.println("SD NOT OK"); 
    return;
  }else{
    digitalWrite(sdErrorLedPin, LOW); 
    //Serial.println("SD OK"); 
  }
  // The audio library needs to know which CS pin to use for recording
  audio.CSPin = SD_ChipSelectPin;
}


void loop() {
/* uncomment if you want to use serial comms for controls
  if(Serial.available()){                          //Send commands over serial to play
     switch(Serial.read()){
       case 'r': audio.startRecording("test.wav",16000,A0); 
                 Serial.println("Starting Recording"); 
                 break;    //Record at 16khz sample rate on pin A0
       case 's': audio.stopRecording("test.wav"); 
                 Serial.println("Stoping Recording"); 
                 break;              //Stop recording
        
      }
    }
*/

  // use buttons as controls
  int recReading = digitalRead(recButtonPin);
  int stopReading = digitalRead(stopButtonPin);

  // start the recording
  if(recReading == HIGH){
    digitalWrite(armedLedPin, HIGH);
    int scaled = A0;
    audio.startRecording("test.wav",8000,scaled); 
    
  }

  //stop the recording
  if(stopReading == HIGH){
    
    audio.stopRecording("test.wav");
    
    //half a second delay to make sure the 
    //file is saved before removing SD card
    delay(500);
    digitalWrite(armedLedPin, LOW);
  }
  
}
