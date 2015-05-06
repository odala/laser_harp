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
    Wire.begin(9);
    Wire.onReceive(receiveEvent);
    pinMode(SOUND_PIN, OUTPUT);
}

/**
 * Loop
 */
void loop() {
    play(step_position);
}


/**
 * Toggle the tone corresponding to the current step position
 * If step_position is between 0 and 11: play the corresponding tone i.
 * If step_position is between 12 and 23: stop playing the corresponding tone (i-12).
 * tone(pin, frequency(Hz), [duration(ms)])
 * notone(pin)
 */
void play(int step_position) {
    if (step_position == current) { return; }
    if (step_position >= 0 && step_position < 12){
        tone(SOUND_PIN, frequencies[step_position]);
        current = step_position;
    }
    else if (step_position >= 12 && step_position < 24){
        if (step_position == current+12){
            noTone(SOUND_PIN);
        }
    } 
}


/**
 * Receive integer (0-23) corresponding to the step position from the other Arduino.
 */
void receiveEvent(int howMany) {
  step_position = Wire.read();    // receive byte as an integer
}


/**
 * simple test of the tone() function
 */
void play_melody(){
    int melody[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5};
    for (int i = 0; i < 10; i++){
        tone(SOUND_PIN, frequencies[i], note_duration);
        delay(note_duration+100);
        noTone(SOUND_PIN);
    }
}
