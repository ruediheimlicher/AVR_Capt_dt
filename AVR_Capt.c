//
//  Tastenblinky.c
//  Tastenblinky
//
//  Created by Sysadmin on 03.10.07.
//  Copyright Ruedi Heimlihcer 2007. All rights reserved.
//



#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/sleep.h>
#include <inttypes.h>
//#define F_CPU 4000000UL  // 4 MHz
#include <avr/delay.h>
#include "lcd.c"
#include "adc.c"
#include "utils.c"

#include "defines.h"
uint16_t loopCount0=0;
uint16_t loopCount1=0;
uint16_t loopCount2=0;

/*
#define TWI_PORT		PORTC
#define TWI_PIN		PINC
#define TWI_DDR		DDRC

#define SDAPIN		4
#define SCLPIN		5
*/


volatile uint8_t					Programmstatus=0x00;
uint8_t Tastenwert=0;
uint8_t TastaturCount=0;
volatile uint16_t					Manuellcounter=0; // Countr fuer Timeout	
uint16_t TastenStatus=0;
uint16_t Tastencount=0;
uint16_t Tastenprellen=0x01F;





void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
	// we use a calibrated macro. This is more
	// accurate and not so much compiler dependent
	// as self made code.
	while(ms){
		_delay_ms(0.96);
		ms--;
	}
}

// ACD https://www.avrprogrammers.com/howto/attiny-comparator
#define COMP_PORT PORTB
#define COMP_DDR DDRB

// Pins fuer Drive der RC
#define COMP_DRIVE_PIN_A  1
#define COMP_DRIVE_PIN_B  2

#define COMP_ADC_PORT PORTC
#define COMP_ADC_DDR DDRC

#define COMP_ADC_PIN_A  4
#define COMP_ADC_PIN_B  5

#define COMP_AIN_PORT   PORTD
#define COMP_AIN_DDR    DDRD
#define COMP_AIN0       6
#define COMP_AIN1       7


#define MULTIPLEX 1

volatile uint16_t captured_value;
volatile uint8_t captured;
volatile uint8_t overflow=0;
volatile uint8_t captAcounter=0;
volatile uint8_t captBcounter=0;
volatile uint16_t mittelwertA[4];
volatile uint16_t mittelwertB[4];
volatile uint8_t mposA=0;
volatile uint8_t mposB=0;
volatile uint8_t adckanal=0;
volatile uint16_t a=0;

volatile uint32_t waitcounter=0;
// end ACD

void timer1_comp(void)
{
   // Set pin for driving resistor low.
   COMP_DDR |= (1<<COMP_DRIVE_PIN_A);
   COMP_PORT &= ~(1<<COMP_DRIVE_PIN_A);
   COMP_DDR |= (1<<COMP_DRIVE_PIN_B);
   COMP_PORT &= ~(1<<COMP_DRIVE_PIN_B);
   
   // Disable the digital input buffers.
   //   DIDR = (1<<AIN1D) | (1<<AIN0D);
   //if (MULTIPLEX)
   {
      // ADC-Eingaenge fuer Capt
      COMP_ADC_DDR &= ~(1<<COMP_ADC_PIN_A);
      COMP_ADC_PORT &= ~(1<<COMP_ADC_PIN_A);
      
      COMP_ADC_DDR &= ~(1<<COMP_ADC_PIN_B);
      COMP_ADC_PORT &= ~(1<<COMP_ADC_PIN_B);
      
      // AIN0, AIN1 Eingang
      COMP_AIN_DDR &= ~(1<<COMP_AIN0);
      COMP_AIN_DDR &= ~(1<<COMP_AIN1);
      
      
      SFIOR |= (1<<ACME);
      //ADMUX = 3;
   }
   
   
   //ADCSRA =0;//| = (1<<ADEN);                    // disable ADC if necessary
   ACSR =   (1<<ACIC) | (1<<ACIS1) | (1<<ACIS0);   // Comparator enabled, no bandgap, input capture.
   
   
   
   
   // Timer...
   TCCR1A = 0;
   
   TCCR1B =   (1<<CS10);                        // F_CPU / 1
   TCCR1B |=  (1<<ICES1);                      // Input capture on rising edge
   
   TCNT1 = 0;
   
   
   TIMSK |= (1<<TOIE1) | (1<<TICIE1);           // Timer interrupts on capture and overflow.
}

