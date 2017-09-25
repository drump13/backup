#include <iostream>
#include <vector>
#include "tree.h"
#include "file_io.h"
extern "C"{
  #include "lcm.h"
}

using namespace std;




Node* new_node(int id,int label){
  Node* newnode = new Node();
  newnode->label = label;
  newnode->id = id;
  return newnode;
}

Item* get_item(int label,bool is_parent,EnumerationTree* et,vector<Tree*> next_occurrences){
  Item* item = new Item();
  item->item_id = 0;
  item->label = label;
  item->is_parent = is_parent; 
  item->et = et;
  item->next_occurrences = next_occurrences;
  return item;		       
}


/*
木のvectorを受け取って、それぞれが持つラベルをキー,要素を木への参照リスト
にした連想配列を生成し返す．
 */
map<int,vector<Tree*>> get_label_map(vector<Tree*> tree_vec,int min_sup){
  map<int,vector<Tree*>> result;
  for(int i = 0, n = tree_vec.size();i<n;i++){
    if(tree_vec[i] != NULL){
      result[tree_vec[i]->get_node()->label].push_back(tree_vec[i]);
    }
  }
  
  //frequent じゃないものを削除
  for(auto itr=result.begin(); itr!= result.end();++itr){
    if(itr->second.size() < min_sup){
      result.erase(itr->first);
    }
  }
  return result;
}

/*
  EnumerationTreeを受け取ってその周りにつく頻出なItemリストを返す
*/
vector<Item*> collect_items(EnumerationTree* et,int min_sup){
  vector<Item*> result;
  //まずはEnumerationTreeのルートから
  if(et->get_parent() == NULL){
    vector<Tree*> parents = et->get_parent_occurrences();
    map<int,vector<Tree*>> label_map = get_label_map(parents,min_sup);
    for(auto itr = label_map.begin(); itr != label_map.end();++itr){
      result.push_back(get_item(itr->first,true,et,itr->second));
    }
  }
  //次に子を再帰で呼びながらアイテムを生成しresultに入れていく
  //todo
  vector<Tree*> chdren = et->get_children_occurrences();
  map<int,vector<Tree*>> chd_map = get_label_map(chdren,min_sup);
  for(auto itr = chd_map.begin();itr != chd_map.end();++itr){
    if(et->get_child(itr->first) == NULL){
      result.push_back(get_item(itr->first,false,et,itr->second));
    }
  }
  for(int i = 0,n = et->get_children().size() ; i < n ; i++){
    vector<Item*> rec_res = collect_items((EnumerationTree*) et->get_children()[i],min_sup);
    result.insert(result.end(),rec_res.begin(),rec_res.end());
  }

  if(et->get_parent() ==NULL){
    for(int i = 0,n=result.size(); i < n; i++ ){
      //LCMの都合上idは1から始める
      result[i]->item_id = i+1;
    }
  }


  return result;


}

/*
tree_vecの中から
指定されたlabelとet_idを持つものが存在するならtrue
しないならfalseを返す
 */
bool is_exist(vector<Tree*> tree_vec,EnumerationTree* et,int tree_id,int node_id){
  for(int i =0,n=tree_vec.size();i<n;i++){
    if(tree_vec[i]->get_tree_id() == tree_id && tree_vec[i]->get_node()->id == node_id){
      return true;
    }
  }
  return false;
}


/*
item_vecのnext_occurrenceに出てくる各Treeにindexを割り振る
 */
/*
vector<Tree*> get_tree_set(vector<Item*> item_vec){
  vector<Tree*> result;
  for(int i =0,n=item_vec.size();i<n;i++){
    for(int j =0,m=item_vec[i]->next_occurrences.size();j<m;j++){
      //      res.insert(item_vec[i]->next_occurrences[j]);
      Tree* next_tree = item_vec[i]->next_occurrences[j];
      if(!is_exist(result,next_tree->get_tree_id(),next_tree->get_node()->id)){
	result.push_back(next_tree);
      }
    }
  }
  cout <<"tree set size is " <<  result.size() << endl;
  cout << "--------------------" << endl;
  for(auto itr = result.begin();itr != result.end();++itr){
    cout << *itr << endl;
  }
  cout << "--------------------" << endl;
    
  return result;

}
*/
/*
tree_vecの中からtreeを検索、要素のindexを返す.
 */
