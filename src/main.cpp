#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        2
#define TFT_RST       4 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        15

uint16_t encodedScore[] = {0,0,0};
int player1score = 0;
int player2score = 0;
int player3score = 0;
int player4score = 0;
int player5score = 0;

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}


void testText(){
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.print("This is a test");
}

void scoreDraw(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setTextWrap(true);

  tft.setCursor(0, 0);
  tft.print("Player 1");
  tft.print(" Score:");
  tft.print(player1score);
  tft.print(" ");
  //tft.print(encodedScore[2]);

  tft.setCursor(0, 10);
  tft.print("Player 2");
  tft.print(" Score:");
  tft.print(player2score);

  tft.setCursor(0, 20);
  tft.print("Player 3");
  tft.print(" Score:");
  tft.print(player3score);

  tft.setCursor(0, 30);
  tft.print("Player 4");
  tft.print(" Score:");
  tft.print(player4score);

  tft.setCursor(0,40);
  tft.print("Player 5");
  tft.print(" Score:");
  tft.print(player5score);
}

void inputData(){
  if(encodedScore[0] == 1){

    if(encodedScore[2] == 0){
      player1score = encodedScore[1];
    }
    else{
      player1score = -encodedScore[1];
    }
  }

  if(encodedScore[0] == 2){
    if(encodedScore[2] == 0){
      player2score = encodedScore[1];
    }
    else{
      player2score = -encodedScore[1];
    }
  }

  if(encodedScore[0] == 3){
    if(encodedScore[2] == 0){
      player3score = encodedScore[1];
    }
    else{
      player3score = -encodedScore[1];
    }
  }

  if(encodedScore[0] == 4){
    if(encodedScore[2] == 0){
      player4score = encodedScore[1];
    }
    else{
      player4score = -encodedScore[1];
    }
  }

  if(encodedScore[0] == 5){
    if(encodedScore[2] == 0){
      player5score = encodedScore[1];
    }
    else{
      player5score = -encodedScore[1];
    }
  }

}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&encodedScore, incomingData, sizeof(encodedScore));
  inputData();
  scoreDraw();
}

void setup(void) {
  Serial.begin(9600);

  //WiFi.channel(0);
  WiFi.mode(WIFI_MODE_STA);
  //Serial.println(String(WiFi.macAddress()));
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);

  Serial.println(F("Initialized"));
  tft.fillScreen(ST77XX_BLACK);
  delay(500);
  tft.fillScreen(ST77XX_BLACK);
  testText();
  Serial.println("done");
}

void loop() {
  delay(500);
}



