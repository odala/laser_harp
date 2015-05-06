#ifndef LASER_H
#define LASER_H

void setup();
void calibrate_light();

void loop();
void increment_mirror();
void stepper_move(int dir);
void record_light();
void max_index(int array[], int size_a);
void detect();
void transmit(int data);

#endif

