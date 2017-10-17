#ifndef LCM
#define LCM

#include<iostream>
#include<vector>
#include "../tree.h"
#include "../file_io.h"

class Itemset{
 
public:
  vector<int> items;
  Itemset(vector<int> items);
  vector<int> get_int_vec(){return items;}
  Itemset* get_prefix(int i);
  int get_tail();
  bool is_included(vector<int> i);


  // @debug
  void print_items();
  
};

class ItemsetDB{
 
public:
  vector<Itemset*> itemsets;
  ItemsetDB(vector<vector<int>> itemsets);
  vector<int> get_occurrences(vector<int> items);
  vector<int> get_occ_intersection(vector<int> occ);
};

//お便利メソッド群 
vector<int> get_intersection(vector<int> a,vector<int> b);
//debug
void print_vec(vector<int> a);

vector<Itemset*> LCM_Algorithm(Itemset* fc_item,int min_sup,ItemsetDB* itemdb);

vector<CP*> Mine_Closed_Pattern(vector<vector<int>> itemsets,int min_sup);
vector<vector<int>> Mine_Closed_Itemsets(vector<vector<int>> itemsets,int min_sup);


#endif //LCM
