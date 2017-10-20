#include<iostream>

extern "C"{
#include "lcm.h"
};
static int MIN_SUP = 5;
int main(){

  Mine_Closed_Itemsets(MIN_SUP);
  return 0;

}
