#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <JPEGDecoder.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define MAP_WIDTH 3200
#define MAP_HEIGHT 600

Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 8, 9);

struct MapObject {
  int x, y, width, height;
  const char* jpgFile;
  uint16_t* buffer;
  int16_t bufferWidth, bufferHeight;
};

MapObject walls[] = {
  {100, 500, 60, 40, "/ob1/1.jpg", nullptr, 0, 0},
  {400, 400, 80, 80, "/ob1/16.jpg", nullptr, 0, 0}
};
const int wallCount = sizeof(walls) / sizeof(walls[0]);

int playerX = 40, playerY = 560, playerW = 35, playerH = 35;
int scrollX = 0, scrollY = 0;

// 預先載入所有物件圖片到 buffer
void preloadImages() {
  for (int i = 0; i < wallCount; i++) {
    walls[i].buffer = loadJpg(walls[i].jpgFile, walls[i].bufferWidth, walls[i].bufferHeight);
  }
}

// 載入JPG圖片到buffer
uint16_t* loadJpg(const char* filename, int16_t& width, int16_t& height) {
  if (JpegDec.decodeSdFile(filename) == 0) return nullptr;
  width = JpegDec.width;
  height = JpegDec.height;
  uint16_t* buffer = (uint16_t*)ps_malloc(width * height * sizeof(uint16_t));
  if (!buffer) return nullptr;
  uint16_t* pImg = buffer;
  while (JpegDec.read()) {
    for (uint16_t y = 0; y < JpegDec.MCUHeight; y++) {
      for (uint16_t x = 0; x < JpegDec.MCUWidth; x++) {
        int mcuPixelIndex = x + y * JpegDec.MCUWidth;
        int imgPixelIndex = (JpegDec.MCUx * JpegDec.MCUWidth + x) + (JpegDec.MCUy * JpegDec.MCUHeight + y) * width;
        if (imgPixelIndex < width * height) {
          buffer[imgPixelIndex] = JpegDec.pImage[mcuPixelIndex];
        }
      }
    }
  }
  return buffer;
}

// 畫物件到 backBuffer
void drawObjectToBuffer(MapObject& obj, uint16_t* backBuffer) {
  for (int j = 0; j < obj.bufferHeight; j++) {
    for (int i = 0; i < obj.bufferWidth; i++) {
      int drawX = obj.x - scrollX + i;
      int drawY = obj.y - scrollY + j;
      if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
        backBuffer[drawY * SCREEN_WIDTH + drawX] = obj.buffer[j * obj.bufferWidth + i];
      }
    }
  }
}

// 畫角色到 backBuffer（這裡用單色方塊代表）
void drawPlayerToBuffer(uint16_t* backBuffer) {
  for (int j = 0; j < playerH; j++) {
    for (int i = 0; i < playerW; i++) {
      int drawX = playerX - scrollX + i;
      int drawY = playerY - scrollY + j;
      if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
        backBuffer[drawY * SCREEN_WIDTH + drawX] = 0xF800; // 紅色
      }
    }
  }
}

// 畫面更新主流程
void updateScreen() {
  // 計算 scrollX/scrollY 讓角色盡量在畫面中央
  scrollX = playerX - SCREEN_WIDTH / 2;
  scrollY = playerY - SCREEN_HEIGHT / 2;
  if (scrollX < 0) scrollX = 0;
  if (scrollX > MAP_WIDTH - SCREEN_WIDTH) scrollX = MAP_WIDTH - SCREEN_WIDTH;
  if (scrollY < 0) scrollY = 0;
  if (scrollY > MAP_HEIGHT - SCREEN_HEIGHT) scrollY = MAP_HEIGHT - SCREEN_HEIGHT;

  // 雙緩衝：先畫到 backBuffer
  static uint16_t* backBuffer = (uint16_t*)ps_malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
  memset(backBuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t)); // 清空為黑色

  // 畫地圖物件
  for (int i = 0; i < wallCount; i++) {
    // 只畫在可見範圍的物件
    if (walls[i].x + walls[i].width >= scrollX && walls[i].x <= scrollX + SCREEN_WIDTH &&
        walls[i].y + walls[i].height >= scrollY && walls[i].y <= scrollY + SCREEN_HEIGHT) {
      drawObjectToBuffer(walls[i], backBuffer);
    }
  }
  // 畫角色
  drawPlayerToBuffer(backBuffer);

  // 一次性把 backBuffer 畫到螢幕
  tft.startWrite();
  tft.setAddrWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.writePixels(backBuffer, SCREEN_WIDTH * SCREEN_HEIGHT);
  tft.endWrite();
}

void setup() {
  tft.begin();
  SD.begin();
  preloadImages();
  updateScreen();
}

void loop() {
  // 這裡可以根據按鈕改變 playerX, playerY
  // 只要角色有移動就呼叫 updateScreen()
}