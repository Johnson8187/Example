#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <esp_heap_caps.h>

// 定義TFT接腳
#define TFT_CS    10
#define TFT_RST   9
#define TFT_DC    8
#define TFT_MOSI  11
#define TFT_CLK   12
#define TFT_MISO  13

// 定義SD卡接腳
#define SD_CS     15

// 創建TFT對象
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // 初始化串口
  Serial.begin(115200);
  
  // 啟用PSRAM
  if (!psramFound()) {
    Serial.println("PSRAM not found");
    while (1); // 停止運行
  }
  Serial.println("PSRAM found and enabled");

  // 初始化按鍵接腳
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  pinMode(BTN_SELECT, INPUT);
  pinMode(BTN_LEFT, INPUT);
  pinMode(BTN_RIGHT, INPUT);
  pinMode(BTN_BACK, INPUT);
  pinMode(BTN_TURN_LEFT, INPUT);
  pinMode(BTN_TURN_RIGHT, INPUT);

  // 初始化TFT螢幕
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Hello, ESP32-S3!");
  
  // 修正顏色反轉
  // 注意：有些螢幕可能需要設置為TRUE 我也不知道為甚麼
  tft.invertDisplay(false);  // 设置为false或true来切换颜色模式
  
  // 初始化SD卡
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    tft.println("SD init failed!");
    return;
  }
  Serial.println("SD Card initialized.");
  tft.println("SD init success.");
}

void loop() {
}
