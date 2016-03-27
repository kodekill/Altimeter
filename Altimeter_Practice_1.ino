#include <stdint.h>
#include "SparkFunBME280.h"
#include "Wire.h"
#include "SPI.h"
#include <EEPROM.h>

const int buttonPin = 7;     
const int ledPin =  13;      
int temp = 0;
int oldAltitude = 0; 
int tare = 0;
int altitude = 0; 
int buttonState = 0;
int EEpromNumber; 
int EEpromAddr = 0;
BME280 mySensor;

void setup() {
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;
  Serial.begin(57600);
  
  Serial.print("Highest Recorded Altitude: ");
  Serial.print(EEPROM.read(EEpromAddr));
  Serial.println("ft");
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  //***Operation settings*****************************//
  
  //renMode can be:
  //  0, Sleep mode
  //  1 or 2, Forced mode
  //  3, Normal mode
  mySensor.settings.runMode = 3; //Normal mode
  
  //tStandby can be:
  //  0, 0.5ms
  //  1, 62.5ms
  //  2, 125ms
  //  3, 250ms
  //  4, 500ms
  //  5, 1000ms
  //  6, 10ms
  //  7, 20ms
  mySensor.settings.tStandby = 0;
  
  //filter can be off or number of FIR coefficients to use:
  //  0, filter off
  //  1, coefficients = 2
  //  2, coefficients = 4
  //  3, coefficients = 8
  //  4, coefficients = 16
  mySensor.settings.filter = 0;
  
  //tempOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.tempOverSample = 1;

  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
    mySensor.settings.pressOverSample = 1;
  
  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.humidOverSample = 1;
  
  
  Serial.print("Program Started\n");
  Serial.print("Starting BME280... result of .begin(): 0x");
  
  //Calling .begin() causes the settings to be loaded
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(mySensor.begin(), HEX);

  Serial.print("Displaying ID, reset and ctrl regs\n");
  
  Serial.print("ID(0xD0): 0x");
  Serial.println(mySensor.readRegister(BME280_CHIP_ID_REG), HEX);
  Serial.print("Reset register(0xE0): 0x");
  Serial.println(mySensor.readRegister(BME280_RST_REG), HEX);
  Serial.print("ctrl_meas(0xF4): 0x");
  Serial.println(mySensor.readRegister(BME280_CTRL_MEAS_REG), HEX);
  Serial.print("ctrl_hum(0xF2): 0x");
  Serial.println(mySensor.readRegister(BME280_CTRL_HUMIDITY_REG), HEX);

  Serial.print("\n\n");

  Serial.print("Displaying all regs\n");
  uint8_t memCounter = 0x80;
  uint8_t tempReadData;
  for(int rowi = 8; rowi < 16; rowi++ )
  {
    Serial.print("0x");
    Serial.print(rowi, HEX);
    Serial.print("0:");
    for(int coli = 0; coli < 16; coli++ )
    {
      tempReadData = mySensor.readRegister(memCounter);
      Serial.print((tempReadData >> 4) & 0x0F, HEX);//Print first hex nibble
      Serial.print(tempReadData & 0x0F, HEX);//Print second hex nibble
      Serial.print(" ");
      memCounter++;
    }
    Serial.print("\n");
    
//Read these 2 values or else the altitude doesn't calibrate correctly.     
mySensor.readTempF();
mySensor.readFloatPressure();
  }
  
}


void loop() {
  // read the state of the pushbutton and read altimeter and set it to temp. 
  buttonState = digitalRead(buttonPin);
  temp = (mySensor.readFloatAltitudeFeet());
  
//Serial.print("First Reading: ");
//Serial.println(temp);

  // check if the pushbutton is pressed, set current altitude to zero, set EEprom to zero. 
  if (buttonState == HIGH) {
    // turn LED on:///////////////////////////
    digitalWrite(ledPin, HIGH);///////////////
    tare = (mySensor.readFloatAltitudeFeet());
    altitude = 0; 
    oldAltitude = 0;
    EEPROM.write(EEpromAddr, altitude);  
  }

  //For now just turn off LED ////////////////
  else {//////////////////////////////////////
    // turn LED off://////////////////////////
    digitalWrite(ledPin, LOW);////////////////
  }

  altitude = (temp - tare);

  //Print out Altitude to Serial Monitor. 
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println("ft");

if (altitude > oldAltitude){
  EEPROM.write(EEpromAddr, altitude);
  oldAltitude = altitude;

  Serial.print("New Highest Altitude: ");
  Serial.print(altitude);
  Serial.println("ft");
}

  delay(1500); // Wait half a second to iterate.
}




