#ifndef LS013B7DH03_H__
#define LS013B7DH03_H__

#include <stdint.h>
#include <stdbool.h>

#define OLED_MAX_X (128)
#define OLED_MAX_Y (128)

void lcd_init(void);
void lcd_clear(void);
void lcd_refresh(void);

void lcd_drawpoint(uint16_t x,uint16_t y,bool bDraw);
void lcd_fillRect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,bool bDraw);
void lcd_fillRectByXY(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,bool bDraw);
uint32_t lcd_getpoint(uint16_t x,uint16_t y);

void lcd_print_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void lcd_print_num(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void lcd_print_string(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);

#endif
