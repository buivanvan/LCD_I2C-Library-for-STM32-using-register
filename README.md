# LCD_I2C-Library-for-STM32-using-register

# Customize your STM32 series in "lcd_i2c.h"
```c
//#include "stm32f1xx.h"
//#include "stm32f2xx.h"
#include "stm32f4xx.h"
```
# How to use library
+ Step 1: Download file "lcd_i2c.h" and "lcd_i2c.c"
+ Step 2: In KeilC, you click right mouse to "Application/User/Core" then click to "Add Existing File to Group ..." and choose 2 file you downloaded
+ Step 3: Include library and use
+ Step 4: If you wire module I2C of LCD to I2C1:
  ```c
  I2C1_Init();
  ```
  If I2C2:
  ```c
  I2C2_Init();
  ```
+ Step 5: Then, you initialize LCD by
```c
LCD_Init(I2C1); // if you use I2C1 or LCD_Init(I2C1); if you use I2C2
```
+ Step 6: Use function to print: LCD_SendString(), LCD_SendFloat(), LCD_SendNumber() as example in file Example.c
<img width="720" height="348" alt="image" src="https://github.com/user-attachments/assets/ae3a26ee-1894-461a-9656-b2801d52f794" />

# Create new char
Use function LCD_CreateChar(uint8_t location, const uint8_t charmap[]) to create new char and function LCD_WriteCustomChar(uint8_t location) to print new char to screen.
