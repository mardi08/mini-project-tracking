#include "URTouch.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "SPI.h"
#include "Fonts/raleway9pt7b.h"
#include "Fonts/raleway12pt7b.h"
#include "Wire.h"

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

// For display and touch
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
URTouch ts(TS_SCK, TS_CS, TS_MOSI, TS_MISO, TS_IRQ);

char automan_flag = '0'; // 0 - do nothing, 1 - auto, 2 - manual
char operation = '0'; //  0 - do nothing 3 - do nothing, 4 - left, 5 - right, 6 - up, 7 - down, 8 - reset
int x, y;
int i = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(0x08);
  tft.begin();
  ts.InitTouch();
  ts.setPrecision(PREC_EXTREME);

  initialDisplay();

  do {
    Wire.onRequest(requestEvent);
    i = 1;
  } while(i = 0);
}

void loop() {
  if (ts.dataAvailable()) {
    ts.read();
    y = tft.height() - ts.getX();
    x = ts.getY();

    if ((x > 4) && (x < 118) && (y > 55) && (y < 80)) {
      automan_flag = '1'; // auto

      tft.drawRect(4, 55, 114, 25, tft.color565(232, 23, 93));
      tft.drawRect(122, 55, 114, 25, tft.color565(168, 167, 167));
    } else if ((x > 122) && (x < 236) && (y > 55) && (y < 80)) {
      automan_flag = '2'; // manual

      tft.drawRect(4, 55, 114, 25, tft.color565(168, 167, 167));
      tft.drawRect(122, 55, 114, 25, tft.color565(232, 23, 93));
    }

    if (automan_flag == '1') {
      operation = '1'; // auto

    } else if (automan_flag == '2') {
      operation = '2';
      if (ts.dataAvailable()) {
        ts.read();
        y = tft.height() - ts.getX();
        x = ts.getY();

        if ((x > 20) && (x < 70) && (y > 170) && (y < 220)) {
          operation = '4'; // left
        } else if ((x > 170) && (x < 220) && (y > 170) && (y < 220)) {
          operation = '5'; // right
        } else if ((x > 95) && (x < 145) && (y > 95) && (y < 145)) {
          operation = '6'; // up
        } else if ((x > 95) && (x < 145) && (y > 245) && (y < 295)) {
          operation = '7'; // down
        } else if ((x > 95) && (x < 145) && (y > 170) && (y < 220)) {
          operation = '8'; 
        } else {
          operation = '3'; // do nothing
        }
      } else {
        operation = '3'; // do nothing
      }
    } else {
      operation = '3';
    }
  }
  Serial.print("Mode ");
  Serial.println(automan_flag);
  Serial.print("Operation: ");
  Serial.println(operation);
}

unsigned long initialDisplay() {
  // background color
  tft.fillScreen(tft.color565(71, 71, 71));

  // draw rectangles for header and buttons
  tft.fillRect(4, 5, 232, 45, tft.color565(204, 82, 122));
  tft.fillRect(4, 55, 114, 25, tft.color565(168, 167, 167));
  tft.fillRect(122, 55, 114, 25, tft.color565(168, 167, 167));
  tft.fillRect(95, 170, 50, 50, tft.color565(168, 167, 167));

  // draw triangles for direction buttons
  tft.fillTriangle(20, 195, 70, 220, 70, 170, tft.color565(168, 167, 167));
  tft.fillTriangle(220, 195, 170, 170, 170, 220, tft.color565(168, 167, 167));
  tft.fillTriangle(120, 295, 145, 245, 95, 245, tft.color565(168, 167, 167));
  tft.fillTriangle(120, 95, 95, 145, 145, 145, tft.color565(168, 167, 167));

  // display text
  tft.setFont(&raleway12pt7b);

  tft.setCursor(95, 35);
  tft.setTextColor(tft.color565(54, 54, 54));
  //tft.setTextSize(5);
  tft.println("V3M");

  tft.setFont(&raleway9pt7b);

  tft.setCursor(17, 72);
  tft.setTextColor(tft.color565(54, 54, 54));
  //tft.setTextSize(2);
  tft.println("Automatic");

  tft.setCursor(145, 72);
  tft.setTextColor(tft.color565(54, 54, 54));
  //tft.setTextSize(2);
  tft.println("Manual");

  tft.setCursor(102, 200);
  tft.setTextColor(tft.color565(54, 54, 54));
  //tft.setTextSize(5);
  tft.println("RST");

}

void requestEvent() {
  //Wire.write(automan_flag);
  Wire.write(operation);
}

