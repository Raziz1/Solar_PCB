
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <Narcoleptic.h>

const byte LED = 13;
 
void setup () 
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  /*
  for (byte i = 0; i <= A5; i++)
    {
    pinMode (i, OUTPUT);    // changed as per below
    digitalWrite (i, LOW);  //     ditto
    }
  */
  
  // slow clock to divide by 256
  clock_prescale_set (clock_div_1);

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  disableAllPeriph();
  // Another tweaks to lower the power consumption
  ADCSRA &= ~(1<<ADEN); //Disable ADC
  ACSR = (1<<ACD); //Disable the analog comparator

  // ATmega48/88/168/328
  DIDR0 = B00111111; //Disable digital input buffers on all ADC0-ADC5 pins
  DIDR1 = (1<<AIN1D)|(1<<AIN0D); //Disable digital input buffer on AIN1/0
}

void loop () 
{ 
  Narcoleptic.delay(10000); //uncalibrated delay for 10s
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
}

/**
 * Utilizes the Narcoleptic library to disable all peripherals
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void disableAllPeriph()
{
  Narcoleptic.disableWire();
  Narcoleptic.disableTimer1();
  Narcoleptic.disableTimer2();
  Narcoleptic.disableTimer3();
  Narcoleptic.disableTimer4();
  Narcoleptic.disableSerial();
  Narcoleptic.disableADC();
  Narcoleptic.disableSPI();
  Narcoleptic.disableTouch();
}