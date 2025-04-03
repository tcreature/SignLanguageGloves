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
}


// For now, we assume a finger can have three states: extended, partially curled, and fully curled
enum FingerState : byte {
  EXTENDED,
  PARTIALLY_CURLED,
  FULLY_CURLED,
  DONT_CARE
}

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


class Sign {
  private:
    const char* name;
    HandState rightHand;
    HandState leftHand;
    // TODO: Add variables for accelerometer data
    
    FingerState flexValueToFingerState(int flexValue) {
      // The value read from the flex sensors will be between 0 and 1023
      int thresholdPartial = 400;
      int thresholdFull = 900;
      
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

  public:
    static const Sign signs[];
    static const int numSigns;

    const char* getName() const {
      return name;
    }
    
    Sign() {
      name = "";
      rightHand = {DONT_CARE, DONT_CARE, DONT_CARE, DONT_CARE, DONT_CARE};
      leftHand = {DONT_CARE, DONT_CARE, DONT_CARE, DONT_CARE, DONT_CARE};
      handOrientation = HO_DONT_CARE;
    }
    
    constexpr Sign(const char* name, HandState rightHand = DONTCAREX5, HandState leftHand = DONTCAREX5) : name(name), rightHand(rightHand), leftHand(leftHand) {}

    void setRightHand(const int flexValues[FINGERS_PER_HAND], const sensors_event_t e) {
      for(int i = 0; i < FINGERS_PER_HAND; i++) {
       // Convert the value read from the analog input pins to a finger state
       rightHand.fingerStates[i] = flexValueToFingerState(flexValues[i]);
      }
      rightHand.handOrientation = accelEventToHandOrientation(e);
    }

    void printHandStates() const {
      Serial.print("Name: ");
      Serial.println(name);
      Serial.println("Right hand:");
      for(int i = 0; i < FINGERS_PER_HAND; i++) {
        Serial.println(rightHand.fingerStates[i]);
      }
      Serial.println("Left hand:");
      for(int i = 0; i < FINGERS_PER_HAND; i++) {
        Serial.println(leftHand.fingerStates[i]);
      }
      Serial.println();
    }

    bool operator == (const Sign& rhs) const {
      if(this->leftHand == rhs.leftHand && this->rightHand == rhs.rightHand) {
        return true;
      } else {
        return false;
      }
    }

    // Return a pointer to a known sign which matches this one, or nullptr if it doesn't match anything
    const Sign* findClosestKnownSign() const {
      const Sign* ptr = nullptr;
      for(int i = 0; i < numSigns; i++) {
        if(*this == signs[i]) {
          ptr = &signs[i];
        }
      }
      return ptr;
    }
};

// The list of all signs our program knows about
const constexpr Sign Sign::signs[] = {
  Sign("a", {EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("b", {FULLY_CURLED, EXTENDED, EXTENDED, EXTENDED, EXTENDED}, HO_UPRIGHT),
  Sign("c", {PARTIALLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED}, HO_UPRIGHT),
  Sign("d", {FULLY_CURLED, EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("e", {FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("f", {FULLY_CURLED, FULLY_CURLED, EXTENDED, EXTENDED, EXTENDED}, HO_UPRIGHT),
  Sign("g", {PARTIALLY_CURLED, EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_ON_SIDE),
  Sign("h", {PARTIALLY_CURLED, EXTENDED, EXTENDED, FULLY_CURLED, FULLY_CURLED}, HO_ON_SIDE),
  Sign("i", {PARTIALLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, EXTENDED}, HO_UPRIGHT),
  // TODO: Add J
  Sign("k", {EXTENDED, EXTENDED, EXTENDED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("l", {EXTENDED, EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  // TODO: Add M
  // TODO: Add N
  Sign("o", {FULLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED}, HO_UPRIGHT),
  Sign("p", {EXTENDED, EXTENDED, PARTIALLY_CURLED, PARTIALLY_CURLED, PARTIALLY_CURLED}, HO_PALM_DOWN),
  Sign("q", {EXTENDED, EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_PALM_DOWN),
  Sign("r", {PARTIALLY_CURLED, EXTENDED, EXTENDED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("s", {PARTIALLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("t", {EXTENDED, PARTIALLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("u", {PARTIALLY_CURLED, EXTENDED, EXTENDED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  // TODO: Add V
  Sign("w", {PARTIALLY_CURLED, EXTENDED, EXTENDED, EXTENDED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("x", {FULLY_CURLED, PARTIALLY_CURLED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED}, HO_UPRIGHT),
  Sign("y", {EXTENDED, FULLY_CURLED, FULLY_CURLED, FULLY_CURLED, EXTENDED}, HO_UPRIGHT)
  //TODO: Add Z
};
const constexpr int Sign::numSigns = sizeof(signs) / sizeof(signs[0]);

#endif
