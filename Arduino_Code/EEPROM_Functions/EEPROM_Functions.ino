/***
    eeprom_put example.

    This shows how to use the EEPROM.put() method.
    Also, this sketch will pre-set the EEPROM data for the
    example sketch eeprom_get.

    Note, unlike the single byte version EEPROM.write(),
    the put method will use update semantics. As in a byte
    will only be written to the EEPROM if the data is actually
    different.

    Written by Christopher Andrews 2015
    Released under MIT licence.
***/

#include <EEPROM.h>

struct DHT11DataObject {
  int temp;
  int humd;
  int index;
};

int writeAddress = 0;   //Location we want the data to be put.

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
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

    default:
      Serial.println("Not valid input!");
      break;
    }
  }
}

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

void putEEPROM()
{
  Serial.flush();
  Serial.println("Enter EEPROM address (0-1023) to write: ");
  while (!Serial.available()); // Wait until data is available
  writeAddress = Serial.parseInt(); // Read the integer from Serial input

  // Check if the entered address is within the valid range
  if (writeAddress >= 0 && writeAddress < EEPROM.length()) {
    DHT11DataObject customVar = {
      21,
      33,
      1
    };
    EEPROM.put(writeAddress, customVar);
    writeAddress += sizeof(DHT11DataObject);
  } else {
    Serial.println("Invalid data. Please enter a value between 0 and 255.");
  }
}

void clearEEPROM()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  } 
}
