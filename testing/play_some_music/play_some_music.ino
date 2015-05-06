#include "pitches.h";

int SOUND_PIN 9;

unsigned long note_duration = 1000;    // in milliseconds

// note frequencies to play corresponding to the 12 different laser beams
int frequencies = {NOTE_A3, NOTE_B3, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5};

void setup(void) {
    pinMode(SOUND_PIN, OUTPUT);

    play_melody();
}

void loop() {
    boolean changed = false;
    increment_mirror();
    changed = monitor_change();
    if (monitor_change()){
        play(step_position);
    }
}


void play(int step_position) {
    // play the note corresponding to this laser beam
    tone(SOUND_PIN, notes[step_position], note_duration)
}


void play_melody(){
    // simple test of the tone() function
    // tone(pin, frequency(Hz), [duration(ms)]) & notone(pin)
    int melody[] = {NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
    int fraction_of_note = {4,8,8,4,4,4,4,4}; // 4 = quarter note, 8 = eight note
    for (int i = 0; i < 8; i++){
        int duration = note_duration/fraction_of_note[i];
        tone(SOUND_PIN, melody[i], duration);
        delay(duration+250);
        notone(SOUND_PIN);
    }
}


void play2(int step_position) {
    tone2(SOUND_PIN, frequencies[step_position], note_duration);
}

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