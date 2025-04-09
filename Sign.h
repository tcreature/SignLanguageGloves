#ifndef SIGN_H
#define SIGN_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// Fingers are labeled with 1-5 with 1 as the thumb and 5 as the pinky finger.
// Here we assign each finger on the right hand to an analog input pin
const int R1 = A0, R2 = A1, R3 = A2, R4 = A6, R5 = A7;
const int FINGERS_PER_HAND = 5;
#define DONTCAREX5 {DONT_CARE, DONT_CARE, DONT_CARE, DONT_CARE, DONT_CARE}

// Define some macros for getting the up, down, left, right, forward, backward axes from an accelerometer sensor event
//#define ACCEL_UP(e) (-e.acceleration.z)
//#define ACCEL_FORWARD(e) (-e.acceleration.y)
//#define ACCEL_RIGHT(e) (-e.acceleration.x)

// To help differentiate between signs, define states for which direction the hand's palm is pointing
/* The accelerometer only allows us to calculate the roll and pitch. This limits how much we can
   actually determine about how the hand is oriented.*/
enum HandOrientation : byte {
  HO_UPRIGHT, // such as A, B, C, O, X, etc...
  HO_ON_SIDE, // such as G, H
  HO_PALM_DOWN, // such as P
  HO_UPSIDE_DOWN, // such as Q
  HO_DONT_CARE
};

// For now, we assume a finger can have three states: extended, partially curled, and fully curled
enum FingerState : byte {
  EXTENDED,
  PARTIALLY_CURLED,
  FULLY_CURLED,
  DONT_CARE
};

struct HandState {
  FingerState fingerStates[FINGERS_PER_HAND];
  HandOrientation handOrientation;

  bool operator == (const HandState& rhs) const {
    if((this->handOrientation != rhs.handOrientation) && (this->handOrientation != HO_DONT_CARE || rhs.handOrientation != HO_DONT_CARE)) {
      return false;
    }
    for(int i = 0; i < FINGERS_PER_HAND; i++) {
      if(this->fingerStates[i] != rhs.fingerStates[i] && (this->fingerStates[i] != DONT_CARE || rhs.fingerStates[i] != DONT_CARE))
        return false;
    }
    return true;
  }
};

struct Sign {
  const char *name;
  HandState rightHand;
  HandState leftHand;

  bool operator == (const Sign& rhs) const {
    if(this->leftHand == rhs.leftHand && this->rightHand == rhs.rightHand) {
      return true;
    } else {
      return false;
    }
  }
};

FingerState flexValueToFingerState(int flexValue) {
  // The value read from the flex sensors will be between 0 and 1023
  const int thresholdPartial = 400;
  const int thresholdFull = 900;

  if(flexValue >= thresholdFull) {
    return FULLY_CURLED;
  } else if(flexValue >= thresholdPartial) {
    return PARTIALLY_CURLED;
  } else {
    return EXTENDED;
  }
}

HandOrientation accelEventToHandOrientation(sensors_event_t e) {
  // TODO: Determine the direction the palm is facing, based on the accelerometer's x, y, and z acceleration
}

void setRightHand(Sign *sign, const int flexValues[FINGERS_PER_HAND], const sensors_event_t e) {
  for(int i = 0; i < FINGERS_PER_HAND; i++) {
    // Convert the value read from the analog input pins to a finger state
    sign->rightHand.fingerStates[i] = flexValueToFingerState(flexValues[i]);
  }
  sign->rightHand.handOrientation = accelEventToHandOrientation(e);
}

void printHandStates(Sign *sign) {
  Serial.print("Name: ");
  Serial.println(sign->name);
  Serial.println("Right hand:");
  for(int i = 0; i < FINGERS_PER_HAND; i++) {
    Serial.println(sign->rightHand.fingerStates[i]);
  }
  Serial.println("Left hand:");
  for(int i = 0; i < FINGERS_PER_HAND; i++) {
    Serial.println(sign->leftHand.fingerStates[i]);
  }
  Serial.println();
}

// The list of all signs our program knows about
const constexpr Sign signs[] = {
  {"a", {{EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT}, {DONTCAREX5, HO_DONT_CARE}},
};
const constexpr int numSigns = sizeof(signs) / sizeof(signs[0]);

const Sign *findClosestKnownSign(Sign *sign) {
  const Sign *ptr = nullptr;
  for(int i = 0; i < numSigns; i++) {
    if(*sign == signs[i]) {
      ptr = &signs[i];
    }
  }
  return ptr;
}

#endif
