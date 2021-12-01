
#include <Wire.h>
#include <Adafruit_SSD1306.h>

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
  //clear buffer
  display.clearDisplay();
  display.setTextSize(2);  
  // Draw white text           
  display.setTextColor(SSD1306_WHITE); 
}
/*
 * prepare the screen
 */
void prePrint(){
  //clear the display
  display.clearDisplay();
  // Normal 1:1 pixel scale
  // Start at top-left corner       
  display.setCursor(0,20);         
}


/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showValue(int v){
  prePrint();
  display.print(F("Pop Ct: "));
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


void setup() {
  // put your setup code here, to run once:
  displaySetUp();
  showAlmostDone();
  delay(1000);
}


void loop() {
//  int timeToWait = 1; //in seconds
//  // test functionality
//  delay(timeToWait*1000);
//  showAlmostDone();
//  delay(timeToWait*1000);
//  showDone();
//  delay(timeToWait*1000);
//  showWorking();
//  delay(timeToWait*1000);
//  showConnected();
//  delay(timeToWait*1000);
//  showConnecting();
//  for(int i = 0; i < 10; i++){
//    delay(timeToWait*1000);
//    showValue(i);
//    showValue2(i+2);
//  }
}
