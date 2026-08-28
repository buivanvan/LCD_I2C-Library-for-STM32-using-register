/**========================================================
 * LCD_I2C library for STM32
 * @file: 		lcd_i2c.c
 * @author: 	VVan
 * @Version: 	1.3
 *========================================================*/

#ifndef LCD_I2C_H
#define LCD_I2C_H

//#include "stm32f1xx.h"
#include "stm32f4xx.h"

/* LCD address */
#define LCD_I2C_ADDR    (0x27 << 1)

/* PCF8574 */
#define LCD_RS          0x01
#define LCD_RW          0x02
#define LCD_EN          0x04
#define LCD_BACKLIGHT   0x08
#define LCD_ALIGN_LEFT    0
#define LCD_ALIGN_CENTER  1
#define LCD_ALIGN_RIGHT   2

void I2C1_Init(void);
void I2C2_Init(void);

void LCD_Init(I2C_TypeDef *I2Cx);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendChar(char c);
void LCD_SendString(char *str);
void LCD_SendNumber(int32_t number);
void LCD_SendFloat(float number, uint8_t decimal);
void LCD_CreateChar(uint8_t location, const uint8_t charmap[]);
void LCD_WriteCustomChar(uint8_t location);
void LCD_PrintAlign(uint8_t row, char *str, uint8_t align);

#endif
