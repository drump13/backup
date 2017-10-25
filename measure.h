#ifndef MEASURE_H
#define MEASURE_H

#include<iostream>
#include<time.h>
#include<chrono>
#include "file_io.h"

using namespace std;
void init_time();
void algorithm_start();
void algorithm_restart();
void algorithm_stop();
void algorithm_end();
void LCM_start();
void LCM_end();
void set_num_of_root_candidate(int num);

double get_LCM_time();
double get_algorithm_time();
int get_num_of_root_candidate();
#endif //MEASURE_H
