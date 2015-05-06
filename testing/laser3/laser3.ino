/*
Laserharpe

Labprosjekt i TFY4190 Instrumentering. 
Jan Gulla og Oda Lauten
*/

const boolean DEBUG = false;

/*********************************
 * IMPORTS
 *********************************/

#include <Stepper.h>
#include <Wire.h>

#include "laser3.h"


/*********************************
 * GLOBAL
 *********************************/

/* Connections */
const int MOTOR_PIN_1 = 4;
const int MOTOR_PIN_2 = 5;
const int MOTOR_PIN_3 = 6;
const int MOTOR_PIN_4 = 7;
const int LIGHT_PIN = 0;
const int LED_PIN = 13;

/* Stepper */
const int STEPS_PER_REVOLUTION = 200;
const int MAX_RPM = 150;
const int NUMBER_OF_STEPS = 12;

/* Light */
const double THRESHOLD = 90;
int lux;
int light_levels[12];
int backlight;

/* Current state */
int current = -1;        // Current note playing. Initial: none
int step_position = 5;   // Current step position. Initial position of stepper: 5 of [0 - 11]
int dir = 1;             // Current step directio. Initial direction of stepper: 1

// initialize the stepper library
Stepper stepper(STEPS_PER_REVOLUTION, MOTOR_PIN_1, MOTOR_PIN_2, MOTOR_PIN_3, MOTOR_PIN_4);

/* Time */
const int TIME_LIMIT[2] = {3000, 10000};
unsigned long time = 0;
unsigned long last_time = 0;

/**********************************/

/*
 * Setup
 * 
 * Initiates the arduino. 
 */
void setup() {
    if (DEBUG) {
        Serial.println(9600);
    }

    /* Connections */
    pinMode(LIGHT_PIN, INPUT);

    /* Init */
    stepper.setSpeed(MAX_RPM);
    Wire.begin();

    /* Calibration */
    calibrate_light();
}

/* 
 * Calibrate light
 * 
 * Calibrates the backlight values. 
 */
void calibrate_light() {
    lux = analogRead(LIGHT_PIN);
    backlight = lux;
    if (DEBUG) {
        Serial.println("backlight: ");
        Serial.println(backlight);
    }
}

/**
 * Loop
 *
 * Main loop. 
 */ 
void loop() {
    int time_index = (dir + 1) / 2;
    time = micros();
    
    if (time - last_time > TIME_LIMIT[time_index]) {
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

/*
 * Increment mirror
 *
 * Increments the mirror a single step in
 * the current direction. 
 */
void increment_mirror() {
    if (dir == 1) {
        if (step_position < NUMBER_OF_STEPS - 1) {
            stepper_move(dir);
            step_position++;
        } else {
            dir = - dir;
            stepper_move(dir);
            step_position--;
        }
    } else {
        if (step_position > 0) {
            stepper_move(dir);
            step_position--;
        } else {
            dir = - dir;
            stepper_move(dir);
            step_position++;
        }
    }
}

/*
 * Stepper move
 *
 * Moves the stepper in the specified direction. 
 */
void stepper_move(int dir) {
    stepper.step(dir);
}

/* Record light
 * 
 * Records the current light level received 
 * by the sensor. The data is stored in
 * light_levels;
 */
void record_light() {
    lux = analogRead(LIGHT_PIN);
    if (DEBUG) {
        if (step_position == 4) {
            Serial.println(lux);
        }
    }
    light_levels[step_position] = lux;
}

/*
 * Max index
 *
 * Returns the index of the maximum element of
 * the array. 
 */
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

/* 
 * Detect
 *
 * Determines if the light is above the 
 * threshold value, thereby signaling 
 * the other arduino to produce a tone. 
 */
void detect() {
    int index = max_index(light_levels, 12);
    
    if (light_levels[index] - backlight > THRESHOLD) {
        transmit(index);
        current = index;
    } else if (current != -1) {
        transmit(current + 12);
        current = -1;
    }
}

/*
 * Transmit
 *
 * Transmits the data to other arduino. 
 */
void transmit(int data) {
    Wire.beginTransmission(9);
    Wire.write(data);
    Wire.endTransmission();
}
