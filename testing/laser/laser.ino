/*
Arduino

Om fila, prosjektet

*/


/*********************************
 *  IMPORTS
 *********************************/

#include <Stepper.h>
#include <SFE_TSL2561.h>
#include <Wire.h>

#include "laser.h"

/*********************************
 *  GLOBAL
 *********************************/

/* Connections */
const int ledPin = 13;
const int motorPin1 = 4;
const int motorPin2 = 5;
const int motorPin3 = 6;
const int motorPin4 = 7;

// Create an SFE_TSL2561 object, here called "light"
SFE_TSL2561 light;

/* Stepper */
const int STEPS_PER_REVOLUTION = 200;
const int MAX_RPM = 150;
const int NUMBER_OF_STEPS = 12;
int step_position = 0;
int dir = 1;

/* Light */
const boolean gain = 0;                 // Gain setting, 0 = X1 low gain, 1 = X16 high gain
unsigned int shutter_time;              // Integration ("shutter") time in millisecond
const double threshold = 50;
const unsigned char shutter_time_code = 0;
double lux = 0;
double old_lux = 0;

// initialize the stepper library on pins 8 through 11:
Stepper stepper(STEPS_PER_REVOLUTION, motorPin1, motorPin2, motorPin3, motorPin4);


/**********************************


/**
 * Setup
 */ 
void setup() {
    //Serial.begin(9600);
  
    /* Connections */
    pinMode(ledPin, OUTPUT);
    
    /* Init */
    stepper.setSpeed(MAX_RPM);
    light.begin();

    // setTiming() will set the shutter time
    light.setTiming(gain, shutter_time_code, shutter_time);

    // To start taking measurements, power up the sensor:
    light.setPowerUp();
    
    /* Calibration */
    calibrate_light();
    calibrate_stepper();
}

void calibrate_light() {
    
}

void calibrate_stepper() {
    
}


/**
 * Loop
 */ 
void loop() {
    boolean changed = false;
    increment_mirror();
    changed = monitor_change();
    play(changed);
}

void stepper_move(int dir) {
    stepper.step(dir);
    delay(1); // 2, 200
}

void increment_mirror() {
    if (dir == 1) {
        if (step_position < NUMBER_OF_STEPS - 1) {
            stepper_move(dir);
            step_position++;
        } else {
            dir = - dir;
            //delay(100);
            stepper_move(dir);
            step_position--;
        }
    } else {
        if (step_position > 0) {
            stepper_move(dir);
            step_position--;
        } else {
            dir = - dir;
            //delay(100);
            stepper_move(dir);
            step_position++;
        }
    }
}

boolean monitor_change() {
    lux = get_lux();
    //Serial.println(lux);
    
    double diff = lux - old_lux;
    
    old_lux = lux;
    if (diff > threshold || diff < - threshold) {
        return true;
    } else {
        return false;
    }
} 

void play(boolean changed) {
    if (changed) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }
}

double get_lux() {
    // Retrieve the data from the device:
    unsigned int data0, data1;

    // getData() returned true, communication was successful

    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light). If this happens, you can
    // reduce the integration time and/or gain.
    if (light.getData(data0,data1)) {
        boolean good;  // True if neither sensor is saturated
        
        double lux_reading;
        // Perform lux calculation:
        good = light.getLux(gain, shutter_time, data0, data1, lux_reading);
        
        return lux_reading;
    } else {
        // getData() returned false because of an I2C error, inform the user.
        byte error = light.getError();
        printError(error);
    }
}

/**
 * If there's an I2C error, this function will
 * print out an explanation.
 */
void printError(byte error) {
    Serial.print("I2C error: ");
    Serial.print(error,DEC);
    Serial.print(", ");

    switch(error) {
    case 0:
        Serial.println("success");
        break;
    case 1:
        Serial.println("data too long for transmit buffer");
        break;
    case 2:
        Serial.println("received NACK on address (disconnected?)");
        break;
    case 3:
        Serial.println("received NACK on data");
        break;
    case 4:
        Serial.println("other error");
        break;
    default:
        Serial.println("unknown error");
    }
}


