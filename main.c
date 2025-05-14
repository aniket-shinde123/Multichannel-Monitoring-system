#include <xc.h>
#include<stdio.h>
#include"DHT_TIMER.h"
#include"LCD_DEC.h"
#include"DHT_TEMP_READ.h"
#pragma config OSC=HS
#pragma PBADEN=OFF
#define _XTAL_FREQ 20000000
volatile unsigned char DHT_Read_flag=0,channel_number,i=0,j=0,UART_Channel_string[]="Enter Channel Number";
volatile unsigned char Timer0_Flag=0,Temp_string[5]={'\0'};
volatile unsigned int Algo_Array[5],Greatest_Algo_Array_Number=0,x,Algo_Array_Flag=0;
volatile unsigned char DHT_HUMIDITY=0,DHT_TEMP=0;
void Data_Send(void);
void System_Init();
void Pic_UART_ESP32(void);
void main(void) 
{
    unsigned char Dis_Flag=0,i;
    ADCON1=0x0f;
    System_Init();
    lcdstring("MULTI - CHANNEL  TEMP DISPLAY  ");
    __delay_ms(1000);
    lcdcmd(0x01);
    lcdstring("  INTERRUPT TO   SELECT CHANNEL ");
    while(1)
    {
        if(DHT_Read_flag==1)
        {
            if(channel_number>=1&&channel_number<=2)
            {
                ADC_TEMP_READ(channel_number);
                Dis_Flag=1;
            }
            else
            {
                lcdcmd(0x01);
                lcdstring(" WRONG CHANNEL     SELECTION   ");
                INT0IE=1;
            }
            DHT_Read_flag=0;
        }
        if(Dis_Flag==1)
        {
            lcdcmd(0x01);
            switch(channel_number)
            {
                case 1 :
                {
                    Data_Send();
                    Timer0_Flag=0;
                    Algo_Array[0x00]+=1;
                    Algo_Array_Flag++;
                    break;
                }
                case 2 :
                {
                    Data_Send();
                    Timer0_Flag=0;
                    Algo_Array[0x01]+=1;
                    Algo_Array_Flag++;
                    break;
                }
                case 3 :
                {
                    Data_Send();
                    Timer0_Flag=0;
                    Algo_Array[0x02]+=1;
                    Algo_Array_Flag++;
                    break;
                }
                case 4 :
                {
                    Data_Send();
                    Timer0_Flag=0;
                    Algo_Array[0x03]+=1;
                    Algo_Array_Flag++;
                    break;
                }
                case 5 :
                {
                    Data_Send();
                    Timer0_Flag=0;
                    Algo_Array[0x04]+=1;
                    Algo_Array_Flag++;
                    break;
                } 
            }
            INT0IE=1;
        }
        Dis_Flag=0;
        if(Algo_Array_Flag>=5)
        {
            INT0IE=0;
            TMR0ON=0;
            ADC_SORT_DISPLAY();
            INT0IE=1;
            TMR0ON=1;
        }
    }
    return ;
}
__interrupt(high_priority)ISR(void)
{
    if(TMR0IF==1)
    {
        TMR0IF=0;
        TMR0ON=0;
        Timer0_Flag+=1;
        TMR0H=0x67;
        TMR0L=0x69;
        TMR0ON=1;
    }
    if(INT0IF==1)
    {
        __delay_ms(50);
        lcdcmd(0x01);
        lcdstring("   ENTER ADC    CHANNEL NUMBER");
        SPEN=1;
        BRGH=0;
        BRG16=0;
        SPBRG=31;
        RCIE=1;
        TXIE=1;
        INT0IF=0;
        TXEN=1;
    }
    if(TXIF==1)
    {
        TXREG=UART_Channel_string[j++];
        if(UART_Channel_string[j]=='\0')
        {
            while(TRMT==0);
            TXEN=0;
            CREN=1;
            j=0;
        }
    }
    if(RCIF==1)
    {
        channel_number=RCREG;
        while(RCIDL==0);
        channel_number-=48;
        CREN=0;
        RCIE=0;
        DHT_Read_flag=1;
    }
}
void System_Init()
{
    GIE=1;
    INTEDG0=1;//falling Edge
    PEIE=1;
    INT0IE=1;
    lcdstart();
    ADC_TIMER0_INIT();
}
void Pic_UART_ESP32(void)
{
    /*TXIE=0;
    TXEN=1;
    TXREG='#';
    while(TRMT==0);
    __delay_ms(10);
    TXREG=DHT_TEMP;
    while(TRMT==0);
    __delay_ms(10);
    TXREG='&';
    while(TRMT==0);
    __delay_ms(10);
    TXREG=DHT_HUMIDITY;
    while(TRMT==0);
    TXEN=0;
    TXIE=1;*/
    unsigned char i;
    TXIE=0;
    TXEN=1;
    i=0;
    while(Temp_string[i])
    {
        TXREG=Temp_string[i];
        i++;
       while(TRMT==0);
    }
    TXEN=0;
}
void Data_Send(void)
{
    //Pic_UART_ESP32();
    lcdstring("HUMIDITY = ");
    sprintf(Temp_string,"%u",DHT_HUMIDITY);
    //Pic_UART_ESP32();
    DHT_HUMIDITY=0;
    lcdstring(Temp_string);
    lcddata('%');
    lcdcmd(0xC0);
    lcdstring("TEMP = ");
    sprintf(Temp_string,"%u",DHT_TEMP);
    //Pic_UART_ESP32();
    DHT_TEMP=0;
    lcdstring(Temp_string);
    lcddata('c');
}