int get_index(vector<Tree*> tree_vec,Tree* tree){
  vector<Tree*>::iterator itr;
  itr = find(tree_vec.begin(),tree_vec.end(),tree);
  return itr-tree_vec.begin();
}

/*
item_vecのnext_occurrencesをindexのvectorに変換
 */
vector<vector<int>> convert_item_vec(vector<Item*> item_vec){
  vector<vector<int>> dummy;
  if(item_vec.size()==0){return dummy;}
  cout << "item 0's occ list size is" << item_vec[0]->et->get_occurrence_list().size() << endl;
  vector<vector<int>> result(item_vec[0]->et->get_occurrence_list().size());
  cout <<"item vec size is:"<< item_vec.size() << endl;
  //  vector<Tree*> basic_tree_set = get_tree_set(item_vec);
  map<int,vector<int>> transaction_map;
  for(int i = 0,n = item_vec.size();i<n;i++){
    for(int j = 0,m = item_vec[i]->et->get_occurrence_list().size() ; j < m;j++){
      if(item_vec[i]->et->get_occurrence_list()[j]->get_parent() != NULL && 
	 item_vec[i]->et->get_occurrence_list()[j]->get_parent() -> get_node()->label == item_vec[i]->label){
	result[j].push_back(item_vec[i]->item_id);
	//rs.push_back(item_vec[i]->item_id);
      }else{
	if(item_vec[i]->et->get_occurrence_list()[j]->get_child(item_vec[i]->label) != NULL){
	  result[j].push_back(item_vec[i]->item_id);
	    ///rs[j].push_back(item_vec[i]->item_id);
	}
      }
      
    }
    /*
   if(item_vec[i]->is_parent){
      //      result.push_back(item_vec[i]->et->get_occurrence_parent_ids(item_vec[i]->label));
     }else{
      result.push_back(item_vec[i]->et->get_occurrence_ids(item_vec[i]->label));
    }
    */
  }

  
  return result;
}



void copy_enumeration_tree_rec(EnumerationTree* et,EnumerationTree* parent){
  EnumerationTree* copy = new EnumerationTree(et);
  parent->add_child(copy);
  copy->set_parent(parent);
  for(int i = 0 ,n = et->get_children().size();i<n;i++){
    copy_enumeration_tree_rec((EnumerationTree*)et->get_children()[i],copy);
  }

}
/*
enumeration treeのディープコピーを返す
 */
EnumerationTree* copy_enumeration_tree(EnumerationTree* et){
  EnumerationTree* copy = new EnumerationTree(et);
  for(int i = 0 ,n = et->get_children().size();i<n;i++){
    copy_enumeration_tree_rec((EnumerationTree*) et->get_children()[i],copy);
  }

  return copy;
}


/*
  次拡張するべき情報をitem_vectorを渡して、それを基にenumeration_treeを拡張
  第一引数はenumeration treeのrootが渡される
*/
EnumerationTree* expand_enumeration_tree(EnumerationTree* root,vector<Item*> items){
  //todo
  EnumerationTree* result = root;
  
  for(int i = 0,n = items.size() ; i < n;i++){
    if(items[i]->is_parent){
      EnumerationTree* new_root = new EnumerationTree(0,items[i]->label,items[i]->next_occurrences);
      new_root->add_child(root);
      root->set_parent(new_root);
      result = new_root;
    }else{
      EnumerationTree* new_node = new EnumerationTree(0,items[i]->label,items[i]->next_occurrences);
      EnumerationTree* new_node_parent = (EnumerationTree*) root->get_tree(items[i]->et->get_node()->id);
      new_node_parent->add_child(new_node);
      new_node->set_parent(new_node_parent);
    }
  }
  return result;
}

Item* get_min_occ_item(vector<Item*> cis){
  int current_occ_size = cis[0]->next_occurrences.size();
  Item* result = cis[0];
  for(int i = 1,n=cis.size() ; i  < n ;i++){
    if(cis[i]->next_occurrences.size() < current_occ_size){
      current_occ_size = cis[i]->next_occurrences.size();
      result = cis[i];
    }
  }
  return result;
}

