#include "measure.h"

chrono::system_clock::time_point GENERAL_BEGIN_TIME;
chrono::system_clock::time_point GENERAL_END_TIME;
double GENERAL_TIME;

chrono::system_clock::time_point LCM_BEGIN_TIME;
chrono::system_clock::time_point LCM_END_TIME;
double LCM_TIME; // LCMは何度も呼ばれるからそれにかかった時間をここに合計して格納

/*
 @brief タイムに関わる変数を0で初期化
 */
void init_time(){
  GENERAL_TIME = 0;
  LCM_TIME = 0;
}

void algorithm_start(){
  GENERAL_BEGIN_TIME = chrono::system_clock::now();
}

void algorithm_restart(){
  GENERAL_BEGIN_TIME = chrono::system_clock::now();
}

void algorithm_stop(){
  GENERAL_END_TIME = chrono::system_clock::now();
  chrono::duration<double> sec = GENERAL_END_TIME - GENERAL_BEGIN_TIME;
  GENERAL_TIME += sec.count();
}

void algorithm_end(){
  GENERAL_END_TIME = chrono::system_clock::now();
  chrono::duration<double> sec = GENERAL_END_TIME - GENERAL_BEGIN_TIME;
  GENERAL_TIME += sec.count();
}

void LCM_start(){
  LCM_BEGIN_TIME = chrono::system_clock::now();
}

void LCM_end(){
  LCM_END_TIME = chrono::system_clock::now();
  chrono::duration<double> sec = LCM_END_TIME- LCM_BEGIN_TIME;
  LCM_TIME += sec.count();
}

double get_LCM_time(){return LCM_TIME;}
double get_algorithm_time(){return GENERAL_TIME;}
