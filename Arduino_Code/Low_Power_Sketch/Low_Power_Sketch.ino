
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <Narcoleptic.h>

const byte LED = 13;
 
void setup () 
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // slow clock to divide by 1
  clock_prescale_set (clock_div_1);
  /*
  for (byte i = 0; i <= A5; i++)
    {
    pinMode (i, OUTPUT);    // changed as per below
    digitalWrite (i, LOW);  //     ditto
    }
  */

  // disable ADC
  ADCSRA = 0;  

  // turn off various modules
  power_all_disable ();
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();              // sleep within 3 clock cycles of above
}

void loop () 
{ 
}