/*********************************************************************

  Radar-Simulator

  Simuliert den Bildschirm eines alten Radargeräts mit einem kreisförmig drehenden Strahl für Modellschiffe.
  Läuft mit einem 0.96" OLED-Display mit 64x128 Pixeln (solle mit jedem 1306 kompatiblen Display funktionieren)
  und einem Arduino Pro Mini (5V, 16 MHz)

  Simulates the display of an vintage radar screen with rotating beam for a model ship. Uses 0.96" oled
  display with 64x128 pixel and an Arduino pro mini (5V, 16 MHz).

  CC-BY SA 2016 Kai Laborenz

*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define REFRESH 0 // refresh rate for radar screen (turn speed of line)

 // 'Radarscreen' bitmap
const unsigned char backgroundMap [] PROGMEM = {
  0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x0c, 0x01, 0x20, 0x80, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x1e, 0x00, 0x00, 0x0c, 0x02, 0x20, 0x80, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1e, 0x9e, 0x00, 0x00, 0x0c, 0x02, 0x21, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xde, 0x00, 0x00, 0x0c, 0x02, 0x01, 0x00, 0x30, 0x04, 0x06, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xce, 0x00, 0x1c, 0x08, 0x02, 0x02, 0x06, 0x18, 0x03, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x18, 0x0e, 0x0e, 0x00, 0x1e, 0x08, 0x24, 0x02, 0x07, 0x38, 0x00, 0x06, 0x00, 0x00, 0x00, 
  0x00, 0x0c, 0x02, 0x0e, 0x00, 0x1c, 0x18, 0x34, 0x02, 0x00, 0x70, 0x00, 0x02, 0xc1, 0x00, 0x00, 
  0x00, 0x06, 0x02, 0x1c, 0x00, 0x1c, 0x18, 0xe4, 0x04, 0x00, 0x30, 0x00, 0x00, 0x63, 0x38, 0x00, 
  0x00, 0x02, 0x00, 0x08, 0x00, 0x04, 0x10, 0x04, 0x04, 0x00, 0x38, 0x00, 0x00, 0x30, 0x84, 0x00, 
  0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x10, 0x08, 0x24, 0x00, 0x10, 0x00, 0x00, 0x24, 0x04, 0x00, 
  0x00, 0x01, 0x00, 0x0c, 0x00, 0x00, 0x30, 0x08, 0x04, 0x06, 0x00, 0x00, 0x00, 0x48, 0x04, 0x00, 
  0x00, 0x01, 0x80, 0x06, 0x00, 0x00, 0x20, 0x08, 0x04, 0x0e, 0x00, 0x00, 0x06, 0x98, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x18, 0x00, 0x1c, 0x00, 0x00, 0x01, 0x90, 0x02, 0x00, 
  0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x02, 0x00, 
  0x00, 0x00, 0x02, 0x06, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0xe0, 0x00, 0x0d, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x38, 0x01, 0xc0, 0x01, 0x00, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xdc, 0x03, 0x03, 0x00, 0x20, 0x00, 0x00, 0x00, 0x0d, 0x80, 0xd0, 0x01, 0x00, 
  0x00, 0x00, 0x8f, 0xdc, 0x0f, 0x86, 0x00, 0x00, 0x00, 0x00, 0x21, 0x01, 0x80, 0x04, 0x1e, 0x00, 
  0x00, 0x00, 0x0e, 0xc0, 0x09, 0x8c, 0x03, 0x40, 0x08, 0x00, 0x00, 0x08, 0x00, 0x22, 0x30, 0x00, 
  0x00, 0x00, 0x01, 0xc0, 0x10, 0x88, 0x03, 0x40, 0x70, 0x01, 0xfd, 0x90, 0x04, 0x00, 0xb0, 0x00, 
  0x00, 0x00, 0x00, 0x80, 0x00, 0x90, 0x0e, 0x80, 0xe0, 0x07, 0xf0, 0x00, 0x18, 0x00, 0x70, 0x00, 
  0x00, 0x06, 0x00, 0x00, 0x09, 0xb0, 0x01, 0x0e, 0x40, 0x3c, 0x06, 0x00, 0x18, 0x02, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x4e, 0x1f, 0x30, 0x00, 0x18, 0x00, 0x00, 0x04, 0x00, 0x00, 0x06, 0x00, 0x00, 
  0x18, 0x00, 0x00, 0x7f, 0xf6, 0xb4, 0x38, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x1e, 0x00, 0x00, 
  0x3e, 0x00, 0x00, 0x3e, 0xc7, 0xad, 0x79, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
  0x32, 0x18, 0x00, 0x00, 0xde, 0x1c, 0x39, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x10, 0x00, 0x00, 
  0x00, 0x10, 0x00, 0x01, 0x7c, 0x3c, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x20, 0x60, 0x00, 
  0x00, 0x30, 0x80, 0x07, 0x43, 0x00, 0x00, 0x00, 0x06, 0x03, 0xc0, 0x00, 0x00, 0x46, 0x60, 0x00, 
  0x00, 0xe1, 0x81, 0xfd, 0x50, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x88, 0x80, 0x00, 
  0x81, 0xe1, 0x87, 0x01, 0x8c, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 
  0x73, 0xf1, 0x86, 0x08, 0x30, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x60, 0x00, 
  0x0e, 0x3d, 0x86, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 
  0x0c, 0x18, 0x82, 0x30, 0x00, 0x00, 0x00, 0x20, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 
  0x08, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x80, 0x20, 0x00, 0x00, 0x00, 0x60, 0x00, 
  0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x70, 0x00, 0x03, 0x01, 0x40, 0x00, 
  0x10, 0x00, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00, 0x08, 0xc0, 0x00, 0x02, 0x00, 0x80, 0x00, 
  0x60, 0x03, 0x00, 0x40, 0x01, 0x18, 0x0c, 0x00, 0x00, 0x08, 0x40, 0x00, 0x06, 0x00, 0x40, 0x00, 
  0x80, 0x01, 0x04, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0c, 0x08, 0x00, 0x04, 0x04, 0x00, 0x20, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x0c, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x00, 0x83, 0x08, 0x01, 0x18, 0x06, 0x00, 0x0c, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x10, 0x00, 
  0x00, 0x00, 0x10, 0x00, 0x01, 0x08, 0x0c, 0x00, 0x00, 0x32, 0x00, 0x08, 0x00, 0x60, 0x10, 0x00, 
  0x00, 0x00, 0x60, 0x3c, 0x00, 0x00, 0x08, 0x00, 0x00, 0x63, 0xe0, 0x00, 0x00, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x00, 0x10, 0x00, 0x60, 0x00, 0x00, 0x10, 0x00, 0x00, 0x64, 0x08, 0x00, 
  0x00, 0x00, 0x04, 0x20, 0x04, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x63, 0x06, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x08, 0xe0, 0x30, 0x63, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x60, 0x03, 0x80, 0x07, 0xf0, 0x08, 0xf6, 0x00, 0x80, 
  0x80, 0x00, 0x08, 0x0c, 0x11, 0xd0, 0x80, 0x00, 0x00, 0xc0, 0x00, 0x60, 0xe4, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x18, 0x19, 0x91, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x87, 0x00, 0x40, 
  0x00, 0x00, 0x3f, 0x18, 0x2c, 0x07, 0x00, 0x00, 0x33, 0x80, 0x00, 0x63, 0x64, 0x53, 0x8c, 0x40, 
  0xc0, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x80, 0x00, 0x70, 0xc2, 0x08, 0x1c, 0x40, 
  0x40, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x20, 0xe2, 0x06, 0x38, 0x20, 
  0x80, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0c, 0x10, 0x10, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x08, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0c, 0x06, 0x01, 0x0e, 0x00, 0x04, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x04, 
  0x80, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0xc4, 0xc0, 0x04, 
  0x80, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x30, 0x24, 0x40, 0x00, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0xf0, 0x12, 0x20, 0x02, 
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x2e, 0x70, 0x0e, 0x20, 0x02, 
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x74, 0x30, 0x00, 0x20, 0x00, 
  0x38, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x74, 0x30, 0x00, 0x10, 0x04, 
  0x1e, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x32, 0x60, 0x00, 0x08, 0x04, 
  0x0f, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb7, 0xc0, 0x00, 0x38, 0x04, 
  0x03, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xc0, 0x00, 0x78, 0xe4
};


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // Clear the buffer.
  display.clearDisplay();

  display.setRotation(1);
  testscrolltext();
  display.clearDisplay();
  display.setRotation(2);
}

void loop() {

// simulated rotating beam by drawing a line from center to display border
// end point for line is calculated in 4 phases for each side
  
for(int x = 0; x < 127; x++){
    display.drawLine(display.width()/2, display.height()/2, x-1, 0, BLACK);
    display.drawLine(display.width()/2, display.height()/2, x, 0, WHITE);
    display.drawLine(display.width()/2, display.height()/2, x+1, 0, WHITE);
    display.drawBitmap(0, 0,  backgroundMap, 128, 64, 1);
    drawScreen();
    delay(REFRESH);
  }
for(int x = 0; x < 63; x++){
    display.drawLine(display.width()/2, display.height()/2, 127, x-1, BLACK);
    display.drawLine(display.width()/2, display.height()/2, 127, x, WHITE);
    display.drawLine(display.width()/2, display.height()/2, 127, x+1, WHITE);
    display.drawBitmap(0, 0,  backgroundMap, 128, 64, 1);
    drawScreen();
    delay(REFRESH); 
  }     
for(int x = 127; x > 0; x--){
    display.drawLine(display.width()/2, display.height()/2, x+1, 63, BLACK);
    display.drawLine(display.width()/2, display.height()/2, x+2, 63, BLACK);
    display.drawLine(display.width()/2, display.height()/2, x, 63, WHITE);
    display.drawLine(display.width()/2, display.height()/2, x-1, 63, WHITE);
    display.drawBitmap(0, 0,  backgroundMap, 128, 64, 1);
    drawScreen();
    delay(REFRESH); 
  }
for(int x = 63; x > 0; x--){
    display.drawLine(display.width()/2, display.height()/2, 0, x+1, BLACK);
    display.drawLine(display.width()/2, display.height()/2, 0, x, WHITE);
    display.drawLine(display.width()/2, display.height()/2, 0, x-1, WHITE);
    display.drawBitmap(0, 0,  backgroundMap, 128, 64, 1);
    drawScreen();
    delay(REFRESH);
  }
  display.drawLine(display.width()/2, display.height()/2, 0, 1, BLACK); // needed to get rid of some artefact lines
  display.drawLine(display.width()/2, display.height()/2, 1, 1, BLACK);
}

void drawScreen(void) {

  // draws circles on display
  
  display.fillCircle(display.width()/2, display.height()/2, 2 , WHITE);
  display.drawCircle(display.width()/2, display.height()/2, 15, WHITE);
  display.drawCircle(display.width()/2, display.height()/2, 30, WHITE);
  display.drawCircle(display.width()/2, display.height()/2, 45, WHITE);
  display.drawCircle(display.width()/2, display.height()/2, 60, WHITE);
  display.display();
}

void testscrolltext(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(1,20);
  display.clearDisplay();
  display.println("Radar");
  display.print("startet");
  display.display();
  delay(800);
  display.print(".");
  display.display();
  delay(800);
  display.print(".");
  display.display();
  delay(800);
  display.print(".");
  display.display();
  delay(3000);
}
