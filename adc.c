/*
 *  adc.c
 *  TWI_Master
 *
 *  Created by Sysadmin on 12.11.07.
 *  Copyright 2007 Ruedi Heimlicher. All rights reserved.
 *
 */

#include "adc.h"
#include <avr/io.h>


struct adcwert16 ADCWert16;


struct adcwert16 readKanal16Bit(uint8_t kanal)
{
 uint8_t i;
 struct adcwert16 tempWert;
 tempWert.wertH=0;
 tempWert.wertL=0;
  tempWert.wert8H=0;
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);    // Frequenzvorteiler auf 32 setzen und ADC aktivieren 
 
  ADMUX = kanal;                      // Ÿbergebenen Kanal waehlen
//  ADMUX |= (1<<REFS1) | (1<<REFS0); // interne Referenzspannung nutzen 
  ADMUX |=  (1<<REFS0); // VCC als Referenzspannung nutzen 
 
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);              // eine ADC-Wandlung (Der ADC setzt dieses Bit ja wieder auf 0 nach dem Wandeln)
  while ( ADCSRA & (1<<ADSC) ) {
     ;     // auf Abschluss der Wandlung warten 
  }
    ADCSRA |= (1<<ADSC);            // eine Wandlung
    while ( ADCSRA & (1<<ADSC) ) {
      ;     // auf Abschluss der Wandlung warten 
    }
	
	 tempWert.wertL=ADCL;            //Read 8 low bits first (important)
	 tempWert.wertH=ADCH;
	 tempWert.wert8H=(ADCW<<2);
	 
// value|=((int)ADCH << 8); //read 2 high bits and shift into top byte

 ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren ("Enable-Bit" auf LOW setzen)
 
 return tempWert;
}

void initADC(uint8_t derKanal)
{
   ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);    // Frequenzvorteiler auf 32 setzen und ADC aktivieren 
 
  ADMUX = derKanal;                      // Ÿbergebenen Kanal waehlen

	//ADMUX |= (1<<REFS1) | (1<<REFS0); // interne Referenzspannung nutzen 
  //if (test)
  {
  //ADMUX |= (1<<REFS0); // VCC als Referenzspannung nutzen 
  }
 
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);              // eine ADC-Wandlung (Der ADC setzt dieses Bit ja wieder auf 0 nach dem Wandeln)
  while ( ADCSRA & (1<<ADSC) ) {
     ;     // auf Abschluss der Wandlung warten 
  }
}

uint16_t readKanal(uint8_t derKanal) //Unsere Funktion zum ADC-Channel aus lesen
{
  uint8_t i;
  uint16_t result = 0;         //Initialisieren wichtig, da lokale Variablen
                               //nicht automatisch initialisiert werden und
                               //zufŠllige Werte haben. Sonst kann Quatsch rauskommen
 ADMUX = derKanal; 
  // Eigentliche Messung - Mittelwert aus 4 aufeinanderfolgenden Wandlungen
  for(i=0;i<4;i++)
  {
    ADCSRA |= (1<<ADSC);            // eine Wandlung
    while ( ADCSRA & (1<<ADSC) ) {
      ;     // auf Abschluss der Wandlung warten 
    }
    result += ADCW;            // Wandlungsergebnisse aufaddieren
  }
//  ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren ("Enable-Bit" auf LOW setzen)
 
  result /= 4;                     // Summe durch vier teilen = arithm. Mittelwert
 
  return result;
}

void closeADC()
{
ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren ("Enable-Bit" auf LOW setzen)
}

uint16_t readKanalOrig(uint8_t derKanal, uint8_t num) //Unsere Funktion zum ADC-Channel aus lesen
{
  uint8_t i;
  uint16_t result = 0;         //Initialisieren wichtig, da lokale Variablen
                               //nicht automatisch initialisiert werden und
                               //zufŠllige Werte haben. Sonst kann Quatsch rauskommen
 
   ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);    // Frequenzvorteiler auf 32 setzen und ADC aktivieren 
 
  ADMUX = derKanal;                      // Ÿbergebenen Kanal waehlen
//  ADMUX |= (1<<REFS1) | (1<<REFS0); // interne Referenzspannung nutzen 
//  ADMUX |= (1<<REFS0); // VCC als Referenzspannung nutzen 
 
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
  ADCSRA |= (1<<ADSC);              // eine ADC-Wandlung (Der ADC setzt dieses Bit ja wieder auf 0 nach dem Wandeln)
  while ( ADCSRA & (1<<ADSC) ) {
     ;     // auf Abschluss der Wandlung warten 
  }
 
  // Eigentliche Messung - Mittelwert aus 4 aufeinanderfolgenden Wandlungen
  for(i=0;i<4;i++)
  {
    ADCSRA |= (1<<ADSC);            // eine Wandlung
    while ( ADCSRA & (1<<ADSC) ) {
      ;     // auf Abschluss der Wandlung warten 
    }
    result += ADCW;            // Wandlungsergebnisse aufaddieren
  }
  ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren ("Enable-Bit" auf LOW setzen)
 
  result /= 4;                     // Summe durch vier teilen = arithm. Mittelwert
 
  return result;
}
