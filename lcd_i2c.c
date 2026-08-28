/**========================================================
 * LCD_I2C library for STM32
 * @file: 		lcd_i2c.c
 * @author: 	VVan
 * @Version: 	1.3
 *========================================================*/

#include "lcd_i2c.h"

static I2C_TypeDef *LCD_I2C;
/*========================================================
 * Delay
 * STM32F407 running at approximately 168 MHz
 *========================================================*/
static void Delay_us(uint32_t us)
{
    uint32_t i;

    while (us--)
    {
        for (i = 0; i < 16; i++)
        {
            __NOP();
        }
    }
}
static void Delay_ms(uint32_t ms)
{
    while (ms--)
    {
        Delay_us(1000);
    }
}
/*========================================================
 * I2C1 Initialization
 *
 * PB8 -> SCL
 * PB9 -> SDA
 *========================================================*/
void I2C1_Init(void)
{
    /* Enable GPIOB */
    RCC->AHB1ENR |= (1U << 1);

    /* Enable I2C1 */
    RCC->APB1ENR |= (1U << 21);

    /*
     * PB8 -> SCL
     * PB9 -> SDA
     */

    /* Alternate Function */
    GPIOB->MODER &= ~((3U << 16) | (3U << 18));
    GPIOB->MODER |=  ((2U << 16) | (2U << 18));

    /* Open Drain */
    GPIOB->OTYPER |= (1U << 8) | (1U << 9);

    /* Pull-up */
    GPIOB->PUPDR &= ~((3U << 16) | (3U << 18));
    GPIOB->PUPDR |=  ((1U << 16) | (1U << 18));

    /* Very High Speed */
    GPIOB->OSPEEDR &= ~((3U << 16) | (3U << 18));
    GPIOB->OSPEEDR |=  ((3U << 16) | (3U << 18));

    /* I2C1 -> AF4 */
    GPIOB->AFR[1] &= ~((0xFU << 0) | (0xFU << 4));

    GPIOB->AFR[1] |= (4U << 0) | (4U << 4);

    /* Reset I2C */
    I2C1->CR1 |= (1U << 15);
    I2C1->CR1 &= ~(1U << 15);

    /*42 MHz */
    I2C1->CR2 = 42;

    /* 100 kHz */
    I2C1->CCR = 210;

    /* 42 MHz + 1 */
    I2C1->TRISE = 43;

    /* Enable */
    I2C1->CR1 |= (1U << 0);
}
/*========================================================
 * I2C2 Initialization
 *
 * PB10 -> SCL
 * PB11 -> SDA
 *========================================================*/
void I2C2_Init(void)
{
    /* Enable GPIOB */
    RCC->AHB1ENR |= (1U << 1);

    /* Enable I2C2 */
    RCC->APB1ENR |= (1U << 22);

    /*
     * PB10 -> SCL
     * PB11 -> SDA
     */

    /* Alternate Function */
    GPIOB->MODER &= ~((3U << 20) | (3U << 22));
    GPIOB->MODER |=  ((2U << 20) | (2U << 22));

    /* Open Drain */
    GPIOB->OTYPER |= (1U << 10) | (1U << 11);

    /* Pull-up */
    GPIOB->PUPDR &= ~((3U << 20) | (3U << 22));
    GPIOB->PUPDR |=  ((1U << 20) | (1U << 22));

    /* Very High Speed */
    GPIOB->OSPEEDR &= ~((3U << 20) | (3U << 22));
    GPIOB->OSPEEDR |=  ((3U << 20) | (3U << 22));

    /* AF4 */
    GPIOB->AFR[1] &= ~((0xFU << 8) | (0xFU << 12));

    GPIOB->AFR[1] |= (4U << 8) | (4U << 12);


    /* Reset I2C */
    I2C2->CR1 |= (1U << 15);
    I2C2->CR1 &= ~(1U << 15);


    /* APB1 = 42 MHz */
    I2C2->CR2 = 42;

    /* 100 kHz */
    I2C2->CCR = 210;

    /* 42 MHz + 1 */
    I2C2->TRISE = 43;

    /* Enable */
    I2C2->CR1 |= (1U << 0);
}
/*========================================================
 * I2C Start
 *========================================================*/
static void I2C_Start(void)
{
    LCD_I2C->CR1 |= (1U << 8);

    while (!(LCD_I2C->SR1 & (1U << 0)));
}
/*========================================================
 * Send Address
 *========================================================*/
static void I2C_SendAddress(uint8_t address)
{
    LCD_I2C->DR = address;

    while (!(LCD_I2C->SR1 & (1U << 1)));

		// read SR1 , SR2 to clear bit ADDR
    (void)LCD_I2C->SR1;
    (void)LCD_I2C->SR2;
}
/*========================================================
 * Send Data
 *========================================================*/
static void I2C_SendData(uint8_t data)
{
    while (!(LCD_I2C->SR1 & (1U << 7)));

    LCD_I2C->DR = data;

    while (!(LCD_I2C->SR1 & (1U << 2)));
}
/*========================================================
 * I2C Stop
 *========================================================*/
static void I2C_Stop(void)
{
    LCD_I2C->CR1 |= (1U << 9);
}
/*========================================================
 * Send one byte to PCF8574
 *========================================================*/
static void PCF8574_Write(uint8_t data)
{
    I2C_Start();

    I2C_SendAddress(LCD_I2C_ADDR);

    I2C_SendData(data);

    I2C_Stop();
}
/*========================================================
 * LCD Write 4 bits
 *========================================================*/
