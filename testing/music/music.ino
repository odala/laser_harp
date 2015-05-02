/*
Arduino

Om fila, prosjektet

*/

/*********************************
 *  IMPORTS
 *********************************/

#include <Wire.h>

#include "pitches.h"

/*********************************
 *  GLOBAL
 *********************************/

/* Connections */
const int SOUND_PIN = 9;
//int ENABLE_PIN = 2;

/* Variables */
unsigned long note_duration = 1000;    // in milliseconds
int step_position;
int current;

// note frequencies corresponding to the 12 different laser beams
int frequencies[] = {NOTE_A3, NOTE_B3, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5};


/**********************************


/**
 * Setup
 */ 
void setup(void) {
    //Serial.begin(9600);
    //play_melody();
    Wire.begin(9);
    Wire.onReceive(receiveEvent);
    pinMode(SOUND_PIN, OUTPUT);
    //pinMode(ENABLE_PIN, OUTPUT);
}

/**
 * Loop
 */
void loop() {
    play(step_position);
    //digitalWrite(ENABLE_PIN, HIGH);
    //delay(1000);
    //digitalWrite(ENABLE_PIN, LOW);
    //delay(1000);
}


/**
 * Toggle the tone corresponding to the current step position
 * tone(pin, frequency(Hz), [duration(ms)]) & notone(pin)
 */
void play(int step_position) {
    if (step_position == current) { return; }
    if (step_position >= 0 && step_position < 12){
        tone(SOUND_PIN, frequencies[step_position], 5000);
        current = step_position;
    }
    else if (step_position >= 12 && step_position < 24){
        if (step_position == current+12){
            noTone(SOUND_PIN);
        }
    } 
}


/**
 * Receive integer from the other Arduino.
 * If step_position is between 0 and 11: play the corresponding tone i.
 * If step_position is between 12 and 23: stop playing the corresponding tone (i-12).
 */
void receiveEvent(int howMany) {
  step_position = Wire.read();    // receive byte as an integer
  //Serial.println(step_position);
}


/**
 * simple test of the tone() function
 */
void play_melody(){
    int melody[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5};
    for (int i = 0; i < 10; i++){
        tone(SOUND_PIN, frequencies[i], note_duration);
        tone(SOUND_PIN, frequencies[i], note_duration);
        delay(note_duration+250);
        noTone(SOUND_PIN);
    }
}


/**
 * not relevant
 */
void play2(int step_position) {
    tone2(SOUND_PIN, frequencies[step_position], note_duration);
}

/**
 * not relevant
 */
void tone2(int SOUND_PIN, int frequency, int note_duration){
    if (frequency == 0.0){
        delayMicroseconds(note_duration);
    }
    else {
        // calculate the half period in microseconds
        int half_period = (int)(1.0E3/(2*frequency));
        // calculate the number of cycles for duration
        int nCycles = (int)(note_duration/(2*half_period));
        for (int i=0; i < nCycles; i++){
            digitalWrite(SOUND_PIN,HIGH);
            delayMicroseconds(half_period);
            digitalWrite(SOUND_PIN,LOW);
            delayMicroseconds(half_period);
        }        
    }
    
}
