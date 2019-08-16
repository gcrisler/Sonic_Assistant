/*
 * lcd_display.c
 *
 *  Created on: Aug 7, 2019
 *      Author: gcrisler
 */

#include "driverlib.h"
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

 void initLcd()
     {
         LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_0, LCD_C_SEGMENT_LINE_21);
         LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_26, LCD_C_SEGMENT_LINE_43);

         LCD_C_initParam initParams = {0};
         initParams.clockSource = LCD_C_CLOCKSOURCE_ACLK;
         initParams.clockDivider = LCD_C_CLOCKDIVIDER_1;
         initParams.clockPrescalar = LCD_C_CLOCKPRESCALAR_16;
         initParams.muxRate = LCD_C_4_MUX;
         initParams.waveforms = LCD_C_LOW_POWER_WAVEFORMS;
         initParams.segments = LCD_C_SEGMENTS_ENABLED;

         LCD_C_init(LCD_C_BASE, &initParams);

         // LCD Operation - VLCD generated internally, V2-V4 generated internally, v5 to ground
         LCD_C_setVLCDSource(LCD_C_BASE, LCD_C_VLCD_GENERATED_INTERNALLY, LCD_C_V2V3V4_GENERATED_INTERNALLY_NOT_SWITCHED_TO_PINS,
              LCD_C_V5_VSS);

         // Set VLCD voltage to 2.60v
         LCD_C_setVLCDVoltage(LCD_C_BASE, LCD_C_CHARGEPUMP_VOLTAGE_2_60V_OR_2_17VREF);

         // Enable charge pump and select internal reference for it
         LCD_C_enableChargePump(LCD_C_BASE);
         LCD_C_selectChargePumpReference(LCD_C_BASE, LCD_C_INTERNAL_REFERENCE_VOLTAGE);

         LCD_C_configChargePump(LCD_C_BASE, LCD_C_SYNCHRONIZATION_ENABLED, 0);

         // Clear LCD memory
         LCD_C_clearMemory(LCD_C_BASE);

     }

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

