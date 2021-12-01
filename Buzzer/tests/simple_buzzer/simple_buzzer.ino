int buzzer = 5;
unsigned long halfSecond = 500;
unsigned long milliSeconds = 0;
void setup() {
  // put your setup code here, to run once:

  
}

void loop() {
  // put your main code here, to run repeatedly:
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

  
