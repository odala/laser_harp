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
const int lightPin = 0;
const int lightPin2 = 2;

// Create an SFE_TSL2561 object, here called "light"
SFE_TSL2561 light;

/* Stepper */
const int STEPS_PER_REVOLUTION = 200;
const int MAX_RPM = 150;
const int NUMBER_OF_STEPS = 12;
int step_position = 5;
int dir = 1;

/* Light */
const boolean gain = 1;                 // Gain setting, 0 = X1 low gain, 1 = X16 high gain
unsigned int shutter_time = 1;              // Integration ("shutter") time in millisecond
const double threshold = 70;
const unsigned char shutter_time_code = 3;
double lux = 0;
double old_lux[12];
double backlight = 0;

int tone_step = -1;

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
    for (int i = 0; i < 12; i++) {
        old_lux[i] = lux;
    }
    backlight = lux;
    Serial.println(backlight);
}

void calibrate_stepper() {
    
}


/**
 * Loop
 */ 
void loop() {
/*    Serial.println(step_position);
    Wire.beginTransmission(9);
    Wire.write(step_position);
    step_position++;
    Wire.endTransmission();
    */
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
        //unsigned long time1 = 0;
        //unsigned long time2 = 0;
        
        boolean changed = false;
        if (dir == 1) {
            detect();
            //changed = monitor_change2();
            //monitor_change2();
        //if (step_position == 11){
        //    Serial.println(changed);
        //}
            //play(changed);
        }
        
        
        increment_mirror();
        
        //boolean start = light.manualStart();
        //delay(1);
        //shutter_time = 1;
        //time1 = micros();
        //boolean stopp = light.manualStop();
        //time2 = micros();
        //Serial.println(time2 - time1);
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

void detect_test() {
    if (tone_step != step_position && tone_step != -1) {
        return;
    }
    int lux = analogRead(lightPin);
    
    int signal;
    
    if (tone_step != step_position) {
        if (lux > old_lux[step_position] + threshold) {
            signal = step_position;
            tone_step = step_position;
            old_lux[step_position] = lux;
        }
    } else {
        if (lux < old_lux[step_position] - threshold) {
            signal = step_position + 12;
            tone_step = -1;
            old_lux[step_position] = lux;
        }
    }
     
    Wire.beginTransmission(9);
    Wire.write(signal);
    Wire.endTransmission();
}


void detect() {
    if (step_position == 4) {
        int lux = analogRead(lightPin);
        Serial.println(lux);
    }
  
  
  /*
    int lux = analogRead(lightPin);
    
    int signal;
    
    if (lux - backlight > threshold) {
    */    
  
  
}

boolean monitor_change() {
    int LDRReading = analogRead(lightPin);
    //int LLRReading = digitalRead(lightPin2); 
    //Serial.print("simple lux: ");
    //if (step_position == 4) {
        //Serial.println(LDRReading);
    //}
    // Print out
    //Serial.println(LDRReading);
    //Serial.println(LLRReading);
    //delay(250); //just here to slow down the output for easier reading
    
    lux = LDRReading;
    
    double diff = LDRReading - backlight;
   
    if (diff > threshold) {
        return true;
    } else {
        return false;
    }  
    
    //return LLRReading;
}

boolean monitor_change2() {
    int LDRReading = analogRead(lightPin);
    //int LLRReading = digitalRead(lightPin2); 
    //Serial.print("simple lux: ");
    //if (step_position == 4) {
        //Serial.println(LDRReading);
    //}
    // Print out
    //Serial.println(LDRReading);
    //Serial.println(LLRReading);
    //delay(250); //just here to slow down the output for easier reading
    
    lux = LDRReading;
    
    double diff = lux - old_lux[step_position];
   
    if (diff > threshold) {
        return true;
    } else {
        return false;
    }  
    
    old_lux[step_position] = lux;
    //return LLRReading;
}

void play(boolean changed) {
    Wire.beginTransmission(9);
    if (changed) {
        Wire.write(step_position);
        //Serial.println(step_position);
    } else {
        Wire.write(step_position+12);
        //Serial.println(step_position + 12);
    }
    Wire.endTransmission();
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
        Serial.print("good: ");
        Serial.println(good);
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


