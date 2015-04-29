#ifndef LASER_H
#define LASER_H

void calibrate_light();
void calibrate_stepper();
void stepper_move(int dir);
void increment_mirror();
boolean monitor_change();
void play(boolean changed);
double get_lux();
void printError(byte error);

#endif
