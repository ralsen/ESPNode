/*
  tft.h

  !!! for history see settings.h !!!

  ToDos:    -

  hints:    -
*/

#if (H_TFT_18 == H_TRUE)
# ifndef _TFT_H_
# define _TFT_H_

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#define TFT_CS        4
#define TFT_RST       16 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        15

// color and text defintions for display2Temps()
#define BG_COLOR          0xad55
#define TEXT_COLOR        ST7735_WHITE
#define VALUE_TEXT_COLOR  ST7735_WHITE
#define VALUE_BG_COLOR    0x0617
#define HEAD_TEXT_SIZE    1
#define VALUE_TEXT_SIZE   5

// initialize ST7735 TFT library with hardware SPI module
//SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
//MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
// BLK an 3,3V VCC  

void tft_hello(void);
void tft_info(void);
void tft_init2Temps(void);
void tft_display2Temps(int t1, int t2);
void tft_textWait(int s);
void tft_initR(int color);
void tft_fillScreen(int color);
void tft_setCursor(int x, int y);
void tft_setTextColor(int color);
void tft_setTextWrap(int mode);
void tft_println(String str);
void tft_print(String str);

extern Adafruit_ST7735 tft;

# endif // _TFT_H_
#endif //(H_TFT == H_TRUE)