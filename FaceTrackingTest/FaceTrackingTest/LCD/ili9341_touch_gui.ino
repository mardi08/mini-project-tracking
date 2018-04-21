#include "URTouch.h"
#include "URTouch.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "SPI.h"


// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// Define Display Pins
#define TFT_DC 9
#define TFT_CS 10
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CLK 13
#define TFT_RST 8

// Define Touch Pins
#define TS_SCK 3
#define TS_CS 4
#define TS_MOSI 5
#define TS_MISO 6
#define TS_IRQ 7

// Define button area
#define autoButton_x 4
#define autoButton_y 55
#define manuButton_x 122
#define manuButton_y 55
#define buttonWidth 114
#define buttonHeight 25


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
URTouch ts(TS_SCK, TS_CS, TS_MOSI, TS_MISO, TS_IRQ);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.begin();
  ts.InitTouch();
  ts.setPrecision(PREC_EXTREME);

  tft.fillScreen(ILI9341_WHITE);
}

void loop() {
  int x, y;
  // put your main code here, to run repeatedly:
  rectDisplay();
  textDisplay();
  triangleDisplay();

  while (1) {
    if (ts.dataAvailable()) {
      ts.read();
      y = tft.height() - ts.getX();
      x = ts.getY();
      Serial.println(x);
      Serial.println(y);

      if ((x > 4) && (x < 118)) {
        if ((y > 55) && (y < 80)) {
          Serial.println("btn hit");
        tft.fillRect(4, 55, 114, 25, tft.color565(5, 0, 0));
        }
      } else {
        Serial.println("btn not hit");
      }
    }
  }

}

unsigned long textDisplay() {

  tft.setCursor(75, 10);

  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(5);
  tft.println("V3M");

  tft.setCursor(8, 60);

  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println("Automatic");

  tft.setCursor(145, 60);

  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println("Manual");
}

unsigned long triangleDisplay() {
  tft.fillTriangle(20, 195, 70, 220, 70, 170, tft.color565(0, 0, 0));
  tft.fillTriangle(220, 195, 170, 170, 170, 220, tft.color565(0, 0, 0));
  tft.fillTriangle(120, 295, 145, 245, 95, 245, tft.color565(0, 0, 0));
  tft.fillTriangle(120, 95, 95, 145, 145, 145, tft.color565(0, 0, 0));
}

unsigned long rectDisplay() {
  tft.drawRect(4, 55, 114, 25, tft.color565(0, 0, 0));
  tft.drawRect(122, 55, 114, 25, tft.color565(0, 0, 0));
}

