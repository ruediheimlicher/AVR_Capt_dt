// vim:sw=8:ts=8:si:et: 
/*********************************************************************************
Title:    DS18X20-Functions via One-Wire-Bus
Author:   Martin Thomas <eversmith@heizung-thomas.de>   
          http://www.siwawi.arubi.uni-kl.de/avr-projects
          
          Modifications and updates by Guido Socher, June 2009
          The code was also reduced to only the needed function for
          this application.
          http://tuxgraphics.org/electronics/
Copyright: GPL

Partly based on code from Peter Dannegger and others

Extended measurements for DS18(S)20 contributed by Carsten Foss (CFO)

**********************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>

uint8_t Tastenwahl(uint8_t Tastaturwert)
{
   //lcd_gotoxy(0,1);
   //lcd_putint(Tastaturwert);
   if (Tastaturwert < TASTE1)
      return 1;
   if (Tastaturwert < TASTE2)
      return 2;
   if (Tastaturwert < TASTE3)
      return 3;
   if (Tastaturwert < TASTE4)
      return 4;
   if (Tastaturwert < TASTE5)
      return 5;
   if (Tastaturwert < TASTE6)
      return 6;
   if (Tastaturwert < TASTE7)
      return 7;
   if (Tastaturwert < TASTE8)
      return 8;
   if (Tastaturwert < TASTE9)
      return 9;
   if (Tastaturwert < TASTEL)
      return 10;
   if (Tastaturwert < TASTE0)
      return 0;
   if (Tastaturwert < TASTER)
      return 12;
   
   return -1;
}








// convert fraction bits to decimal (0-9)
// This number represents one digit behind the decimal point.
// cel_frac_bits is a number from 0 to 15
// cel_frac_bits   meaning  returnValue
//   0             0.0        0
//   1             0.0625     1
//   2             0.1250     1
//   3             0.1875     2
//   4             0.2500     2
//   5             0.3125     3
//   6             0.3750     4
//   7             0.4375     4
//   8             0.5000     5
//   9             0.5625     5
//   10            0.6250     6
//   11            0.6875     7
//   12            0.7500     7
//   13            0.8125     8
//   14            0.8750     9
//   15            0.9375     9
//   Note this rouding makes sense because the accuracy of
//   the sensor is anyhow only 0.5'C
uint8_t DS18X20_frac_bits_decimal(uint8_t cel_frac_bits)
{
        uint8_t d;
        d=cel_frac_bits*6 + 5;
        if (cel_frac_bits>12) d+=3;
        return(d/10);
}

