#include "stm32f10x.h"


//================ DELAY =================

void delay()
{
    unsigned int i;

    for(i=0;i<50000;i++);
}



//================ LCD ==================

#define RS (1<<10)
#define EN (1<<11)


void LCD_Command(unsigned char cmd)
{

    GPIOA->ODR &= ~RS;

    GPIOA->ODR &= 0xFFFFFF00;

    GPIOA->ODR |= cmd;


    GPIOA->BSRR = EN;

    delay();

    GPIOA->BRR = EN;

}



void LCD_Data(unsigned char data)
{

    GPIOA->ODR |= RS;


    GPIOA->ODR &= 0xFFFFFF00;

    GPIOA->ODR |= data;


    GPIOA->BSRR = EN;

    delay();

    GPIOA->BRR = EN;

}



void LCD_String(char *str)
{

    while(*str)
    {
        LCD_Data(*str);

        str++;
    }

}



void LCD_Init()
{

    RCC->APB2ENR |= (1<<2);


    GPIOA->CRL = 0x33333333;


    GPIOA->CRH &= ~(0xFF00);

    GPIOA->CRH |= 0x3300;


    delay();


    LCD_Command(0x38);

    LCD_Command(0x0C);

    LCD_Command(0x01);

}



//================ SOFTWARE I2C =================


#define SDA_HIGH GPIOB->BSRR=(1<<7)
#define SDA_LOW  GPIOB->BRR=(1<<7)


#define SCL_HIGH GPIOB->BSRR=(1<<6)
#define SCL_LOW  GPIOB->BRR=(1<<6)




void I2C_Init()
{

    RCC->APB2ENR |= (1<<3);



    // PB6 PB7 output

    GPIOB->CRL &= 0x00FFFFFF;

    GPIOB->CRL |= 0x33000000;


    SDA_HIGH;

    SCL_HIGH;

}



void I2C_Start()
{

    SDA_HIGH;

    SCL_HIGH;

    delay();


    SDA_LOW;

    delay();


    SCL_LOW;

}



void I2C_Stop()
{

    SDA_LOW;

    SCL_HIGH;

    delay();


    SDA_HIGH;

}



void I2C_Write(unsigned char data)
{

    unsigned int i;


    for(i=0;i<8;i++)
    {

        if(data & 0x80)

            SDA_HIGH;

        else

            SDA_LOW;



        SCL_HIGH;

        delay();



        SCL_LOW;



        data <<= 1;

    }


    // ACK clock

    SDA_HIGH;

    SCL_HIGH;

    delay();

    SCL_LOW;

}




unsigned char I2C_Read()
{

    unsigned char data=0;

    unsigned int i;


    SDA_HIGH;



    for(i=0;i<8;i++)
    {

        SCL_HIGH;

        delay();


        data <<=1;



        if(GPIOB->IDR & (1<<7))

            data |=1;



        SCL_LOW;

    }



    return data;

}



//================ DS1307 =================


#define DS1307_WRITE 0xD0

#define DS1307_READ  0xD1



unsigned char BCD2DEC(unsigned char val)
{

    return ((val>>4)*10)+(val&0x0F);

}





unsigned char DS1307_Read(unsigned char reg)
{

    unsigned char data;



    I2C_Start();



    I2C_Write(DS1307_WRITE);



    I2C_Write(reg);



    I2C_Start();



    I2C_Write(DS1307_READ);



    data=I2C_Read();



    I2C_Stop();



    return data;

}



// Use this only once to set time

void DS1307_SetTime()
{

    I2C_Start();


    I2C_Write(DS1307_WRITE);


    I2C_Write(0x00);



    I2C_Write(0x00);   // seconds

    I2C_Write(0x30);   // minutes

    I2C_Write(0x14);   // hours 14:30:00



    I2C_Stop();

}



//================ MAIN =================


int main()
{

    unsigned char hour;

    unsigned char minute;

    unsigned char second;



    SystemInit();



    LCD_Init();



    I2C_Init();



    // Uncomment only first time
    // DS1307_SetTime();



    LCD_String("DS1307 RTC");



    while(1)
    {


        second = BCD2DEC(DS1307_Read(0x00));


        minute = BCD2DEC(DS1307_Read(0x01));


        hour = BCD2DEC(DS1307_Read(0x02));



        LCD_Command(0xC0);



        LCD_Data((hour/10)+'0');

        LCD_Data((hour%10)+'0');



        LCD_Data(':');



        LCD_Data((minute/10)+'0');

        LCD_Data((minute%10)+'0');



        LCD_Data(':');



        LCD_Data((second/10)+'0');

        LCD_Data((second%10)+'0');



        delay();



    }

}
