
#include <Wire.h>
#include <Adafruit_SSD1306.h>

//declare and initialize library
Adafruit_SSD1306 dis = Adafruit_SSD1306(128, 64, &Wire, -1);


/*
 * 
 * clear the current display
 * set text size and color
 */
void displaySetUp(){
  //begin display usage
  dis.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  //clear buffer
  dis.clearDisplay();
  dis.setTextSize(2);  
  // Draw white text           
  dis.setTextColor(SSD1306_WHITE); 
}
/*
 * prepare the screen
 */
void prePrint(){
  //clear the display
  dis.clearDisplay();
  // Normal 1:1 pixel scale
  // Start at top-left corner       
  dis.setCursor(0,20);         
}


/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showValue(int v){
  prePrint();
  dis.print(F("Pop Ct: "));
  dis.println(v);
  dis.display();
}
/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showValue2(int v){
  dis.print(F("Time: "));
  dis.println(v);
  dis.display();
}
/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showConnecting(){
  prePrint();
  dis.println(F("Connecting..."));
  dis.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showConnected(){
  prePrint();
  dis.println(F("Connected!"));
  dis.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showWorking(){
  prePrint();
  //populate buffer
  dis.println(F("Optimizing Popcorn..."));
  //push to display
  dis.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showDone(){
  prePrint();
  dis.println(F("Done!"));
  dis.display();
}

/*
 * prepare screen for print
 * fill buffer with text to show
 * push text to display
 */
void showAlmostDone(){
  prePrint();
  dis.println(F("Almost"));
  dis.println(F("Done!"));
  dis.display();
}


void setup() {
  // put your setup code here, to run once:
  displaySetUp();
  showConnecting();
  delay(1000);
}


void loop() {
  int timeToWait = 1; //in seconds
  // test functionality
  delay(timeToWait*1000);
  showAlmostDone();
  delay(timeToWait*1000);
  showDone();
  delay(timeToWait*1000);
  showWorking();
  delay(timeToWait*1000);
  showConnected();
  delay(timeToWait*1000);
  showConnecting();
  for(int i = 0; i < 10; i++){
    delay(timeToWait*1000);
    showValue(i);
    showValue2(i+2);
  }
}
