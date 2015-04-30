#include "pitches.h";

int SOUND_PIN 9;

unsigned int = frequency;   // in hertz
unsigned long duration = ;  // in microseconds

void setup(void) {
    pinMode(SOUND_PIN, OUTPUT);

    // simple test of the tone() function
    // tone(pin, frequency(Hz), [duration(ms)]) & notone(pin)
    int melody[] = {NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
    int note_durations = {4,8,8,4,4,4,4,4}; // 4 = quarter note, 8 = eight note
    for (int i = 0; i < 8; i++){
        int note_duration = 1000/note_durations[i];
        tone(SOUND_PIN, melody[i], note_duration);
        delay(note_duration+250);
        notone(SOUND_PIN);
    }
}

void loop() {
    boolean changed = false;
    increment_mirror();
    changed = monitor_change();
    play(changed);
}


void play(bool changed, int step_position) {
    frequency = get_frequency(step_position);
    play_tone(frequency, duration);
}

void play_tone(int frequency, int duration){
    if (frequency == 0.0){
        delayMicroseconds(duration);
    }
    else {
        // calculate the half period in microseconds
        int half_period = (int)(1.0E6/(2*frequency));
        // calculate the number of cycles for duration
        int nCycles = (int)(duration*frequency/1.0E6);
        for (int i=0; i < nCycles; i++){
            digitalWrite(SOUND_PIN,HIGH);
            delayMicroseconds(half_period);
            digitalWrite(SOUND_PIN,LOW);
            delayMicroseconds(half_period);
        }        
    }
    
}

int get_frequency(char step_position){
    if (step_position = 0){
        return 261;
    }
    else if (step_position = 1){
        return 294;
    }
    else if (step_position = 2){
        return 329;
    }
    else if (step_position = 3){
        return 349;
    }
    else if (step_position = 4){
        return 392;
    }
    else if (step_position = 5){
        return 440;
    }
    else if (step_position = 6){
        return 493;
    }
    else {
        return 523;
    }

}