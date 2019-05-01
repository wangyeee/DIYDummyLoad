#ifndef __ILI9163C_H
#define __ILI9163C_H

#include "spi.h"
#include "ILI9163C_regs.h"
#include "stdio.h"
#include "string.h"

#define _TFTWIDTH 128 //the REAL W resolution of the TFT
#define _TFTHEIGHT 160 //the REAL H resolution of the TFT
#define _GRAMWIDTH _TFTWIDTH
#define _GRAMHEIGH _TFTHEIGHT
#define _GRAMSIZE _GRAMWIDTH * _GRAMHEIGH //*see note 1
#define __COLORSPC 1 // 1:GBR - 0:RGB
#define __GAMMASET3 //uncomment for another gamma
#define __OFFSET 0

#define	BLACK 0x0000
#define	BLUE 0x001F
#define	RED 0xF800
#define	GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0  
#define WHITE 0xFFFF

// used for lcd_printf
#define LCD_STR_BUF_LEN 256

#define fillScreen(x) clearScreen(x)

extern void ILI9163CInit(void);
extern uint8_t write_string(char* str, uint8_t len);

extern void colorSpace(uint8_t cspace);
extern void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern void clearScreen(uint16_t color);
extern void setRotation(uint8_t m);
extern uint8_t boundaryCheck(int16_t x, int16_t y);

extern void drawPixel(int16_t x, int16_t y, uint16_t color);

extern void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
extern void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

extern void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
extern void invertDisplay(uint8_t i);

//////////
extern void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
extern void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

extern void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
extern void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
extern void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
extern void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
extern void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
extern void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
extern void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
extern void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
extern void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
extern void drawBitmapBg(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
extern void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
extern void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
extern void setCursor(int16_t x, int16_t y);
extern void setTextColor(uint16_t c);
extern void setTextColorBg(uint16_t c, uint16_t bg);
extern void setTextSize(uint8_t s);
extern void setTextWrap(uint8_t w);

extern void write(uint8_t c);
extern int16_t height(void);
extern int16_t width(void);
extern uint8_t getRotation(void);
extern uint8_t write_string(char* str, uint8_t len);

#define lcd_printf(fmt, args...) {                   \
    char buf[LCD_STR_BUF_LEN];                       \
    uint32_t ret, i;                                 \
    ret = snprintf(buf, LCD_STR_BUF_LEN, fmt, args); \
    for (i = 0; i < ret; i++) {                      \
        write(buf[i]);                               \
    }                                                \
    memset(buf, 0, ret);                             \
}

#define lcd_print(s) lcd_printf("%s", s)
#define lcd_println(s) lcd_printf("%s\n", s)

#endif /* __ILI9163C_H */
