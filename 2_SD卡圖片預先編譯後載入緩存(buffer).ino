#include <SD.h>
#include <JPEGDecoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// 定義TFT接腳
#define TFT_CS    10
#define TFT_RST   9
#define TFT_DC    8
#define TFT_MOSI  11
#define TFT_CLK   12
#define TFT_MISO  13

// 定義SD卡接腳
#define SD_CS     15

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// 遊戲物件結構
struct GameObject {
  int x, y;
  const char* jpgFile;
  uint16_t* buffer;
  int16_t bufferWidth, bufferHeight;
};

// 假設有兩個物件
GameObject objects[] = {
  { 50, 50, "/img1.jpg", nullptr, 0, 0 },
  { 150, 100, "/img2.jpg", nullptr, 0, 0 }
};

// 載入JPG並解碼到buffer
uint16_t* loadJpg(const char* filename, int16_t& width, int16_t& height) {
  if (JpegDec.decodeSdFile(filename) == 0) {
    Serial.print("JPEG decode failed: ");
    Serial.println(filename);
    return nullptr;
  }
  width = JpegDec.width;
  height = JpegDec.height;
  uint16_t* buffer = (uint16_t*)malloc(width * height * sizeof(uint16_t));
  if (!buffer) {
    Serial.println("Buffer allocation failed!");
    return nullptr;
  }
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

// 預先載入所有圖片
void preloadImages() {
  for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
    objects[i].buffer = loadJpg(objects[i].jpgFile, objects[i].bufferWidth, objects[i].bufferHeight);
    if (objects[i].buffer == nullptr) {
      Serial.print("Failed to load: ");
      Serial.println(objects[i].jpgFile);
    } else {
      Serial.print("Loaded: ");
      Serial.println(objects[i].jpgFile);
    }
  }
}

// 畫出buffer到螢幕
void drawBuffer(uint16_t* buffer, int x, int y, int w, int h) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      tft.drawPixel(x + i, y + j, buffer[j * w + i]);
    }
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized");

  preloadImages(); // 預先載入所有圖片
}

void loop() {
  tft.fillScreen(ILI9341_BLACK);
  // 取用buffer畫出所有物件
  for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
    if (objects[i].buffer != nullptr) {
      drawBuffer(objects[i].buffer, objects[i].x, objects[i].y, objects[i].bufferWidth, objects[i].bufferHeight);
    }
  }
  delay(1000); // 每秒重畫一次
}