/*
  対応するindex_listを返す
*/
vector<int> get_correspond_id_list(vector<Item*> cis,EnumerationTree* current_et){
  vector<int> index_list;
  Item* min_item = get_min_occ_item(cis);
  if(!min_item->is_parent){ 
    vector<Tree*> tv = min_item->next_occurrences;
    cout << "print tv " << tv.size() << endl;
    print_tree_vector(tv);
    vector<Tree*> ptv = min_item->et->get_root_occurrence_list(tv);
    cout << "print ptv" << ptv.size() << endl;
    print_tree_vector(ptv);
    //current_etが持っているoccurrence_listに対応するindexのリストを計算
    index_list = get_correspond_index_list(current_et->get_occurrence_list(),ptv);
    
  }else{
    index_list = min_item->et->get_occurrence_parent_ids(min_item->label);
  }
  return index_list;
}
    
//debug    
void print_item_vector(vector<Item*> item_vec){
  for(int i =0,n = item_vec.size();i<n;i++){
    cout << " id = " << item_vec[i]->item_id << " (label : " << item_vec[i]->label
	 << " parent_flag:" << item_vec[i]->is_parent 
	 << "occurrence size:" << item_vec[i]->next_occurrences.size() <<")" 
	 << "this enumeration tree's label is " << item_vec[i]->et->get_node()->label <<  endl;
    print_tree_vector(item_vec[i]->next_occurrences);

  }
}


//debug 
void print_transactions(vector<vector<int>> transactions){
  cout << "transactions size is" << transactions.size() << endl;
  for(int i = 0,n = transactions.size(); i < n;i++){
    for(int j = 0,m = transactions[i].size();j<m;j++){
      cout << transactions[i][j] << " ";
    }
    cout << endl;
  }
}


//debug 
void print_index_list(vector<int> index_list){
  cout <<"index_list size is " <<index_list.size() << endl;
  for(int i =0, n = index_list.size(); i<n;i++){
    cout << index_list[i] << ",";
  }
  cout << endl;
}



/************************
このアルゴリズムの本体
AttributeTreeDBとConstrainedTreeを入力にして
ConstrainedTreeを含むClosedTreeを全て列挙する．
**************************/
vector<EnumerationTree*> SCCA_Miner(TreeDB* attribute_trees,EnumerationTree* constrained_tree, int minimum_support){
  vector<EnumerationTree*> closed_trees;
  vector<Tree*> trees = attribute_trees->get_subtree_list(constrained_tree);
  //constrained_tree->add_occurrence_list(trees);
  EnumerationTree* closed_tree = constrained_tree->expand_occurrence_matched();
  //解の1つを列挙 
  cout << " hogehoge " << endl;
  closed_tree->print_tree();
  closed_trees.push_back(closed_tree);
  
  //enumeration_treeのreindexingは必須
  closed_tree->reindexing(0);
  vector<Item*> items = collect_items(closed_tree,minimum_support);
  if(items.size() == 0){return closed_trees;} 
  //  print_item_vector(items);

  vector<vector<int>> transactions = convert_item_vec(items);

  //LCM
  // cout << "----------calling LCM---------" << endl;
  //print_transactions(transactions);
  //cout << "----- input end " << endl;
  write_file_to_item_transactions(transactions);
  //cout << "min_sup = " << minimum_support << endl;

  Mine_Closed_Itemsets(minimum_support);

   transactions = read_result_of_lcm();  
   //print_transactions(transactions);
  // cout << "-------------LCM end---------------" << endl;
  //closed_tree->print_tree();

  //LCMからの結果を基に次のClosedTreeを求める  
  for(int i = 0,n=transactions.size() ; i < n;i++){
    vector<Item*> cis;
    
    for(int j = 0, m =transactions[i].size();j<m;j++){
      cis.push_back(items[transactions[i][j]-1]);
    }
    EnumerationTree* current_et = copy_enumeration_tree(closed_tree); 
    current_et = expand_enumeration_tree(current_et,cis);
    vector<int> index_list = get_correspond_id_list(cis,current_et);
    //cis[0]にparentに拡張するものがあれば入っているハズ
    //cout << "print_index_list" << endl;
    current_et->reindexing(0);
    //    print_index_list(index_list);

    //cout << "before update" << endl;
    //current_et->print_tree();
    print_index_list(index_list);
    current_et->update_occurrence_list(index_list);
    //cout << "after update" << endl;
    //current_et->print_tree();

    vector<EnumerationTree*> cts = SCCA_Miner(attribute_trees,current_et,minimum_support);
    closed_trees.insert(closed_trees.end(),cts.begin(),cts.end());
  }



  return closed_trees;
}

