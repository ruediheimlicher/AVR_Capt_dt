/*
    File:       lcd.h
    Version:    0.1.0
    Date:       Feb. 25, 2006
    
    C header file for HD44780 LCD module using a 74HCT164 serial in, parallel 
    out out shift register to operate the LCD in 8-bit mode.  Example schematic
    usuing a Powertip PC-1202A LCD available at http://www.micahcarrick.com
    
    Uses the avr-libc library for gcc (avr-gcc).
    
    ****************************************************************************
    Copyright (C) 2006 Micah Carrick   <email@micahcarrick.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ****************************************************************************
*/
#ifndef _LCD_H
#define _LCD_H

#include <inttypes.h>

/* AVR port and pins connected to '164 and/or LCD */
/*
#define LCD_PORT                PORTB
#define LCD_DDR						DDRB


#define LCD_RSDS_PIN            5
#define LCD_ENABLE_PIN          6
#define LCD_CLOCK_PIN           7
*/

#define LCD_PORT                PORTC
#define LCD_DDR						DDRC

#define LCD_RSDS_PIN            0
#define LCD_ENABLE_PIN          1
#define LCD_CLOCK_PIN           2

#define LCD_BACKLIGHT_PIN       7

/* number of columns on the display */
#define LCD_COLS 20

/* LCD commands */
#define LCD_CMD_HOME            0x02    /* cursor home */
#define LCD_CMD_ENTRY_DEC       0x04    /* decrement, no shift */
#define LCD_CMD_ENTRY_DEC_SHIFT 0x05    /* decrement with shift */
#define LCD_CMD_ENTRY_INC       0x06    /* increment, no shift */
#define LCD_CMD_ENTRY_INC_SHIFT 0x07    /* increment with shift */
#define LCD_CMD_CLEAR           0x01    /* clear, cursor home */
#define LCD_CMD_OFF             0x08    /* display off */
#define LCD_CMD_ON              0x0C    /* display on, cursor off */
#define LCD_CMD_ON_BLINK        0x0D    /* display on, cursor off, blink char */
#define LCD_CMD_ON_CURSOR       0x0E    /* display on, cursor on */
#define LCD_CMD_ON_CURSOR_BLINK 0x0F    /* display on, cursor on, blink char */
#define LCD_CMD_CURSOR_LEFT     0x10    /* decrement cursor (left) */
#define LCD_CMD_CURSOR_RIGHT    0x14    /* increment cursor (right) */
#define LCD_CMD_SHIFT_LEFT      0x18    /* shift (left) */
#define LCD_CMD_SHIFT_RIGHT     0x1C    /* shift (right)*/

#define LCD_LINE_1              0x80 /* bit 7 is always set: 0x80 = 0x00 */ 
#define LCD_LINE_2              0xC0 /* 2nd line at position 40 */ 

/* function set: (always 8-bit with the shift-register circuit I'm using */
#define LCD_FUNCTION_8x1        0x30   /* 5x7 characters, single line display */
#define LCD_FUNCTION_8x2        0x38   /* 5x7 characters, 2 line display */

//Fleury
#define LCD_LINES           2     /**< number of visible lines of the display */
#define LCD_DISP_LENGTH    20     /**< visibles characters per line of the display */
#define LCD_START_LINE1  0x00     /**< DDRAM address of first char of line 1 */
#define LCD_START_LINE2  0x40     /**< DDRAM address of first char of line 2 */
#define LCD_START_LINE3  0x14     /**< DDRAM address of first char of line 3 */
#define LCD_START_LINE4  0x54     /**< DDRAM address of first char of line 4 */

#define LCD_CGRAM             6      /* DB6: set CG RAM address             */
#define LCD_DDRAM             7      /* DB7: set DD RAM address             */



/* F_CPU is used by delay routines from util/delay.h. May already be defined */
#ifndef F_CPU
#define F_CPU 1000000UL  /* 1 MHz CPU clock */
#endif

/* functions */
void lcd_backlight(int);
void lcd_initialize(uint8_t, uint8_t, uint8_t);
void lcd_load_byte(uint8_t);
void lcd_send_cmd(void);
void lcd_send_char(void);
void lcd_strobe_E(void);
void lcd_cursor_home(void);
void lcd_move_cursor(uint8_t, uint8_t);
void lcd_inc_cursor(uint8_t);
void lcd_dec_cursor(uint8_t);
void lcd_putc(const char c);
void lcd_puts(const char *s);
void lcd_putint(uint8_t zahl);
void lcd_putint1(uint8_t zahl);
void lcd_putint2(uint8_t zahl);
void lcd_putint16(uint16_t zahl);
void lcd_puthex(uint8_t zahl);
void lcd_cls(void);
void lcd_clr_line(uint8_t Linie);
void lcd_put_zeit(uint8_t minuten, uint8_t stunden);
void lcd_put_wochentag(uint8_t wd);
void lcd_put_temperatur(uint16_t temperatur);
#endif
