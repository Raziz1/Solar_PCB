
#include <avr/sleep.h>
#include <avr/wdt.h>

const byte LED = 13;

void flash ()
  {
  pinMode (LED, OUTPUT);
  for (byte i = 0; i < 10; i++)
    {
    digitalWrite (LED, HIGH);
    delay (50);
    digitalWrite (LED, LOW);
    delay (50);
    }
  
  digitalWrite (LED, LOW);

  //pinMode (LED, INPUT);
    
  }  // end of flash
  
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
 
void setup () { }

void loop () 
{
 
  flash ();
  
  // disable ADC
  ADCSRA = 0;  

  // clear various "reset" flags
  MCUSR = 0;     
  // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);
  // set interrupt mode and an interval 
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  wdt_reset();  // pat the dog
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();  
  
  // cancel sleep as a precaution
  sleep_disable();
  
  } // end of loop