ISR(TIMER1_CAPT_vect)
{
   //OSZIA_HI;
   //COMP_PORT &= ~(1<<COMP_DRIVE_PIN_A);
   // Save the captured value and drop the drive line.
   if (captured == 1)
   {
      ;
   }

   if (captured == 0)
   {
            // captured_value = ICR1;
      
      
      // Oberer Wert?
        if (adckanal == COMP_ADC_PIN_B)
      {
         captBcounter++;
         
         mittelwertB[mposB++] = ICR1;
         mposB &= 0x03;
         COMP_PORT &= ~(1<<COMP_DRIVE_PIN_A);
      }
      
      // unterer Wert
      if (adckanal == COMP_ADC_PIN_A)
      {
         OSZIA_LO;
         a = ICR1;
         captAcounter++;
         mittelwertA[mposA++] = ICR1;           // Ringbuffer fuer gleitenden Mittelwert
         mposA &= 0x03;                         // position incrementieren
         COMP_PORT &= ~(1<<COMP_DRIVE_PIN_A);   // auf 4 beschraenken
      
   //      adckanal = COMP_ADC_PIN_B;

      }
      OSZIA_HI;
       
      TCNT1 = 0;
      captured = 1;
   }
   //TCNT1 = 0;
}

ISR(TIMER1_OVF_vect)
{
   overflow++;
   COMP_PORT &= ~(1<<COMP_DRIVE_PIN_A);
   COMP_PORT &= ~(1<<COMP_DRIVE_PIN_B);
   // If we overflowed, the capacitor is bigger than
   // this range supports. Use a smaller series resistor.
}

void switchChannel (uint8_t channel) // https://github.com/Teknoman117/avr/blob/master/unsorted/mxx4lib/acomp.h
{ //switched the negative input pin of the Analog comparator in multiplexer mode
   ADMUX = channel & 0x07;     //set the multiplexer channel and mask unused buts
   __asm ("NOP");              //wait 2 clock cycles
   __asm ("NOP");
}


void slaveinit(void)
{
	//LCD
	LCD_DDR |= (1<<LCD_RSDS_PIN);	//Pin 5 von PORT B als Ausgang fuer LCD
 	LCD_DDR |= (1<<LCD_ENABLE_PIN);	//Pin 6 von PORT B als Ausgang fuer LCD
	LCD_DDR |= (1<<LCD_CLOCK_PIN);	//Pin 7 von PORT B als Ausgang fuer LCD
   LOOPLED_DDR |= (1<<LOOPLED_PIN);
   DDRB |= (1<<0); // HI fuer Spannungsteiler
   PORTB |= (1<<0);
   DDRB |= (1<<3); // HI fuer Spannungsteiler
   PORTB |= (1<<3);

   OSZIDDR |= (1<<PULSA);	//Pin 0 von  als Ausgang fuer OSZI
   OSZIPORT |= (1<<PULSA);		// HI


}

uint16_t floatmittel(uint16_t* werte)
{
   uint8_t pos=4;
   uint16_t mittel =0;
   while (pos--)
   {
      mittel += werte[pos]/4;
   }
   return mittel;
}

int main (void)
{
	slaveinit();
	
	lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);
	lcd_puts("Guten Tag\0");
	delay_ms(1000);
	lcd_cls();
	lcd_puts("READY");
	
	delay_ms(1000);
   lcd_gotoxy(0,0);
   lcd_puts("     ");
   
   // timer 1 einrichten
   timer1_comp();
   sei();

