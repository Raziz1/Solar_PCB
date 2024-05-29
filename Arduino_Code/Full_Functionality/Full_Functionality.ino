/***
  File: Full_Functionality.ino
  Author: Rahim Aziz
  Date: May 21, 2024
  Description: Arduino script for reading and writing data to EEPROM, including temperature and humidity readings from a DHT11 sensor. 
***/

// ===============================
// Libraries
// ===============================
#include <EEPROM.h>
#include <DHT11.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Narcoleptic.h>

// ===============================
// Global Variables
// ===============================
struct DHT11DataObject {
  int temp;
  int humd;
  int index;
};

#define LED 13
#define DEBUGMODE 0

// Create an instance of the DHT11 class.
// - For Arduino: Connect the sensor to Digital I/O Pin 2.
DHT11 dht11(2);

int writeTempAddress = 0;   //Location we want the data to be put.

// ===============================
// Setup Function
// ===============================
void setup() {
  Serial.begin(9600);

  // Uncomment the line below to set a custom delay between sensor readings (in milliseconds).
  // dht11.setDelay(500); // Set this to the desired delay. Default is 500ms.

  // Low power mode setup
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  disableAllPeriph();
  // Another tweaks to lower the power consumption
  ADCSRA &= ~(1<<ADEN); //Disable ADC
  ACSR = (1<<ACD); //Disable the analog comparator

  // ATmega48/88/168/328
  DIDR0 = B00111111; //Disable digital input buffers on all ADC0-ADC5 pins
  DIDR1 = (1<<AIN1D)|(1<<AIN0D); //Disable digital input buffer on AIN1/0
}

// ===============================
// Main Loop
// ===============================
void loop() {
  if(DEBUGMODE)
  {
    debugMode();
  }
  else
  {
    Narcoleptic.delay(10000); //uncalibrated delay for 10s
    Narcoleptic.enableSerial();
    readTEMP();
    Serial.flush();
    Narcoleptic.disableSerial();
  }
}

/**
 * Reads the data of size type struct DHT11DataObject from the EEPROM at the users desired address
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void getEEPROM()
{
  Serial.flush();
  int readAddress = 0;

  Serial.println("Enter desired read address: ");
  while (!Serial.available()); // Wait until data is available
  readAddress = Serial.parseInt();
  
  Serial.println("Getting EEPROM address: " + String(readAddress));
  // Check if the entered address is within the valid range
  if (readAddress >= 0 && readAddress < EEPROM.length()) {
    // Read the data from EEPROM at the specified address
    DHT11DataObject customReadVar;
    EEPROM.get(readAddress, customReadVar);
    Serial.println("Temp: " + String(customReadVar.temp));
    Serial.println("Humid: " + String(customReadVar.humd));
    Serial.println("Index: " + String(customReadVar.index));
  } else {
    Serial.println("Invalid EEPROM address. Please enter a value between 0 and 1023.");
  }
}

/**
 * Write the data of size type struct DHT11DataObject to the EEPROM at the users desired address
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void putEEPROM()
{
  Serial.flush();
  Serial.println("Enter EEPROM address (0-1023) to write: ");
  while (!Serial.available()); // Wait until data is available
  int writeAddress = Serial.parseInt(); // Read the integer from Serial input

  // Check if the entered address is within the valid range
  if (writeAddress >= 0 && writeAddress < EEPROM.length()) {
    DHT11DataObject customVar = {
      21,
      33,
      1
    };
    Serial.println("Putting data in EEPROM address: " + String(writeAddress));
    EEPROM.put(writeAddress, customVar);
    writeAddress += sizeof(DHT11DataObject);
  } else {
    Serial.println("Invalid EEPROM address. Please enter a value between 0 and 1023.");
  }
}

/**
 * Resets all of the data in the EEPROM to 0
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void clearEEPROM()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  } 
}

/**
 * Reads environmental data from the DHT11 and stores it in the EEPROM
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void writeTEMP()
{
    int temperature = 0;
    int humidity = 0;

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    if (result == 0) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C\tHumidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      DHT11DataObject tempReading = {temperature, humidity, writeTempAddress};

      // Check if the entered address is within the valid range
      if (writeTempAddress >= 0 && writeTempAddress < EEPROM.length()) {
        Serial.println("Writing temperature data to address: " + String(writeTempAddress));
        EEPROM.put(writeTempAddress, tempReading);
        writeTempAddress += sizeof(DHT11DataObject);       
      }
      else
      {
        Serial.println("EEPROM out of range!");
      }
    } 
    else 
    {
        Serial.println(DHT11::getErrorString(result));
    }
}

/**
 * Reads environmental data from the DHT11
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void readTEMP()
{
    int temperature = 0;
    int humidity = 0;

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    if (result == 0) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C\tHumidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    } else {
        Serial.println(DHT11::getErrorString(result));
    }
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

/**
 * Utilizes the Narcoleptic library to enable all peripherals
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void enableAllPeriph()
{
  Narcoleptic.enableWire();
  Narcoleptic.enableTimer1();
  Narcoleptic.enableTimer2();
  Narcoleptic.enableTimer3();
  Narcoleptic.enableTimer4();
  Narcoleptic.enableSerial();
  Narcoleptic.enableADC();
  Narcoleptic.enableSPI();
  Narcoleptic.enableTouch();
}

/**
 * Takes a keyboard input and runs the appropriate function. 
 * This function is primarily used for debugging
 * 
 * @param 'N/A'
 * @return N/A
 * @throws N/A
 * 
 */
void debugMode()
{
  if (Serial.available() > 0) 
  {
    switch (Serial.read())
    {
    case 'w':
      putEEPROM(); 
      break;

    case 'r':
      getEEPROM();
      break;

    case 'c':
      clearEEPROM();
      break;

    case 'a':
      readTEMP();
      break;

    case 't':
      writeTEMP();
      break;

    default:
      Serial.println("Not valid input!");
      break;
    }
  }
}
