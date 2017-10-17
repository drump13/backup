#include <iostream>
#include <vector>
#include "tree.h"
#include "file_io.h"
//#include "SCCA_Miner.cpp"
//#include "SCC_Path_Miner.cpp"
#include "SCC_Miner.cpp"
#include "lcm_cpp/lcm.h"



static int MIN_SUP = 2;

int main(){
  // vector<string> sv = split("0 3 3 -1 3 -1 -1 4",' ');
  
  //  Tree* tree = new Tree(sv,0,NULL);

  Tree* tree1 = make_tree("0 2 3 6 -1 -1 1 5 -1 -1 4 5 -1 -1 6 -1 -1 -1");
  Tree* tree2 = make_tree("2 6 -1 3 5 -1 -1 1 5 -1 -1 4 6 -1 -1 -1");
  Tree* tree3 = make_tree("0 2 3 -1 1 5 -1 -1 4 -1 2 4 6 -1 -1 1 5 -1 -1 -1 -1 -1");
  TreeDB* db = new TreeDB();
  db->add_tree(tree1);
  db->add_tree(tree2);
  db->add_tree(tree3);
  db->set_tree_id();
  db->print_tree_db();
  Tree* subtree = make_tree("1 5 -1 -1");
  vector<Tree*> trees = db->get_subtree_list(subtree);
  
  EnumerationTree* et = gen_enumeration_tree(subtree,NULL,trees); 
  //  et->print_tree();
  //cout << " --------------------" << endl;
  //et = et->expand_occurrence_matched();
  //cout << "ok" << endl;
  //et->print_tree();
//cout << tree->is_subtree(subtree) << endl;
   //cout << "enumeration_tree" << endl;
  //  et->print_tree();

  cout << "SCC_Miner start !!!!!" << endl;

  // vector<EnumerationTree*> result = SCC_Miner(db,et,MIN_SUP);
  vector<EnumerationTree*> result = SCC_Miner_Improved(db,et,MIN_SUP);
  cout << "closed tree size is" << result.size() << endl; 
  // result = SCC_Path_Miner(db,et,MIN_SUP);
  result = SCC_Miner(db,et,MIN_SUP);
  cout << "closed tree size is " << result.size() << endl;
  for(int i = 0 , n = result.size() ; i<n; i++){
    cout << result[i]->get_tree_string() << endl;
    cout << " ------- " << endl;
}
  //  cout << " --";
  //result[6]->print_tree();
  
    //  write_final_result(result);
  /*  Tree* tree4 = make_tree("0 1 2 -1 -1 3 2 -1 -1 -1");
  Tree* tree5 = make_tree("0 1 2 -1 -1 3 2 -1 -1 -1");
  TreeDB* db2 = new TreeDB();
  db2->add_tree(tree4);
  db2->add_tree(tree5);
  db2->set_tree_id();

  subtree = make_tree("2 -1");
  trees = db2->get_subtree_list(subtree);
  EnumerationTree* et2 = gen_enumeration_tree(subtree,NULL,trees); 
  result = SCC_Miner_Improved(db2,et2,MIN_SUP);
  

  */


  /*  
  //cs_logs exp  
  vector<string> cs = read_cslogs();
  cout << "cs_logs tree num is " << cs.size() << endl;

  TreeDB* csdb = new TreeDB();
  for(int i = 0 , n = cs.size();i<n;i++){
    //    cout << cs[i] << endl;
    csdb->add_tree(make_tree(cs[i]));    
  }
  csdb->set_tree_id();



  subtree = make_tree("1 3 -1 -1");
  trees = csdb->get_subtree_list(subtree);
  cout <<"trees size is " << trees.size() << endl;
OA  //print_tree_vector(trees);
  et = gen_enumeration_tree(subtree,NULL,trees);
  //  result = SCCA_Miner(csdb,et,MIN_SUP);
  //cout << result.size()<<endl;   
  
  //  vector<Tree*> oc_list = db -> get_subtree_list(subtree);
  cout << "calling SCC_Path_Miner" << endl;
  //SCC_Path_Miner(db,et,MIN_SUP);
  SCC_Path_Miner(csdb,et,MIN_SUP);
  cout << "end SCC_Path_Miner" << endl;
  //  vector<int> id_list = make_initial_id_list(oc_list);
    //  vector<PathItem*> piv =  make_transactions(oc_list,id_list,to_string(subtree->get_node()->label),MIN_SUP);
    //  cout << "path item vec size is "  << piv.size() << endl;
  // print_path_item_vector(piv);
  */  

  return 0;		
}



