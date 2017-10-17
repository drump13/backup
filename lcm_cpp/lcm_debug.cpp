#include"lcm.h"

int main(){
  vector<int> test0{1,2,4,6};
  vector<int> test1{1,6,9};
  vector<int> test2{1,2,5};  
  vector<int> test3{1,2,9};
  
  vector<vector<int>> testdb;
  testdb.push_back(test0);
  testdb.push_back(test1);
  testdb.push_back(test2);
  testdb.push_back(test3);
  ItemsetDB* test_item_db = new ItemsetDB(testdb);
  
  vector<int> test_occ{0,1,2,3};
  print_vec(test_item_db->get_occ_intersection(test_occ));
  
  return 0;

}