static void LCD_Write4Bits(uint8_t data)
{
    uint8_t temp;

    // EN = 1
    temp = data | LCD_EN;

    PCF8574_Write(temp);

    Delay_us(1);

    // EN = 0
    temp = data & ~LCD_EN;

    PCF8574_Write(temp);

    Delay_us(50);
}
/*========================================================
 * LCD Write byte
 *
 * rs = 0 -> command
 * rs = 1 -> data
 *========================================================*/
static void LCD_Write(uint8_t data, uint8_t rs)
{
    uint8_t high;
    uint8_t low;

    high = data & 0xF0;

    low = (data << 4) & 0xF0;

    high |= LCD_BACKLIGHT;

    low |= LCD_BACKLIGHT;

    if (rs)
    {
        high |= LCD_RS;
        low  |= LCD_RS;
    }

    LCD_Write4Bits(high);
    LCD_Write4Bits(low);
}


/*========================================================
 * LCD Send Command
 *========================================================*/
void LCD_SendCommand(uint8_t cmd)
{
    LCD_Write(cmd, 0);

    if (cmd == 0x01 || cmd == 0x02)
    {
        Delay_ms(2);
    }
}
/*========================================================
 * LCD Send Character
 *========================================================*/
void LCD_SendChar(char c)
{
    LCD_Write((uint8_t)c, 1);
}
/*========================================================
 * LCD Send String
 *========================================================*/
void LCD_SendString(char *str)
{
    while (*str)
    {
        LCD_SendChar(*str);
        str++;
    }
}
/*========================================================
 * LCD Send Number
 *========================================================*/
void LCD_SendNumber(int32_t number)
{
    char buffer[12];
    uint8_t i = 0;

    // Case 0
    if (number == 0)
    {
        LCD_SendChar('0');
        return;
    }

    // Case negative number
    if (number < 0)
    {
        LCD_SendChar('-');
        number = -number;
    }

    // Separate each digit
    while (number > 0)
    {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    // Inve Buffer
    while (i > 0)
    {
        LCD_SendChar(buffer[--i]);
    }
}
/*========================================================
 * LCD Send Float
 *========================================================*/
void LCD_SendFloat(float number, uint8_t decimal)
{
    uint32_t integer_part;
    uint32_t fractional_part;
    uint32_t multiplier = 1;
    uint8_t i;

    // Negative number
    if (number < 0.0f)
    {
        LCD_SendChar('-');
        number = -number;
    }

    // Calculate 10^decimal
    for (i = 0; i < decimal; i++)
    {
        multiplier *= 10;
    }

    // Integer part
    integer_part = (uint32_t)number;

    LCD_SendNumber((int32_t)integer_part);

    // If no need decimal
    if (decimal == 0)
    {
        return;
    }

    // '.'
    LCD_SendChar('.');

    // Calculate decimal part
    fractional_part = (uint32_t)(number * multiplier) - (integer_part * multiplier);

    multiplier /= 10;

    while (multiplier > 0)
    {
        LCD_SendChar(
            ((fractional_part / multiplier) % 10) + '0'
        );

        multiplier /= 10;
    }
}
/*========================================================
 * LCD Clear
 *========================================================*/
void LCD_Clear(void)
{
    LCD_SendCommand(0x01);

    Delay_ms(2);
}
/*========================================================
 * LCD Set Cursor
 *========================================================*/
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 0)
    {
        address = 0x00 + col;
    }
    else
    {
        address = 0x40 + col;
    }

    LCD_SendCommand(0x80 | address);
}


/*========================================================
 * LCD Initialization
 *========================================================*/
void LCD_Init(I2C_TypeDef *I2Cx)
{
    LCD_I2C = I2Cx;

    Delay_ms(50);

    LCD_Write4Bits(0x30 | LCD_BACKLIGHT);
    Delay_ms(5);

    LCD_Write4Bits(0x30 | LCD_BACKLIGHT);
    Delay_us(150);

    LCD_Write4Bits(0x30 | LCD_BACKLIGHT);
    Delay_us(150);

    LCD_Write4Bits(0x20 | LCD_BACKLIGHT);
    Delay_us(150);

    LCD_SendCommand(0x28);

    LCD_SendCommand(0x08);

    LCD_Clear();

    LCD_SendCommand(0x06);

    LCD_SendCommand(0x0C);
}

void LCD_CreateChar(uint8_t location, const uint8_t charmap[])
{
    uint8_t i;

    /*
     * LCD 1602 cho có 8 vi trí ky tu tuy chinh
     * location: 0 -> 7
     */
    location &= 0x07;

    /*
     * Set CGRAM address
     *
     * Moi ky tu chiem 8 byte
     */
    LCD_SendCommand(0x40 | (location << 3));

    // Ghi 8 byte du lieu vao CGRAM
    for (i = 0; i < 8; i++)
    {
        LCD_SendChar(charmap[i]);
    }
}

void LCD_WriteCustomChar(uint8_t location)
{
    location &= 0x07;

    LCD_SendChar(location);
}

void LCD_PrintAlign(uint8_t row, char *str, uint8_t align)
{
    uint8_t length = 0;
    uint8_t position = 0;

    // Tinh do dai chuoi
    while (str[length] != '\0')
    {
        length++;
    }

    // LCD 1602 co 16 ky tu moi dong
    if (length > 16)
    {
        length = 16;
    }

    // Tinh vi tri bat dau
    switch (align)
    {
        case LCD_ALIGN_LEFT:
            position = 0;
            break;

        case LCD_ALIGN_CENTER:
            position = (16 - length) / 2;
            break;

        case LCD_ALIGN_RIGHT:
            position = 16 - length;
            break;

        default:
            position = 0;
            break;
    }

    // Dua con tro den vi tri can in
    LCD_SetCursor(row, position);

    // In chuoi
    for (uint8_t i = 0; i < length; i++)
    {
        LCD_SendChar(str[i]);
    }
}
