/*
 * lcd_display.c
 *
 *  Created on: Aug 7, 2019
 *      Author: gcrisler
 */
#include <msp430.h>
#include "ussSwLib.h"
#include "driverlib.h"
//#include "USS_Config/USS_userConfig.h"

#include "lcd_display.h"

const char top_row[10]={
                        0x77, //0
                        0x12, //1
                        0xE3, //2
                        0xD3, //3
                        0x96, //4
                        0xD5, //5
                        0xF5, //6
                        0x13, //7
                        0xF7, //8
                        0x97}; //9

const char middle_row[10]={
                        0xEE, //0
                        0xC0, //1
                        0x5E, //2
                        0xDA, //3
                        0xF0, //4
                        0xBA, //5
                        0xBE, //6
                        0xC8, //7
                        0xFE, //8
                        0xF8}; //9

 char buffer[10];
 unsigned int bcdreturn;
 unsigned int j,k;

 extern void LcdInit()
     {
         LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_0, LCD_C_SEGMENT_LINE_7);
         LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_9, LCD_C_SEGMENT_LINE_21);

         LCD_C_initParam initParams = {0};
         initParams.clockSource = LCD_C_CLOCKSOURCE_ACLK;
         initParams.clockDivider = LCD_C_CLOCKDIVIDER_15;
         initParams.clockPrescalar = LCD_C_CLOCKPRESCALAR_4;
         initParams.muxRate = LCD_C_8_MUX;
         initParams.waveforms = LCD_C_STANDARD_WAVEFORMS;
         initParams.segments = LCD_C_SEGMENTS_ENABLED;

         LCD_C_init(LCD_C_BASE, &initParams);

         // LCD Operation - VLCD generated internally, V2-V4 generated internally, v5 to ground
         LCD_C_setVLCDSource(LCD_C_BASE, LCD_C_VLCD_GENERATED_INTERNALLY, LCD_C_V2V3V4_GENERATED_INTERNALLY_NOT_SWITCHED_TO_PINS,
              LCD_C_V5_VSS);

         // Set VLCD voltage to 2.60v
         LCD_C_setVLCDVoltage(LCD_C_BASE, LCD_C_CHARGEPUMP_VOLTAGE_2_90V_OR_2_42VREF);

         // Enable charge pump and select internal reference for it
         LCD_C_enableChargePump(LCD_C_BASE);
         LCD_C_selectChargePumpReference(LCD_C_BASE, LCD_C_INTERNAL_REFERENCE_VOLTAGE);

         LCD_C_configChargePump(LCD_C_BASE, LCD_C_SYNCHRONIZATION_ENABLED, 0);

         // Clear LCD memory
         LCD_C_clearMemory(LCD_C_BASE);
     }

 void LcdDisplayBottomRow(long int incoming)
          {
           long int temp = 0;
           temp = Dec2BcdLong(incoming);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_7, top_row[(temp & 0xf0000000) >>28]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_6, top_row[(temp & 0xf000000) >>24]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_5, top_row[(temp & 0xf00000) >>20]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_4, top_row[(temp & 0xf0000) >>16]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_3, top_row[(temp & 0xf000) >>12]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_2, top_row[(temp & 0x0f00) >>8]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_1, top_row[(temp & 0xf0) >>4]);
           LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_0, top_row[temp & 0x0f]);
           LCD_C_on(LCD_C_BASE);    // Turn LCD on
          }

 void LcdDisplayMiddleRow(long int incoming)
         {
          long int temp = 0;
          temp = Dec2BcdLong(incoming);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_20, middle_row[(temp & 0xf0000000) >>28]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_19, middle_row[(temp & 0xf000000) >>24]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_9, middle_row[(temp & 0xf00000) >>20]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_10, middle_row[(temp & 0xf0000) >>16]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_11, middle_row[(temp & 0xf000) >>12]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_12, middle_row[(temp & 0x0f00) >>8]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_13, middle_row[(temp & 0xf0) >>4]);
          LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_14, middle_row[temp & 0x0f]);
          LCD_C_on(LCD_C_BASE); // Turn LCD on
         }

  void LcdDisplayTopRow(int incoming)
        {
         int temp = 0;
         temp = Dec2BCD(incoming);
         LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_18, top_row[(temp & 0xf000) >>12]);
         LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_17, top_row[(temp & 0x0f00) >>8]);
         LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_16, top_row[(temp & 0xf0) >>4]);
         LCD_C_setMemory(LCD_C_BASE, LCD_C_SEGMENT_LINE_15, top_row[temp & 0x0f]);
         LCD_C_on(LCD_C_BASE);  // Turn LCD on
        }

unsigned long int Dec2BcdLong(unsigned long int value)
      {
      unsigned int i;
      unsigned long int Output;
      for (i = 32, Output= 0; i; i--)// BCD Conversion,32-Bit
          {
           Output= __bcd_add_long(Output,Output);
           if (value& 0x80000000) Output= __bcd_add_long(Output,1);
           value<<= 1;
          }return Output;
      }

unsigned int Dec2BCD(int value)
     {
        unsigned int i;
        unsigned int Output;
        for (i = 16, Output = 0; i; i--) // BCD Conversion, 16-Bit
             {
             Output = __bcd_add_short(Output, Output);
             if (value & 0x8000) Output = __bcd_add_short(Output, 1);
             value <<= 1;
             } return Output;
     }

extern char* itoa(int value, char* result)
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

 extern void  LcdCycle()
        {
           unsigned int i;
            for(i = 0; i < 21; i++)
                {
                    LCD_C_setMemory(LCD_C_BASE, i, 0xd6);
//                    USS_generateLPMDelay(&gUssSWConfig, USS_low_power_mode_option_low_power_mode_3, 15000);
                }
        }