#pragma mark while
	while (1) 
	{
		
		loopCount0 ++;
		//_delay_ms(2);
      
      //COMP_PORT ^= (1<<COMP_DRIVE_PIN_A);
		if (loopCount0 >=0x0AFF)
      {
         
         //LOOPLED_PORT ^= (1<<LOOPLED_PIN);
         loopCount1++;
         
         if ((loopCount1 >0x0080) && (!(Programmstatus & (1<<MANUELL))))
         {
            //PORTB ^= (1<<1);
            // Werte reset
            captured_value=0;
            captured = 0;
            // Kanal waehlen
            adckanal = COMP_ADC_PIN_A;
            ADMUX = COMP_ADC_PIN_A & 0x07; // 4
            // counter reset
            TCNT1 = 0;
            // Pin HI
            COMP_PORT |= (1<<COMP_DRIVE_PIN_A);
            waitcounter=1;
          //  OSZIA_LO;
     //       while (!captured);
            while ((!captured) || (waitcounter < 0xFFA))
            {
               waitcounter++;
               //lcd_gotoxy(16,1);
               //lcd_putint(waitcounter);

               
            }; // warten, captured wird in ISR gesetzt
            //OSZIA_HI;
            _delay_us(100);
            /*
            captured_value=0;
            captured = 0;
            adckanal = COMP_ADC_PIN_B;
            
            ADMUX = COMP_ADC_PIN_B & 0x07; // 5
            TCNT1 = 0;
            COMP_PORT |= (1<<COMP_DRIVE_PIN_B);
            waitcounter=1;
            while ((!captured) || (waitcounter == 0))
            {
               waitcounter++;
            }; // warten, captured wird in ISR gesetzt
            */
            
            lcd_gotoxy(16,1);
            lcd_putc('w');
            lcd_putint(waitcounter);

            lcd_gotoxy(16,0);
            lcd_putc('o');
            lcd_putint(overflow);
            //               captured_value=0;
            
            lcd_gotoxy(0,0);
            lcd_puts("chA:");
            lcd_putint16(a);
            //lcd_putint16(floatmittel(mittelwertA));
            
            lcd_gotoxy(0,1);
            lcd_puts("cA:");
            lcd_putint(captAcounter);

            lcd_gotoxy(8,1);
            lcd_puts("cB:");
            lcd_putint(captBcounter);

            
            captured = 0;
            
            LOOPLED_PORT ^= (1<<LOOPLED_PIN);
            loopCount1=0;
            
            
         }
         
         loopCount0 =0;
      }
		
      
      
#pragma mark Tastatur
		/* ******************** */
      if (TASTATUR_ON)
      {
		initADC(TASTATURPIN);
		Tastenwert=(readKanal(TASTATURPIN)>>2);
		
//		lcd_gotoxy(3,1);
//		lcd_putint(Tastenwert);
//		Tastenwert=0;
		if (Tastenwert>5)
		{
			/*
			 0:											1	2	3
			 1:											4	5	6
			 2:											7	8	9
			 3:											x	0	y
			 4: Schalterpos -
			 5: Manuell ein
			 6: Schalterpos +
			 7: 
			 8: 
			 9: 
			 
			 12: Manuell aus
			 */
			 
			TastaturCount++;
			if (TastaturCount>=200)
			{
				
				 
				 lcd_gotoxy(17,1);
				 lcd_puts("T:  \0");
				 //lcd_putint(Tastenwert);
				 
				uint8_t Taste=Tastenwahl(Tastenwert);
				//Taste=0;
				 lcd_gotoxy(19,1);
				 lcd_putint1(Taste);
				 //delay_ms(600);
				// lcd_clr_line(1);
				 

				TastaturCount=0;
				Tastenwert=0x00;
				uint8_t i=0;
				uint8_t pos=0;
//				lcd_gotoxy(18,1);
//				lcd_putint2(Taste);
				
				switch (Taste)
				{
					case 0:// Schalter auf Null-Position
					{ 
						if (Programmstatus & (1<<MANUELL))
						{
							Manuellcounter=0;
							Programmstatus |= (1<<MANUELLNEU);
							/*
							lcd_gotoxy(13,0);
							lcd_puts("S\0");
							lcd_gotoxy(19,0);
							lcd_putint1(Schalterposition); // Schalterstellung
							lcd_gotoxy(0,1);
							lcd_puts("SI:\0");
							lcd_putint(ServoimpulsdauerSpeicher); // Servoimpulsdauer
							lcd_gotoxy(5,0);
							lcd_puts("SP\0");
							lcd_putint(Servoimpulsdauer); // Servoimpulsdauer
							*/
						}
						
					}break;
						
					case 1:	//	
					{ 
					if (Programmstatus & (1<<MANUELL))
						{
						Manuellcounter=0;
						
						}
					}break;
						
					case 2://
					{ 
					
						if (Programmstatus & (1<<MANUELL))
						{
						Manuellcounter=0;
						
						
						}
						
					}break;
						
					case 3: //	Uhr aus
					{ 
						if (Programmstatus & (1<<MANUELL))
						{
						uint8_t i=0;
						Manuellcounter=0;
						

						}
					}break;
						
					case 4://
					{ 
                  uint8_t i=0;

					}break;
						
					case 5://
					{ 
						Programmstatus |= (1<<MANUELL);	// MANUELL ON
						Manuellcounter=0;
						MANUELL_PORT |= (1<<MANUELLPIN);
						Programmstatus |= (1<<MANUELLNEU);
						lcd_clr_line(1);
						/*
							lcd_gotoxy(13,0);
							lcd_puts("S\0");
							lcd_putint1(Schalterposition); // Schalterstellung
							lcd_gotoxy(0,1);
							lcd_puts("SP:\0");
							lcd_putint(ServoimpulsdauerSpeicher); // Servoimpulsdauer
							lcd_gotoxy(5,0);
							lcd_puts("SI\0");
							lcd_putint(Servoimpulsdauer); // Servoimpulsdauer
						*/
					}break;
						
					case 6://
					{ 
					
					}break;
						
					case 7:// Schalter rückwaerts
					{ 
						if ((Programmstatus & (1<<MANUELL)) )
						{
							Manuellcounter=0;
							Programmstatus |= (1<<MANUELLNEU);
							//OSZIALO;
							/*
							lcd_gotoxy(13,0);
							lcd_puts("S\0");
							lcd_putint1(Schalterposition); // Schalterstellung
							lcd_gotoxy(0,1);
							lcd_puts("SP:\0");
							lcd_putint(ServoimpulsdauerSpeicher); // Servoimpulsdauer
							lcd_gotoxy(5,0);
							lcd_puts("SI\0");
							lcd_putint(Servoimpulsdauer); // Servoimpulsdauer
							//OSZIAHI;
							*/
						}
						else 
						{
							
						}
	
					}break;
						
					case 8://
					{ 
						
					}break;
						
					case 9:// Schalter vorwaerts
					{ 
						Manuellcounter=0;
						if ((Programmstatus & (1<<MANUELL)) )
						{
							//OSZIALO;
							Programmstatus |= (1<<MANUELLNEU);
							/*
							lcd_gotoxy(13,0);
							lcd_puts("S\0");
							lcd_putint1(Schalterposition); // Schalterstellung
							lcd_gotoxy(0,1);
							lcd_puts("SP:\0");
							lcd_putint(ServoimpulsdauerSpeicher); // Servoimpulsdauer
							lcd_gotoxy(5,0);
							lcd_puts("SI\0");
							lcd_putint(Servoimpulsdauer); // Servoimpulsdauer

							//OSZIAHI;
							*/
							
						}
						else 
						{
							//lcd_gotoxy(10,0);
							//lcd_puts("S:!\0");
						}
					

					}break;

					case 10:// *
					{ 
						
					}break;

					case 11://
					{ 
						
					}break;
						
					case 12: // # Normalbetrieb einschalten
					{
						Programmstatus &= ~(1<<MANUELL); // MANUELL OFF
						Programmstatus &= ~(1<<MANUELLNEU);
						MANUELL_PORT &= ~(1<<MANUELLPIN);
					}
						
				}//switch Tastatur
				
//				delay_ms(400);
//				lcd_gotoxy(18,1);
//				lcd_puts("  ");		// Tastenanzeige loeschen

			}//if TastaturCount	
			
		}
      } // if TASTATUR_ON
	}
	
	
	return 0;
}
