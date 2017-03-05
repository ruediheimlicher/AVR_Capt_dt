/*
 * ----------------------------------------------------------------------------
 * defines
 */

#define LOOPLED_PORT	PORTD
#define LOOPLED_DDR	DDRD
#define LOOPLED_PIN	4


#define TASTATUR_ON            0

#define MANUELL_PORT          PORTD
#define MANUELL_DDR           DDRD
#define MANUELL_PIN           PIND

#define MANUELL               7	// Bit 7 von Status
#define MANUELLPIN            6	// Pin 6 von PORT D fuer Anzeige Manuell
#define MANUELLNEU            7	// Pin 7 von Status. Gesetzt wenn neue Schalterposition eingestellt
#define MANUELLTIMEOUT        100 // Loopled-counts bis Manuell zurueckgesetzt wird. 02FF: ca. 100 s


//Oszi
#define OSZIPORT           PORTB
#define OSZIDDR            DDRB
#define PULSA              7
#define OSZIA_LO OSZIPORT &= ~(1<<PULSA)
#define OSZIA_HI OSZIPORT |= (1<<PULSA)
#define OSZIA_TOGG OSZIPORT ^= (1<<PULSA)
