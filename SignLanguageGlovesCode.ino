#include <Arduino.h>
#include <Audio.h>
#include "Sign.h"
#include <string.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
// Wireless tranceiver (RF24) headers
#include <nRF24L01.h>
#include <RF24.h>
// Accelerometer headers
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DSOX.h>

// create an RF24 object 
RF24 radio(9, 8);
// address through which the two wireless modules communicate
const byte address[6] = "00001";

Adafruit_LSM6DSOX sox;

AudioOutputI2S           audioOutput;
AudioPlaySdWav           playWav1;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

  // Uncomment this for the left hand. analogReference doens't work on Teensy 4.1. All Teensy analog pins are referenced to 3.3v.
  //analogReference(EXTERNAL);

  AudioMemory(8);

  if (!(SD.begin(BUILTIN_SDCARD))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  radio.begin();
  // set the address
  radio.openReadingPipe(0, address);
  // set module as transmitter
  radio.startListening();

  /* Initialise the accel/gyro sensor */
  if(!sox.begin_I2C(LSM6DS_I2CADDR_DEFAULT, &Wire2))
  {
    /* There was a problem detecting the LSM6DSOX ... check your connections */
    Serial.println("Ooops, no LSM6DSOX detected ... Check your wiring!");
    //while(1);
  }

  /* Set the range to +- 16G */
  //sox.setRange(ADXL343_RANGE_16_G);
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(25);

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
}

const Sign *previousSign = nullptr;
// number of times the same sign has been detected in a row.
// once this reaches a certain threshold, we play the audio for the sign.
int consecutiveCount = 0;
const int consecutiveThreshold = 10;
bool justPlayedAudio = false;
// the loop routine runs over and over again forever:
void loop() {
  char filename[256];
  //for(int i = 0; i < numSigns; i++) {
  //  strcpy(filename, signs[i].name);
  //  strcat(filename, ".WAV");
  //  playFile(filename);
  //}
  // Declare a variable to store the sign currently being made according to the sensors
  Sign currentSign;
  currentSign.name = "";
  // read the input on the right thumb
  int flexValuesR[FINGERS_PER_HAND];
  flexValuesR[0] = analogRead(R1);
  flexValuesR[1] = analogRead(R2);
  flexValuesR[2] = analogRead(R3);
  flexValuesR[3] = analogRead(R4);
  flexValuesR[4] = analogRead(R5);

  // read event from accelerometer
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  //Serial.print("Z: "); Serial.print(ACCEL_UP(accelEventR)); Serial.print("  ");Serial.println("m/s^2 ");
  
  setRightHand(&currentSign, flexValuesR, accel);
  // ignore the left hand for now
  currentSign.leftHand = {DONTCAREX5, HO_DONT_CARE};

  printHandStates(&currentSign);
  
  //currentSign = {"", {{EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT}, {DONTCAREX5, HO_DONT_CARE}};
  const Sign *ptr = findClosestKnownSign(&currentSign);

  if(ptr == previousSign) {
    consecutiveCount++;
  } else {
    consecutiveCount = 0;
    justPlayedAudio = false;
  }
  if(ptr != nullptr) {
    Serial.println(ptr->name);
    if(consecutiveCount >= consecutiveThreshold && !justPlayedAudio) {
      strcpy(filename, ptr->name);
      strcat(filename, ".WAV");
      playFile(filename);
      justPlayedAudio = true;
    }
  } else {
    Serial.println("No match");
  }

  previousSign = ptr;
  // TODO: Get left hand values from wireless module

  //Serial.println(flexValuesR[0]);
  //Serial.println(flexValuesR[1]);
  //Serial.println(flexValuesR[2]);
  //Serial.println(flexValuesR[3]);
  //Serial.println(flexValuesR[4]);
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
