
#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, 0);//

void setup() {
  //(screen width, screen height, library, resetpin for oled)
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);

  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,20);         // Start at top-left corner
  display.println(F("Connecting..."));
  display.display();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,20);   
  display.println(F("Connected!"));
  display.display();
  
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,20);  
  display.println(F("Optimizing Popcorn..."));
  display.display();
    delay(1000);
    display.clearDisplay();
   display.setCursor(0,20);  
  display.println(F("Almost"));
  display.println(F("Done!"));
  display.display();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,20);   
  display.println(F("Done!"));
    display.display();
    delay(1000);
}

void loop() {

  testdrawstyles();    // Draw 'stylized' characters
  delay(1000);
}

void testdrawstyles(void) {
  
}
