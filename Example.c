#include "lcd_i2c.h"

int main() 
{
// If you wire module I2C of LCD to I2C1
I2C1_Init();
// If you wire module I2C of LCD to I2C2
//I2C1_Init();

//Init LCD
LCD_Init(I2C1);

// Print float number
LCD_SetCursor(0, 4);
LCD_SendFloat(30.56, 2);

// Print string and create new char
uint8_t heart[] =
{
    0x00,
    0x0A,
    0x1F,
    0x1F,
    0x1F,
    0x0E,
    0x04,
    0x00
};
LCD_CreateChar(0, heart);
LCD_SetCursor(1, 4);
LCD_SendString("I ");
LCD_WriteCustomChar(0);
LCD_SendString(" STM32");

while(1)
{
}
}
