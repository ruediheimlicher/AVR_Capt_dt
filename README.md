# AVR_Capt
Atmega: Read PT1000 or KTY8x without ADC

A Sensor in Series with a Capacitor is triggered to Vcc. 
The Analogcomparator is used to detect the crossing of the voltage level of the capacitor with a reference voltage and causes timer1
to count the pulses to that event. 
If the reference voltage is on 0.615 Vcc, the time is the time constant of the RC combination. So the actual resistance of the Sensor can be calculated with the timer count. This gives a greater resolution than using the ADC of the Atmega.
Tolerances of the capacitor can be adjusted by maniüulating the reference voltage of the AC.
Using the multiplexer of the ADC, multiple sensors can be read.


