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

//#include "laser3.h"

/*********************************
 *  GLOBAL
 *********************************/

/* Connections */
const int ledPin = 13;
const int motorPin1 = 4;
const int motorPin2 = 5;
const int motorPin3 = 6;
const int motorPin4 = 7;
const int lightPin = 0;
const int lightPin2 = 2;

/* Stepper */
const int STEPS_PER_REVOLUTION = 200;
const int MAX_RPM = 150;
const int NUMBER_OF_STEPS = 12;
int step_position = 5;
int dir = 1;

/* Light */
const boolean gain = 1;                 // Gain setting, 0 = X1 low gain, 1 = X16 high gain
unsigned int shutter_time = 1;              // Integration ("shutter") time in millisecond
const double threshold = 90;
const unsigned char shutter_time_code = 3;
int lux;
int light_levels[12];
int backlight = 0;

int current = -1;

// initialize the stepper library on pins 8 through 11:
Stepper stepper(STEPS_PER_REVOLUTION, motorPin1, motorPin2, motorPin3, motorPin4);

unsigned long time = 0;
unsigned long last_time = 0;

/**********************************


/**
 * Setup
 */ 
void setup() {
    Serial.begin(9600);
    
    /* Connections */
    pinMode(ledPin, OUTPUT);
    pinMode(lightPin, INPUT);
    //pinMode(lightPin2, INPUT);
    
    /* Init */
    stepper.setSpeed(MAX_RPM);
    Wire.begin();
    
    //Serial light sensor
    //light.begin(); 

    // setTiming() will set the shutter time
    //light.setTiming(gain, shutter_time_code, shutter_time);

    // To start taking measurements, power up the sensor:
    //light.setPowerUp();
    
    //shutter_time = 1;
    
    /* Calibration */
    calibrate_light();
    calibrate_stepper();
}

void calibrate_light() {
    lux = analogRead(lightPin);
    backlight = lux;
    Serial.println(backlight);
}

void calibrate_stepper() {
    
}


/**
 * Loop
 */ 
void loop() {
    time = micros();
    double time_limit;
    if (dir == 1) {
        time_limit = 10000;
    } else {
        time_limit = 3000;
    }
    if (time - last_time > time_limit) { // 3000
        //Serial.println("step");
        last_time = time;

        if (dir == 1) {
            record_light();
        }
        
        if (step_position == 11) {
            detect();
        }
        
        increment_mirror();
    }
}

void stepper_move(int dir) {
    stepper.step(dir);
    //delay(1); // 2, 200
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

void record_light() {
    lux = analogRead(lightPin);
    //delay(1000);
    if (step_position == 4) {
        Serial.println(lux);
    }
    light_levels[step_position] = lux;
}

int max_index(int array[], int size_a) {
    int temp = 0;
    int index = 0;
    for (int i = 0; i < size_a; ++i) {
        if (array[i] > temp) {
            temp = array[i];
            index = i;
        }
    }
    return index;
}

void detect() {
    int index = max_index(light_levels, 12);
    
    if (light_levels[index] - backlight > threshold) {
        transmit(index);
        current = index;
    } else if (current != -1) {
        transmit(current + 12);
        current = -1;
    }
}

void transmit(int data) {
    Wire.beginTransmission(9);
    Wire.write(data);
    Wire.endTransmission();
}
