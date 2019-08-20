/*
 * lcd_display.h
 *
 *  Created on: Aug 6, 2019
 *      Author: gcrisler
 */

#ifndef LCD_DISPLAY_H_
#define LCD_DISPLAY_H_


void    LcdInit(void);

void    LcdCycle(void);
unsigned int Dec2BCD(int);
unsigned long int Dec2BcdLong(unsigned long int);
void LcdDisplayTopRow(int);
void LcdDisplayMiddleRow(long int);
void LcdDisplayBottomRow(long int);


#endif /* LCD_DISPLAY_H_ */
