#include"lcm.h"

int main(){
  vector<int> test0{0,1,2,3,4,6};
  vector<int> test1{0,1,2,3,4,5,6};
  vector<int> test2{0,1,2,3,4};
  vector<int> test3{0,1,2,4,5};
  
  vector<vector<int>> testdb;
  testdb.push_back(test0);
  testdb.push_back(test1);
  testdb.push_back(test2);
  testdb.push_back(test3);
  ItemsetDB* test_item_db = new ItemsetDB(testdb);
  
  vector<int> test_occ{0,1,2,3};
  print_vec(test_item_db->get_occ_intersection(test_occ));
  cout << "general item set" << endl;
  test_item_db->print_general_itemset();
  
  vector<int> test_item{1,2,5};
  cout << "get minimum item of same occurrenes : ";
  cout << test_item_db->get_minimum_item_of_same_occurrence(test_item) << endl;

  vector<int> init;
  Itemset* i = new Itemset(init);
  LCM_Algorithm(i,2,test_item_db);
  //print_Itemset_vec(FRCL_PATTERN);
  vector<vector<int>> closed =  Mine_Closed_Itemsets(testdb,2);
  print_v_v(closed);
  return 0;

}
