/*********************************************************************

  Radar-Simulator

  Simuliert den Bildschirm eines alten Radargeräts mit einem kreisförmig drehenden Strahl für Modellschiffe.
  Läuft mit einem 0.96" OLED-Display mit 64x128 Pixeln (solle mit jedem 1306 kompatiblen Display funktionieren)
  und einem Arduino Pro Mini (5V, 16 MHz)

  Simulates the display of an vintage radar screen with rotating beam for a model ship. Uses 0.96" oled
  display with 64x128 pixel and an Arduino pro mini (5V, 16 MHz).

  CC-BY SA 2016 Kai Laborenz

*********************************************************************/

//#include <avr/power.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

#define OLED_RESET 4

// change here between small 0.49" display with 64x32 pixel powered by 1306 driver
// and 0.96" display with 1106 driver (i2c)

#define OLED_BIG
//#define OLED_SMALL

// uncomment to use fast mode with broader stepping
//#define FASTMODE

#ifdef OLED_SMALL
  U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // tiny 64*32 display
#else  
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R3, /* reset=*/ U8X8_PIN_NONE); // 128*64 dsiplay
#endif

int lh; //lineheight
int dw; // displayWidth
int dh; // displayHeight

void initRadar(void) {

  int textStart = 20;  // starting position for text messages

  #ifdef OLED_SMALL
    u8g2.setFont(u8g2_font_profont10_tr);
    textStart = 15;
    #define REFRESH 20 // refresh rate for radar screen (turn speed of line, bigger is slower)
  #else  
    u8g2.setFont(u8g2_font_profont11_tr);
    #define REFRESH 0 
  #endif  

  u8g2.clearBuffer();
  lh = u8g2.getMaxCharHeight();
  u8g2.setCursor(1,textStart);
  
  u8g2.print("Radar");
  u8g2.setCursor(1,textStart+lh); //start on next line
  u8g2.print("startet");
  u8g2.sendBuffer();
  delay(500);

  for(int p = 0; p < 3; p++){
    u8g2.print(".");
    u8g2.sendBuffer();
    delay(500);
  }

 #ifdef FASTMODE
  u8g2.setCursor(1,textStart+3*lh);
  u8g2.print("FASTMODE");
  u8g2.sendBuffer();
 #endif
 
  delay(2000);
  u8g2.clearDisplay();
}

void drawScreen(void) {

 // draws circles on display
 int size_increment = 15;
 int number_circles = 4;

 #ifdef OLED_SMALL
  size_increment = 10;
  number_circles = 3;
 #endif 

  u8g2.drawDisc(dw/2, dh/2, 2, U8G2_DRAW_ALL);
  
  for(int p = 0; p < number_circles+1; p++){
    u8g2.drawCircle(dw/2, dh/2, p*size_increment, U8G2_DRAW_ALL);
  }
  //u8g2.sendBuffer();
}

void drawEchoes(void) {

 // draws some random radar echoes on screen

#ifdef OLED_SMALL
  int maxSmall = 20;
  int maxMedium = 10;
  int maxBig = 5;
#else
  int maxSmall = 40;
  int maxMedium = 25;
  int maxBig = 20;
#endif
 

 // small echoes 
 for(int p = 0; p < maxSmall; p++){
  int x = random(2, dw);
  int y = random(2, dh);
  u8g2.drawPixel(x,y);
  } 
  
// medium echoes 
for(int p = 0; p < maxMedium; p++){
  int x = random(2, dw);
  int y = random(2, dh);
   u8g2.drawPixel(x,y);
  u8g2.drawPixel(x+1,y);
  u8g2.drawPixel(x+2,y+1);
  u8g2.drawPixel(x,y+2);
  u8g2.drawPixel(x-1,y);
  }
  
// large echoes 
 for(int p = 0; p < maxBig; p++){
  int x = random(2, dw);
  int y = random(2, dh);
   u8g2.drawPixel(x,y);
  u8g2.drawPixel(x+1,y);
  u8g2.drawPixel(x+2,y+1);
  u8g2.drawPixel(x-1,y+2);
  u8g2.drawPixel(x+2,y+3);
  } 
}

void setup()   {          
  Serial.begin(9600);
  Wire.setClock(400000);

  // init display
  u8g2.begin();

  // get screen dimensions for later use
  dw = u8g2.getDisplayWidth();
  dh = u8g2.getDisplayHeight();
  
  // Clear buffer and display
  u8g2.clearBuffer();
  u8g2.clearDisplay();

  initRadar();
}

void loop() {

// simulated rotating beam by drawing a line from center to display border
// end point for line is calculated in 4 phases for each side
u8g2.clearBuffer();
drawEchoes();

#ifdef FASTMODE

for(int x = 0; x < dw; x+=2){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, x-2, 0);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2, dh/2, x, 0);
    u8g2.drawLine(dw/2, dh/2, x+2, 0);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH);
  }

for(int x = 0; x < dh; x+=2){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, dw-1, x-2);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2,dh/2, dw-1, x);
    u8g2.drawLine(dw/2, dh/2, dw-1, x+2);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH); 
}     

for(int x = dw; x > 0; x-=2){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, x+2, dh-1);
    u8g2.drawLine(dw/2, dh/2, x+3, dh-1);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2, dh/2, x, dh-1);
    u8g2.drawLine(dw/2, dh/2, x-2, dh-1);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH); 
  }

for(int x = dh; x > 0; x-=2){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, 0, x+2);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2, dh/2, 0, x);
    u8g2.drawLine(dw/2, dh/2, 0, x-2);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH);
  }

#else

for(int x = 0; x < dw; x++){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, x-1, 0);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2, dh/2, x, 0);
    u8g2.drawLine(dw/2, dh/2, x+1, 0);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH);
  }

for(int x = 0; x < dh; x++){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, dw-1, x-1);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2,dh/2, dw-1, x);
    u8g2.drawLine(dw/2, dh/2, dw-1, x+1);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH); 
}     

for(int x = dw; x > 0; x--){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, x+1, dh-1);
    u8g2.drawLine(dw/2, dh/2, x+2, dh-1);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2, dh/2, x, dh-1);
    u8g2.drawLine(dw/2, dh/2, x-1, dh-1);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH); 
  }

for(int x = dh; x > 0; x--){
    u8g2.setDrawColor(0);
    u8g2.drawLine(dw/2, dh/2, 0, x+1);
    u8g2.setDrawColor(1);
    u8g2.drawLine(dw/2, dh/2, 0, x);
    u8g2.drawLine(dw/2, dh/2, 0, x-1);
    drawScreen();
    u8g2.sendBuffer();
    delay(REFRESH);
  }
#endif  
}
