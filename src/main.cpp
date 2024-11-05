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

/////////////////ESPNOW INIT/////////////////////////////////////
uint8_t player1Mac[] = {0xC8, 0x2E, 0x18, 0x25, 0xEC, 0xB0};
esp_now_peer_info_t peerInfo;
String sta;

///////////////INTEGERS///////////////////////////////////////////

uint16_t encodedScore[] = {0,0,0};

int player1score = 0;
int player2score = 0;
int player3score = 0;
int player4score = 0;
int player5score = 0;

int timeRemaining = 0;
int secondTick = 0;
int gameLength = 1;

uint8_t gameFlag = 1;
int gameFlagupdated = 0;

bool EOGroutineDone = false;

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


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

void displayRefresh(){
  if(gameFlag == 1){
    tft.fillScreen(ST77XX_BLACK);
  }
  if(gameFlag == 0){
    tft.fillScreen(ST77XX_RED);
  }
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

  tft.setCursor(0,80);
  tft.setTextSize(2);
  tft.print("Time Left: ");
  tft.setCursor(0,100);
  tft.print(timeRemaining);


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

void gameTimer(){
  if((millis() - secondTick > 1000) && timeRemaining > 0){
    timeRemaining = (timeRemaining - 1);
    secondTick = millis();
    displayRefresh();
  }

  if(timeRemaining == 0){
    gameFlag = 0;
    if(EOGroutineDone == false){
      displayRefresh();
      EOGroutineDone = true;
    }
  }
}

void gameFlagsend(){
  if(millis() - gameFlagupdated > 1000){
    esp_now_send(player1Mac, (uint8_t *) &gameFlag, sizeof(gameFlag));
    gameFlagupdated = millis();
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&encodedScore, incomingData, sizeof(encodedScore));
  Serial.print(encodedScore[0]);
  Serial.print("  ");
  Serial.print(encodedScore[1]);
  Serial.print("  ");
  Serial.println(encodedScore[2]);
  inputData();
  displayRefresh();
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status == ESP_NOW_SEND_SUCCESS) 
  sta="Delivery Success"; else sta="Delivery Fail";
}

void setup(void) {
  Serial.begin(9600);

  //WiFi.channel(0);
  WiFi.mode(WIFI_MODE_STA);
  //Serial.println(String(WiFi.macAddress()));
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  //////////ADDING ESP PEERS//////////////////
  memcpy(peerInfo.peer_addr, player1Mac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

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

  timeRemaining = (gameLength * 60 * 1000)/1000;
}

void loop() {
  gameTimer();
  gameFlagsend();
  delay(1);
}



