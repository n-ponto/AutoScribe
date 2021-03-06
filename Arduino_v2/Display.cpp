/*
Documentation for Nokia 5110 Library
https://www.arduino.cc/reference/en/libraries/nokia-5110-lcd-library/

YouTube video about loading graphics to the display
https://www.youtube.com/watch?v=aUZP0nzxc0k

Default font is 8 pixels high by 5 pixels wide
*/

#include <Nokia_LCD.h>

#include "Hardware.h"

#define FONT_HEIGHT 8
#define FONT_WIDTH (5 + 1)
#define SCREEN_HEIGHT 48
#define SCREEN_WIDTH 84
#define BOTTOM_ROW 4

Nokia_LCD lcd(DSP_PIN_CLK, DSP_PIN_DIN, DSP_PIN_DC, DSP_PIN_CE, DSP_PIN_RST, DSP_PIN_BL);

const unsigned char start_screen[504] PROGMEM= {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF1, 0x7F, 0x6F, 0x63,
0x6F, 0x7E, 0xF8, 0xC0, 0x00, 0x00, 0x0C, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x0C, 0x8C, 0xFC, 0xFC,
0x00, 0x00, 0x00, 0x0C, 0x0E, 0xFF, 0xFF, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0xF0, 0xF8,
0x1C, 0x0C, 0x04, 0x04, 0x0C, 0x1C, 0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03,
0x03, 0x02, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 0x03, 0x02, 0x00, 0x00, 0x03, 0x07, 0x06, 0x06,
0x03, 0x03, 0x03, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x06, 0x06, 0x06, 0x03, 0x03,
0x01, 0x00, 0x00, 0x03, 0x03, 0x07, 0x06, 0x06, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0x30, 0x18, 0x18,
0x18, 0x18, 0x18, 0x38, 0x30, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x0C,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFC, 0xFC, 0x04, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x81,
0x83, 0x87, 0x86, 0x86, 0xCE, 0xCC, 0xFC, 0x78, 0x10, 0x00, 0x20, 0xFC, 0xFF, 0x83, 0x81, 0xC3,
0xC3, 0x60, 0x60, 0x30, 0x18, 0x1C, 0xFF, 0xFF, 0x38, 0x0E, 0x06, 0x03, 0x03, 0x03, 0x03, 0x00,
0x7F, 0xFF, 0xC0, 0x80, 0xC0, 0xE0, 0x60, 0x30, 0x18, 0xFF, 0xFF, 0x3C, 0x8E, 0x87, 0xC3, 0x61,
0x7B, 0x3F, 0x3E, 0x18, 0x0C, 0x0C, 0x7E, 0xFF, 0xD9, 0xCC, 0xC7, 0xC3, 0x60, 0x60, 0x30, 0x18,
0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

void displayHeader(const char *mode) {
    lcd.clear();
    if (!lcd.setCursor(0, 0))  // Top left
        Serial.println("displayHeader out of bounds error");
    lcd.print(mode);
}

void initCoordinateDisplay() {
    const char coordText[] = "X 0000  Y 0000";
    lcd.setCursor(0, BOTTOM_ROW);
    lcd.print(coordText);
}

void clearCoord(uint16_t coord) {
    const char space = ' ';
    if (coord < 1000)
        lcd.print(space);
    if (coord < 100)
        lcd.print(space);
    if (coord < 10)
        lcd.print(space);
}

/*********************** Public Functions *************************************/

void displayInit() {
    // Set up display pins
    pinMode(DSP_PIN_CLK, OUTPUT);
    pinMode(DSP_PIN_DIN, OUTPUT);
    pinMode(DSP_PIN_DC, OUTPUT);
    pinMode(DSP_PIN_CE, OUTPUT);
    pinMode(DSP_PIN_RST, OUTPUT);
    pinMode(DSP_PIN_BL, OUTPUT);
    // Set up lcd
    lcd.begin();          // Initialize the screen
    lcd.setContrast(60);  // Should be 40-60
    lcd.clear();          // Fill screen with black pixels
    lcd.draw(start_screen, sizeof(start_screen), true);
}

void displayManualControl() {
    displayHeader("MANUAL CONTROL");
    initCoordinateDisplay();
}

void displayDrawing() {
    displayHeader("DRAWING");
}

void updateCoordinateDisplay(int16_t x, int16_t y) {
    // Coordinates are left aligned
    lcd.setCursor(FONT_WIDTH * 2, BOTTOM_ROW);
    lcd.print(x);
    clearCoord(x);
    lcd.setCursor(FONT_WIDTH * 10, BOTTOM_ROW);
    lcd.print(y);
    clearCoord(y);
}

void updateInstructionCountDisplay(int count) {
    lcd.setCursor(0, BOTTOM_ROW);
    lcd.print(count);
    clearCoord(count);
}
