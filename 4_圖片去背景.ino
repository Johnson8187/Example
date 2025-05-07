#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "myImage.h"  // 這是你用工具轉出來的.h檔，裡面有 uint16_t myImage[寬*高]

#define TFT_CS   10
#define TFT_DC   8
#define TFT_RST  9

#define IMG_WIDTH  35
#define IMG_HEIGHT 35

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// 去背函式：把左上角顏色當背景，全部換成黑色
void removeBackground(uint16_t* buffer, int width, int height) {
  uint16_t bgColor = buffer[0]; // 取得左上角顏色
  for (int i = 0; i < width * height; i++) {
    if (buffer[i] == bgColor) {
      buffer[i] = ILI9341_BLACK; // 換成黑色
    }
  }
}

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_WHITE);

  // 1. 複製圖片到暫存區（避免直接改.h檔內容）
  uint16_t imgBuffer[IMG_WIDTH * IMG_HEIGHT];
  memcpy(imgBuffer, myImage, sizeof(imgBuffer));

  // 2. 去背
  removeBackground(imgBuffer, IMG_WIDTH, IMG_HEIGHT);

  // 3. 顯示在螢幕上
  tft.drawRGBBitmap(50, 50, imgBuffer, IMG_WIDTH, IMG_HEIGHT);
}

void loop() {
  // 不需重複畫
}