#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "man1.h"
#include "man2.h"
#include "man3.h"
#include "man4.h"

#define TFT_CS   10
#define TFT_DC   8
#define TFT_RST  9
#define IMG_WIDTH  35
#define IMG_HEIGHT 35

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// 動畫幀陣列
const uint16_t* manFrames[] = { man1, man2, man3, man4 };
const int frameCount = 4;

int playerX = 100;
int playerY = 100;
int animationFrame = 0;
unsigned long lastAnimationTime = 0;
unsigned long animationInterval = 120; // 幀切換間隔(ms)
bool isMoving = false;
bool facingLeft = false;

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  pinMode(2, INPUT); // 左
  pinMode(3, INPUT); // 右
  drawPlayer();
}

void loop() {
  // 偵測左右按鈕
  bool leftPressed = digitalRead(2) == HIGH;
  bool rightPressed = digitalRead(3) == HIGH;
  isMoving = leftPressed || rightPressed;

  // 方向判斷
  if (leftPressed)  facingLeft = true;
  if (rightPressed) facingLeft = false;

  // 動畫更新
  if (isMoving) {
    if (millis() - lastAnimationTime > animationInterval) {
      animationFrame = (animationFrame + 1) % frameCount;
      lastAnimationTime = millis();
      drawPlayer();
    }
    // 可加上移動座標邏輯
  } else {
    if (animationFrame != 0) {
      animationFrame = 0;
      drawPlayer();
    }
  }

  delay(10);
}

// 畫角色，根據 facingLeft 決定是否鏡像
void drawPlayer() {
  tft.fillRect(playerX, playerY, IMG_WIDTH, IMG_HEIGHT, ILI9341_BLACK); // 清除舊圖
  drawImageWithMirror(manFrames[animationFrame], playerX, playerY, IMG_WIDTH, IMG_HEIGHT, facingLeft);
}

// 鏡像繪圖函式
void drawImageWithMirror(const uint16_t* buffer, int16_t x, int16_t y, int16_t width, int16_t height, bool mirror) {
  for (int16_t j = 0; j < height; j++) {
    for (int16_t i = 0; i < width; i++) {
      int16_t drawX = mirror ? (width - 1 - i) : i;
      uint16_t color = buffer[j * width + i];
      if (color != buffer[0]) { // 跳過背景色
        tft.drawPixel(x + drawX, y + j, color);
      }
    }
  }
}