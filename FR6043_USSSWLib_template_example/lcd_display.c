/*
 * lcd_display.c
 *
 *  Created on: Aug 7, 2019
 *      Author: gcrisler
 */

//#include "driverlib.h"
#include "lcd_display.h"
unsigned int gary = 23;

const char middle_row[2] = { 0x77,0x23};

const char top_row[10]={
                        0x77, //0
                        0x12, //1
                        0xE3, //2
                        0xD3, //3
                        0x96, //4
                        0x55, //5
                        0xF5, //6
                        0x13, //7
                        0xF7, //8
                        0x97}; //9


 char buffer[10];
 unsigned int bcdreturn;
 unsigned int j,k;

 unsigned int Dec2BCD(unsigned int Value)
 {
 unsigned int i;
 unsigned int Output;
 for (i = 16, Output = 0; i; i--) // BCD Conversion, 16-Bit
         {
             Output = __bcd_add_short(Output, Output);
             if (Value & 0x8000)
             Output = __bcd_add_short(Output, 1);
             Value <<= 1;
         }
     return Output;
 }

char* itoa(int value, char* result)
    {

            char* ptr = result, *ptr1 = result, tmp_char;
            int tmp_value;

            do {
                tmp_value = value;
                value /= 10;
                *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * 10)];
                } while ( value );

    // Apply negative sign

        *ptr-- = '\0';
        while(ptr1 < ptr)
            {
            tmp_char = *ptr;
            *ptr--= *ptr1;
            *ptr1++ = tmp_char;
            }
        return result;
    }

