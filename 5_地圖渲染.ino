// 假設已經有 TFT、SD、JPEGDecoder 初始化

#define MAP_WIDTH  3200
#define MAP_HEIGHT 600
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 地圖物件結構
struct MapObject {
  int x, y, width, height;
  const char* jpgFile;
  uint16_t* buffer;
  int16_t bufferWidth, bufferHeight;
  bool specialEffect; // 例如：true=會閃爍
};

// 定義地圖物件陣列
MapObject mapObjects[] = {
  { 100, 500, 60, 40, "/ob1/1.jpg", nullptr, 0, 0, false },   // 牆
  { 400, 400, 80, 80, "/ob1/16.jpg", nullptr, 0, 0, true },   // 特殊效果物件
  { 900, 300, 50, 50, "/ob1/34.jpg", nullptr, 0, 0, false }   // 鑰匙
};
const int mapObjectCount = sizeof(mapObjects) / sizeof(mapObjects[0]);

// 預先載入所有物件圖片到 buffer
void preloadMapImages() {
  for (int i = 0; i < mapObjectCount; i++) {
    mapObjects[i].buffer = loadJpg(mapObjects[i].jpgFile, mapObjects[i].bufferWidth, mapObjects[i].bufferHeight);
    // loadJpg 需回傳 uint16_t* 並設定 bufferWidth/bufferHeight
  }
}

// 畫地圖（只畫目前螢幕可見範圍）
void drawMapScene(int scrollX, int scrollY) {
  for (int i = 0; i < mapObjectCount; i++) {
    // 判斷是否在可見範圍
    if (mapObjects[i].x + mapObjects[i].width >= scrollX &&
        mapObjects[i].x <= scrollX + SCREEN_WIDTH &&
        mapObjects[i].y + mapObjects[i].height >= scrollY &&
        mapObjects[i].y <= scrollY + SCREEN_HEIGHT) {
      // 特殊效果：閃爍
      if (mapObjects[i].specialEffect && (millis() / 300) % 2 == 0) continue;
      // 畫圖
      drawJpgFromBuffer(
        mapObjects[i].buffer,
        mapObjects[i].x - scrollX,
        mapObjects[i].y - scrollY,
        mapObjects[i].bufferWidth,
        mapObjects[i].bufferHeight
      );
    }
  }
}

// 載入JPG圖片到buffer（需搭配JPEGDecoder）
uint16_t* loadJpg(const char* filename, int16_t& width, int16_t& height) {
  if (JpegDec.decodeSdFile(filename) == 0) return nullptr;
  width = JpegDec.width;
  height = JpegDec.height;
  uint16_t* buffer = (uint16_t*)ps_malloc(width * height * sizeof(uint16_t));
  // ...將JpegDec.pImage資料複製到buffer...
  return buffer;
}

// 畫buffer到螢幕（可用 drawRGBBitmap 或自訂backBuffer）
void drawJpgFromBuffer(const uint16_t* buffer, int16_t x, int16_t y, int16_t width, int16_t height) {
  tft.drawRGBBitmap(x, y, buffer, width, height);
}

// --- 使用方式 ---
// setup() 裡呼叫 preloadMapImages();
// loop() 裡根據玩家位置計算 scrollX, scrollY，然後呼叫 drawMapScene(scrollX, scrollY);
