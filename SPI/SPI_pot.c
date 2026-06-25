#include "stm32f10x.h"
#include <stdio.h>

/* ================= 8-BIT LCD PIN MAPPING =================
RS -> PB0
EN -> PB1  (Note: Make sure RW on LCD is connected to GND)
D0 -> PB7
D1 -> PB8
D2 -> PB9
D3 -> PB10
D4 -> PB11
D5 -> PB12
D6 -> PB13
D7 -> PB14
============================================================ */

#define RS (1 << 0)
#define EN (1 << 1)

/* ================= DELAY ================= */
void delay(volatile uint32_t t)
{
    while(t--);
}

/* ================= LCD FUNCTIONS (8-Bit Mode) ================= */
void LCD_WRITE_BUS(unsigned char value)
{
    // Clear pins PB7 to PB14 (8 bits total)
    // 0xFF shifted left by 7 masks exactly bits 7,8,9,10,11,12,13,14
    GPIOB->ODR &= ~(0xFF << 7);
    
    // Shift our 8-bit data command to map to PB7-PB14
    GPIOB->ODR |= (value << 7);
    
    // Pulse Enable
    GPIOB->BSRR = EN;
    delay(2000);
    GPIOB->BRR = EN;
    delay(2000);
}

void LCD_CMD(unsigned char cmd)
{
    GPIOB->BRR = RS; // RS = 0 for command
    LCD_WRITE_BUS(cmd);
    delay(5000);
}

void LCD_DATA(unsigned char data)
{
    GPIOB->BSRR = RS; // RS = 1 for data
    LCD_WRITE_BUS(data);
    delay(5000);
}

void LCD_STRING(char *str)
{
    while(*str)
    {
        LCD_DATA(*str++);
    }
}

void LCD_INIT(void)
{
    RCC->APB2ENR |= (1 << 3); // Enable GPIOB clock

    // --- FIXING THE GPIO CONFIGURATION BLOCKS ---
    // GPIOB->CRL configuration:
    // PB0, PB1 (Control) -> Output Push-Pull (0x3)
    // PB2 to PB6 -> Keep as default Floating Input (0x4)
    // PB7 (LCD D0) -> Output Push-Pull (0x3)
    GPIOB->CRL = 0x34444433; 

    // GPIOB->CRH configuration:
    // PB8 to PB14 (LCD D1-D7) -> Output Push-Pull (0x3)
    // PB15 -> Keep as default Floating Input (0x4)
    GPIOB->CRH = 0x43333333; 

    delay(40000);

    LCD_CMD(0x38); // 8-bit mode, 2-line display, 5x7 font
    LCD_CMD(0x0C); // Display ON, Cursor OFF
    LCD_CMD(0x06); // Entry mode: increment cursor
    LCD_CMD(0x01); // Clear display
    delay(40000);
}

/* ================= SPI1 INITIALIZATION ================= */
void SPI1_INIT(void)
{
    RCC->APB2ENR |= (1 << 2);   // GPIOA clock
    RCC->APB2ENR |= (1 << 12);  // SPI1 clock

    // PA4 = Output Push-Pull (CS) -> 0x3
    // PA5 = Alt-Function Output Push-Pull (SCK) -> 0xB
    // PA6 = Input Floating (MISO) -> 0x4
    // PA7 = Alt-Function Output Push-Pull (MOSI) -> 0xB
    GPIOA->CRL &= ~(0xFFFFFFFF);
    GPIOA->CRL |=  (0xB4B34444); 
    
    GPIOA->BSRR = (1 << 4);     // Pull CS high initially

    // SPI Configure: Master Mode, Prescaler fclk/32, Software Slave Management
    SPI1->CR1 = (1 << 2) | (1 << 4) | (1 << 9) | (1 << 8);
    SPI1->CR1 |= (1 << 6);      // Enable SPI1
}

/* ================= SPI FUNCTIONS ================= */
unsigned char SPI_TRANSFER(unsigned char data)
{
    while(!(SPI1->SR & (1 << 1))); // Wait for TXE (Transmit Empty)
    SPI1->DR = data;
    while(!(SPI1->SR & (1 << 0))); // Wait for RXNE (Receive Complete)
    return SPI1->DR;
}

/* ================= MCP3008 ADC READ ================= */
unsigned int MCP3008_READ(unsigned char channel)
{
    unsigned char b1, b2;
    unsigned int result;

    GPIOA->BRR = (1 << 4); // CS LOW

    SPI_TRANSFER(0x01); // Start bit
    b1 = SPI_TRANSFER((0x08 | channel) << 4); // Single-ended control bit selection
    b2 = SPI_TRANSFER(0x00); // Read final data byte

    GPIOA->BSRR = (1 << 4); // CS HIGH

    result = ((b1 & 0x03) << 8) | b2;
    return result;
}

/* ================= MAIN APPLICATION ================= */
int main(void)
{
    unsigned int adc;
    char buffer[16];

    LCD_INIT();
    SPI1_INIT();

    while(1)
    {
        adc = MCP3008_READ(0); // Query channel 0

        LCD_CMD(0x80); // Line 1
        sprintf(buffer, "ADC: %4d     ", adc);
        LCD_STRING(buffer);

        LCD_CMD(0xC0); // Line 2
        sprintf(buffer, "V:   %.2fV    ", (adc * 3.3) / 1023.0);
        LCD_STRING(buffer);

        delay(800000); // Stable tracking refresh rate
    }
}
