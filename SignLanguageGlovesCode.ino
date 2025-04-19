#include <Arduino.h>
#include "Sign.h"
#include <SPI.h>
// Wireless tranceiver (RF24) headers
#include <nRF24L01.h>
#include <RF24.h>
// Accelerometer headers
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// create an RF24 object 
RF24 radio(9, 8);
// address through which the two wireless modules communicate
const byte address[6] = "00001";

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

  // Uncomment this for the left hand. analogReference doens't work on Teensy 4.1. All Teensy analog pins are referenced to 3.3v.
  //analogReference(EXTERNAL);

  radio.begin();
  // set the address
  radio.openReadingPipe(0, address);
  // set module as transmitter
  radio.startListening();

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to +- 16G */
  accel.setRange(ADXL343_RANGE_16_G);
}

// the loop routine runs over and over again forever:
void loop() {
  // Declare a variable to store the sign currently being made according to the sensors
  //Sign currentSign;
  // read the input on the right thumb
  int flexValuesR[FINGERS_PER_HAND];
  flexValuesR[0] = analogRead(R1);
  flexValuesR[1] = analogRead(R2);
  flexValuesR[2] = analogRead(R3);
  flexValuesR[3] = analogRead(R4);
  flexValuesR[4] = analogRead(R5);

  // read event from accelerometer
  //sensors_event_t accelEventR;
  ///accel.getEvent(&accelEventR);

  
  //Serial.print("Z: "); Serial.print(ACCEL_UP(accelEventR)); Serial.print("  ");Serial.println("m/s^2 ");
  
  //currentSign.setRightHand(flexValuesR);
  
  //const Sign *ptr = currentSign.findClosestKnownSign();
  
//  if(ptr != nullptr) {
//    //Serial.println(ptr->getName());
//  } else {
//    //Serial.println("No match");
//  }

  // TODO: Get left hand values from wireless module

  Serial.println(flexValuesR[0]);
  Serial.println(flexValuesR[1]);
  Serial.println(flexValuesR[2]);
  Serial.println(flexValuesR[3]);
  Serial.println(flexValuesR[4]);
  //int txtLength = strlen_P(txt);
//  for(byte i = 0; i < txtLength; i++) {
//    char myChar = pgm_read_byte_near(txt + i);
//    Serial.print(myChar);
//  }
//  Serial.println();

  //Sign workingSign;
  //memcpy_P(&workingSign, &signs[15], sizeof(Sign));
  //workingSign.printHandStates();
  //Sign::signs[0].printHandStates();
  // print out the value you read:
  //Serial.println(currentSign.rightFingers);



  //const char text[] = "Helloww World";
//  if(radio.available()) {
//    char text[32] = {0};
//    radio.read(&text, sizeof(text));
//    Serial.println(text);
//  }
  //radio.write(&text, sizeof(text));
  //delay(1000);
}
