#include <stdint.h>
#include "SparkFunBME280.h"
#include "Wire.h"
#include "SPI.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <EEPROM.h>
#define BUTTONPIN 0
#define PRESS 0 //Switch this 0 to a 1 when I put this program on anything other than my protoboard. 

int buttonState = 0;
int tare = 0;
int temp = 0;
int altitude = 0;
int oldAltitude = 0;
int MaxValue = 0;
int EEpromAddr = 0;
int P_EEpromAddr = 1;
int count = 0;

BME280 mySensor;
Adafruit_7segment matrix = Adafruit_7segment();

void setup() {
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;
  matrix.begin(0x70);
  Serial.begin(9600);

  for (int i = 3; i >= 0; i--) { //countdown so I know I've been in the setup.
    matrix.print(i, DEC);
    matrix.writeDisplay();
    delay(700);
  }

  pinMode(BUTTONPIN, INPUT); // initialize the pushbutton pin as an input:

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
  for (int rowi = 8; rowi < 16; rowi++ )
  {
    Serial.print("0x");
    Serial.print(rowi, HEX);
    Serial.print("0:");
    for (int coli = 0; coli < 16; coli++ )
    {
      tempReadData = mySensor.readRegister(memCounter);
      Serial.print((tempReadData >> 4) & 0x0F, HEX);//Print first hex nibble
      Serial.print(tempReadData & 0x0F, HEX);//Print second hex nibble
      Serial.print(" ");
      memCounter++;
    }
    Serial.print("\n");
    //Read these two values or else the altitude doesn't calibrate correctly.
    mySensor.readTempF();
    mySensor.readFloatPressure();
  }

   int my_max = EEPROM.read(P_EEpromAddr);
   Serial.print("Max Value: ");
   Serial.println(my_max);
   
}


void loop() {
  matrix.print(MaxValue, DEC);
  matrix.writeDisplay();

  buttonState = digitalRead(BUTTONPIN); // read the state of the pushbutton
  temp = (mySensor.readFloatAltitudeFeet());// take an altimeter reading and set it to temp.

  // check if the pushbutton is pressed, set current altitude to zero, set EEprom to zero.
  if (buttonState == PRESS) {
    tare = (mySensor.readFloatAltitudeFeet());
    altitude = 0;
    oldAltitude = 0;
    EEPROM.write(EEpromAddr, altitude);

    count++;
    Serial.println(count);
  }


  if (count == 2){
    EEPROM.write(P_EEpromAddr, MaxValue);
    Serial.print("Max Value: ");
    Serial.println(MaxValue);
  }
  
  altitude = (temp - tare);

  if (altitude > oldAltitude) {
    EEPROM.write(EEpromAddr, altitude);
    MaxValue = altitude;
    oldAltitude = altitude;
  }

  else {
    delay(300); // Wait half a second to iterate.
    Serial.print("Altitude = ");
    Serial.println(altitude);
    count = 0; 
  }
}



