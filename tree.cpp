
#include "tree.h"
// using namespace std;


/***************************************
以下木クラスの実装
****************************************/

Tree::Tree(int id, int label ){
  Node* node = new Node();
  node->label = label;
  node->id = id;
  this->node = node;
  parent = NULL;
}
 /*
 コピーコンストラクタ
 ノードを渡す事でそのノードを根にした部分木を生成
 */
Tree::Tree(Tree* root){
  //todo
}

 /*
   @brief 標準形文字列を渡すとその木を生成する
   @param canonical_form
 */
Tree::Tree(string canonical_form){
  cout << "here is Tree(string) 1" << endl;
  node = new Node();
  vector<string> sv = split(canonical_form,' ');
  node->label = stoi(sv[0]);
  add_child(new Tree(0,sv,NULL));
  cout << "here is Tree(string) 2" << endl;
  reindexing(0);
}

/*
  @brief XMLのタグ列を渡すとその木を生成する．
  @param tag_vec タグのベクター
  @param exception_root 根
  @detail
  xmlが1本の巨大な木になっている場合はexception_rootを考えずに森と考えることがある．
 */
Tree::Tree(vector<string> tag_vec,string exception_root){
  //todo
}

Tree::Tree(int current_index,vector<string> sv,Tree *parent){
  int svi = stoi(sv[current_index]);
  this->parent = parent;
  if(current_index >= sv.size()){
    return ;
  }
  if(svi == -1){
    Tree t =new Tree(current_index+1,sv,this->parent);


  }else{
    node = new Node();
    node-> id = 0;		
    node->label = svi;
    add_child(new Tree(current_index+1,sv,this));
    
  }

}






Tree::~Tree(){

}


Node* Tree::new_node(int lab){
	Node* newnode;
	newnode = new Node();
	newnode->label = lab;
	newnode->id = 0;
	return newnode;

}

Node *Tree::get_node(){
  return node;
}

Tree *Tree::get_parent(){
  return parent;
}

void Tree::set_children(vector<Tree*> c){
  children = c;
}

vector<Tree*> Tree::get_children(){
  return children;
}

//ノードidを指定して、対応する木を返す．
Tree *Tree::get_tree(int node_id){
  int n = children.size();
  if(node->id == node_id){
    return this;
  }
  Tree* t = NULL;
  for(int i = 0;i < n;i++){
    if(t != NULL){
      return t; 
    }
    t = children[i]->get_tree(node_id);
  }
  return t;

}


//現在の子供の最も右の子として新たに追加
void Tree::add_child(Tree *new_child){
  children.push_back(new_child);
  new_child->set_parent(this);
}

/*
親ノードを指定して，そのノードの最も右の子として新たなノードを付ける
*/
void Tree::add_node(Tree *parent,Node *new_node){
  Tree *new_tree = new Tree(new_node->id,new_node->label);
  children.push_back(new_tree);
  new_tree->set_parent(this);

}
//この木の親としてtreeをセットする
void Tree::set_parent(Tree* tree){
  parent = tree;
}
int Tree::reindexing(int current){
  node->id = current;
  int c = current;
  for(int i = 0 ; i < children.size();i++){
    c = children[i]->reindexing(c+1);
  }
  return c;
}
/*
@brief 任意の無順序木をattribute treeに変換 
@detail
同じラベルを持つノードが複数現れるとき，より左に現れるノードとその子孫を削除
 */
void Tree::to_attribute_tree_by_cutting(){
  map<int,Tree*> label_map;
  for(int i = 0,n=children.size(); i<n;i++){
    label_map[children[i]->get_node()->label] = children[i];
  }
  vector<Tree*> new_children;
  for(auto itr = label_map.begin(); itr!=label_map.end();++itr){
    new_children.push_back(itr->second);
  }
  children = new_children;
  for(Tree* c :children){
    c->to_attribute_tree_by_cutting();
  }
}

/*
@brief 任意の無順序木をattribute treeに変換
@detail
同じラベルを持つ子ノードは，その孫ノードを1つにまとめることによってattribute treeとする．
 */
void Tree::to_attribute_tree_by_merging(){
  //todo
  map<int,vector<Tree*>> label_map;
  for(int i = 0 , n = children.size();i<n;i++){
    label_map[children[i]->get_node()->label].push_back(children[i]);
  }
  vector<Tree*> new_children;
  for(auto itr = label_map.begin(); itr != label_map.end();++itr){
    if(itr->second.size() > 1){
      Tree* t = new Tree(get_node()->id,get_node()->label);
      t->set_parent(parent);
      for(int i = 0 , n  = itr->second.size();i<n;i++){
	for(int j = 0, m = itr->second[i]->get_children().size();j<m;j++){
	  t->add_child(itr->second[i]->get_children()[j]);
	}
      }
      new_children.push_back(t);
    }else{
      new_children.push_back(itr->second[0]);
    }
  }
  children = new_children;
  for(Tree* c :children){
    c->to_attribute_tree_by_merging();
  }

}
  
void Tree::print_tree(){
  //string result = "";
  //  result = get_string();
  //cout << result << endl;
  cout << node->id << ":" << node->label << " "<<endl;
  for(int i = 0 ; i < children.size();i++){
    children[i]->print_tree();
    
  }
  cout << -1 << " "<<endl;
}

string Tree::get_string(){
  stringstream s;
  s << node->id;
  s << ":";
  s << node->label;
  string result = s.str();
  for(int i = 0;i < children.size();i++){
    result = result + " ";
    result = result+children[i]->get_string();
  }
  result = result+" -1";
  return result;
}

/*
引数の木がこのオブジェクトの部分木になっているかどうか判定
attribute-treeにのみ対応している
*/
bool Tree::is_subtree(Tree* tree){
  vector<Tree*> ch = tree->get_children();
  int ch_size = ch.size();
  bool result = true;
  if(ch_size==0){return true;}
  for(int i=0;i < ch_size;i++){
    int n = children.size();
    if(ch_size > n){return false;}
    for(int j =0;j<n;j++){
      if(ch[i]->get_node()->label == children[j]->get_node()->label){
	result &= children[j]->is_subtree(ch[i]);
	if(!result){return false;}
	break;
      }
      if(j == n-1){
	return false;
      }
    }
  }
  return result;
}

/*
引数のtreeのサブパターンを列挙
 */
vector<Tree*> Tree::enumerate_subtree(Tree* tree){
  vector<Tree*> result;
  if(node->label == tree->get_node()->label && is_subtree(tree)){
    result.push_back(this);
  }
  for(int i = 0,n=children.size() ; i < n;i++){
    vector<Tree*> rec_res = children[i]->enumerate_subtree(tree); 
    result.insert(result.end(),rec_res.begin(),rec_res.end());
  }

  return result;
}

Tree* Tree::get_child(int label){
  for(int i =0,n=children.size();i<n;i++){
    if(children[i]->get_node()->label == label){
      return children[i];
    }
  }
  return NULL;
}

Tree* Tree:: get_root(){
  Tree* current = this;
  while(current->parent != NULL){
    current = current->parent;
  }
  return current;
}

int Tree::get_num_of_nodes(){
  int result = children.size();
  for(int i = 0 , n = children.size();i<n;i++){
    result += children[i]->get_num_of_nodes();
  }
  return result;
}

void Tree::update_tree_id(int id){
  tree_id = id;
  for(int i =0,n=children.size();i<n;i++){
    children[i]->update_tree_id(id);
  }
}


int Tree::get_tree_id(){
  return tree_id;
}

string Tree::get_tree_string(){
  string s = to_string(node->label) + " ";
  //  cout << s << endl;
  for(int i =0,n=children.size();i<n;i++){
    s = s + children[i]->get_tree_string();
  }
  return s + "-1 ";
}

//debug
void Tree::print_node(){
  cout <<"tree_id:" << tree_id  << " node_id:" << node->id << endl;
}


EnumerationTree::EnumerationTree(int id ,int label):Tree(id,label){
  //todo
}

EnumerationTree::EnumerationTree(int id,int label,vector<Tree*> occ_list):Tree(id,label){
  occurrence_list = occ_list;
}

//shallow copy
EnumerationTree::EnumerationTree(EnumerationTree* et):Tree(et->get_node()->id,et->get_node()->label){
  for(int i = 0,n = et->get_occurrence_list().size();i<n;i++){
    occurrence_list.push_back(et->get_occurrence_list()[i]);
  }

}


void EnumerationTree::add_occurrence_list(vector<Tree*> tree_vec){
  vector<Tree*> next;
  for(int i =0,n = tree_vec.size();i<n;i++){
    next.push_back(tree_vec[i]);
  }
  occurrence_list = next;

}


EnumerationTree* EnumerationTree::expand_occurrence_matched(){
  //todo
  EnumerationTree* result = exp_om_parent(); 
  result->exp_om_children();
  return result;
}
EnumerationTree* EnumerationTree::exp_om_parent(){
  if(occurrence_list[0]->get_parent() == NULL){return this;}
  int next_label = occurrence_list[0]->get_parent()->get_node()->label;
  vector<Tree*> next_occ_list;
  next_occ_list.push_back(occurrence_list[0]->get_parent());
  for(int i = 1,n=occurrence_list.size();i<n;i++){
    bool flag  = occurrence_list[i] -> get_parent() == NULL;
    if(occurrence_list[i]->get_parent()==NULL){return this;}
    if(occurrence_list[i]->get_parent()->get_node()->label != next_label){
      return this;
    }
    next_occ_list.push_back(occurrence_list[i]->get_parent());
  }

  EnumerationTree* next_parent = new EnumerationTree(0,next_label,next_occ_list);
  parent = next_parent;
  next_parent->add_child(this);
  return next_parent->exp_om_parent();
  
}

//子孫を探索し、occurrence_matchなノードをEnumeration_treeに追加していく
void EnumerationTree::exp_om_children(){
  map<int,int> label_map;
  for(int i = 0,n=occurrence_list[0]->get_children().size() ; i <n;i++){
    label_map[occurrence_list[0]->get_children()[i]->get_node()->label] = 1;
  }

  //0番目の子に存在したラベルに1を追加
  for(int i = 1,n=occurrence_list.size() ; i < n;i++){
    vector<Tree*> current_children = occurrence_list[i]->get_children();
    for(int j = 0,m=current_children.size() ;j<m;j++){
      auto itr = label_map.find(current_children[j]->get_node()->label);
      if(itr!=label_map.end()){
	label_map[current_children[j]->get_node()->label]++;
      }
    }
  }
  
  for(auto itr =label_map.begin(); itr != label_map.end();++itr){
    if(occurrence_list.size() == itr->second){
      bool flag = get_child(itr->first) != NULL;
      if(get_child(itr->first) != NULL){
	EnumerationTree* c = (EnumerationTree*) get_child(itr->first);
      	c->exp_om_children();
	
      }else{
	vector<Tree*> new_child_occ = make_new_child_occ(itr->first);
	EnumerationTree *new_child = new EnumerationTree(0,itr->first,new_child_occ);
	children.push_back(new_child);
	new_child->set_parent(this);
	new_child->exp_om_children();
      }
    }
  }
  
}

//labelを指定してそのラベルを持つ子のocc_listを作成
vector<Tree*> EnumerationTree::make_new_child_occ(int label){
  vector<Tree*> result;
  for(int i = 0 ,n = occurrence_list.size();i<n;i++){
    result.push_back(occurrence_list[i]->get_child(label));
    
  }
  return result;
}

vector<Tree*> EnumerationTree::get_parent_occurrences(){
  vector<Tree*> result;
  for(int i = 0,n = occurrence_list.size() ;i<n;i++ ){
    if(occurrence_list[i]->get_parent()!= NULL){
    result.push_back(occurrence_list[i]->get_parent());
    }
  }
  return result;
}


/*
occurrence_listに存在しているノードの子を全てvectorに入れて返す
 */
vector<Tree*> EnumerationTree::get_children_occurrences(){
  vector<Tree*> result;
  for(int i = 0,n=occurrence_list.size();i<n;i++){
    vector<Tree*> chdren = occurrence_list[i]->get_children();
    for(int j =0,m=chdren.size();j<m;j++){
      result.push_back(chdren[j]);
    }
  }
  return result;

}

vector<Tree*> EnumerationTree::get_occurrence_list(){
  return occurrence_list;
}
/*
  @brief ラベルを指定して、そのラベルを持つ親を持つoccurrenceのindexを全て格納して返す
 */
vector<int> EnumerationTree::get_occurrence_parent_ids(int label){
  vector<int> occurrence_ids;
  for(int i =0,n=occurrence_list.size();i < n; i++){
    if(occurrence_list[i]->get_parent() == NULL){continue;}
    if(occurrence_list[i]->get_parent()->get_node()->label == label){
      //lcmの都合上、indexに1加える
      //occurrence_ids.push_back(i+1);
      occurrence_ids.push_back(i);
    }
  }
  return occurrence_ids;
}


/*
ラベルを指定して、そのラベルを持つ子供を持つoccurrenceのindexを全て格納して返す
 */
vector<int> EnumerationTree::get_occurrence_ids(int label){
  vector<int> occurrence_ids;
  for(int i = 0,n = occurrence_list.size(); i < n;i++){
    if(occurrence_list[i]->get_child(label) != NULL){
    //lcmの都合上、idに1を加えて返す
      //      occurrence_ids.push_back(i+1);
      occurrence_ids.push_back(i);
    }
  }
  return occurrence_ids;
}


/*
indexのリストを渡して、そのoccurrence_listの要素からそのインデックスの
要素を抜き出して保存
 */
void EnumerationTree::update_occurrence_list(vector<int> list){
  vector<Tree*> new_occurrence_list;
  if(list.size() == occurrence_list.size() && parent!= NULL){
    //何もしない
  }else if(list.size() < occurrence_list.size()){
    for(int i =0,n=list.size();i < n;i++){
      new_occurrence_list.push_back(occurrence_list[list[i]]);
    }
    occurrence_list = new_occurrence_list;
  }/*else{
    return;
    }*/

  for(int i =0,n=children.size();i<n;i++){
    ((EnumerationTree*) children[i])->update_occurrence_list(list);
  }
}


/*
そのEnumerationTreeのoccurrence_listがその根ノードのどのoccurrence_list
の要素に対応するのか返す
渡されるのは呼ばれたEnumerationTreeのoccurrence_list
 */
vector<Tree*> EnumerationTree::get_root_occurrence_list(vector<Tree*> tree_vec){
  vector<Tree*> new_tree_vec;
  for(int i = 0,n = tree_vec.size() ; i < n ; i++){
    if(tree_vec[i]->get_parent()!= NULL){
      new_tree_vec.push_back(tree_vec[i]->get_parent());
    }
  }
  if(parent != NULL){
    return ((EnumerationTree*)parent)->get_root_occurrence_list(new_tree_vec);
  }else{
    return new_tree_vec;
  }
}

/*
  @brief ラベルを指定してそのラベルの子を削除
  @param label
  @detail
  実際は新しいchildrenにlabelを持つ子以外を追加することで実装する
*/
void EnumerationTree::remove_child(int label){
  vector<Tree*> new_children;
  for(int i = 0,n = children.size();i<n;i++){
    if(children[i]->get_node()->label != label){
      new_children.push_back(children[i]);
    }
  }
  set_children(new_children);

}


//debug
void EnumerationTree::print_tree(){
  cout << node->id << ":" << node->label << " "<< occurrence_list.size()<<endl;
  print_occurrence_list();
  for(int i = 0 ; i < children.size();i++){
    children[i]->print_tree();
    
  }
  cout << -1 << " "<<endl;
}
//debug
void EnumerationTree::print_occurrence_list(){
  cout << "[";
  for(int i = 0,n=occurrence_list.size() ; i < n;i++){
    cout << occurrence_list[i]->get_tree_id() << ":" << occurrence_list[i]->get_node()->id <<",";
  }
  cout << "]" << endl;
}



/*****************************************
Treeクラスを持ったTreeDBクラス，
各アルゴリズムとはこのクラスと直接やり取りする事になる
****************************************/

TreeDB::TreeDB(){
}



void TreeDB::add_tree(Tree* tree){
	treedb.push_back(tree);
}

//木のidとノードのidを指定して、対応する木を取ってくる
Tree* TreeDB::get_tree(int tree_id,int node_id){
  Tree *root = treedb[tree_id];
  return root->get_tree(node_id);
}

bool TreeDB::is_frequent(Tree* tree,int minimum_sup){
//todo
  return false;
}

vector<Tree*> TreeDB::get_subtree_list(Tree* subtree){
  vector<Tree*> result;
  for(int i = 0,n=treedb.size() ; i < n;i++){
    vector<Tree*> trees = treedb[i]->enumerate_subtree(subtree);
    result.insert(result.end(),trees.begin(),trees.end());
  }

  return result;
}

/*
@brief TreeDB中の木全てをattributeTreeに変換
@sa Tree::to_attribute_tree_by_cutting
 */
void TreeDB::to_attribute_tree_by_cutting(){
  for(int i = 0 ,n = treedb.size();i<n;i++){
    treedb[i]->to_attribute_tree_by_cutting();
  }
}

/*
@brief TreeDB中の木全てをattributeTreeに変換
@sa Tree::to_attriute_tree_by_merging
*/
 void TreeDB::to_attribute_tree_by_merging(){
   for(int i = 0 ,n = treedb.size();i<n;i++){
     treedb[i]->to_attribute_tree_by_merging();
   }
   
 }


/*
@brief TreeDB中のノードの数を返す
@sa Tree::get_num_of_nodes
 */
int TreeDB::get_num_of_nodes(){
  int result = 0;
  for(int i = 0, n= treedb.size() ;i<n;i++){
    result += treedb[i]->get_num_of_nodes();
  }
  return result;
}

/*
@brief DB中の木の数を返す
 */
int TreeDB::get_num_of_trees(){
  return treedb.size();
}

/*
@brief DB中のi番目の木の根を返す
@param i
 */
Tree* TreeDB::get_tree(int i){
  return treedb[i];
}

void TreeDB::set_tree_id(){
  for(int i =0,n=treedb.size();i<n;i++){
    treedb[i]->update_tree_id(i);
  }
}

void TreeDB::print_tree_db(){
  for(int i = 0 ,n=treedb.size(); i < n ; i++){
    cout <<"----- tree id is "<<treedb[i]->get_tree_id()<<"-----"  <<endl;

    treedb[i]->print_tree();
  }
}

/***************** RPTreee ******************/

RPTree::RPTree(int label,vector<Tree*> occ_list):EnumerationTree(0,label,occ_list){
  for(int i = 0 , n = occ_list.size();i<n;i++){
    item_list.push_back(i);
  }
}

RPTree::RPTree(int label,vector<Tree*> occ_list,vector<int> item_list):EnumerationTree(0,label,occ_list){
  for(int i = 0,n=item_list.size();i<n;i++){
    this->item_list.push_back(item_list[i]);
  }
}

/*
  @brief このノード以下の木を削除
  @return
   */
void RPTree::rm_dec(){
  ((RPTree*)parent)->remove_child(get_node()->label);
  parent = NULL;
}

/*
  @brief 以下のノードのパスとOccurrence_listのペアのリストを返す
  @param label_path　外から渡されるときは空のベクターを引数に取る
  @detail
*/
vector<Path_OCCL*> RPTree::get_POCCL_list(vector<int> label_path){
   vector<Path_OCCL*> result;
    vector<int> s;
    if(label_path.size() == 0){
      s.push_back(get_node()->label);
      Path_OCCL* poc = new Path_OCCL(s,get_occurrence_list(),item_list);
      result.push_back(poc);
    }else{
      Path_OCCL* poc = new Path_OCCL(label_path,get_occurrence_list(),item_list);
      poc->rp_path.push_back(get_node()->label);
      s = poc->rp_path;
      result.push_back(poc);
    }
    
    for(int i = 0 , n = get_children().size();i<n;i++){
      vector<Path_OCCL*> res = ((RPTree*) get_children()[i])->get_POCCL_list(s);
      result.insert(result.end(),res.begin(),res.end());
    }
    
    return result;
 

}

//debug
void RPTree::print_tree(){
  cout << node->id << ":" << node->label << " " << get_occurrence_list().size() << endl;
  print_occurrence_list();
  for(int i = 0,n=children.size();i<n;i++){
    children[i]->print_tree();
  }
  cout << -1 << " " << endl;
}


/**********RGTree **********/
RGTree::RGTree(RPTree* rp_tree):EnumerationTree(0,rp_tree->get_node()->label,rp_tree->get_occurrence_list()){
  for(int i=0,n=rp_tree->get_occurrence_list().size();i<n;i++){
    item_list.push_back(i);
  }
}

RGTree::RGTree(int label, vector<Tree*> occ_list,vector<int> item_list):EnumerationTree(0,label,occ_list){
  this->item_list = item_list;
}

RGTree::RGTree(int label,vector<int> item_list):EnumerationTree(0,label){
  this->item_list = item_list;
}

//debug
void RGTree::print_item_list(){
  cout <<"size:" << item_list.size()<< " {" ;
  for(int i = 0,n = item_list.size();i <n;i++){
    cout << item_list[i]<< " ";
  }
  cout << "}"<<endl;
}
//debug
void RGTree::print_tree(){
  cout << node->id << ":" << node->label << " " << get_occurrence_list().size() << endl;
  print_occurrence_list();
  print_item_list();
  for(int i = 0,n=children.size();i<n;i++){
    children[i]->print_tree();
  }
  cout << -1 << " " << endl;
}

/*
  @brief 子孫のitem_listをvectorにまとめて返す
*/
vector<vector<int>> RGTree::get_item_transaction(){
  vector<vector<int>> transaction;
  transaction.push_back(item_list);
  for(int i = 0,n = get_children().size() ; i < n;i++){
    vector<vector<int>> res = ((RGTree*)get_children()[i])->get_item_transaction();
    transaction.insert(transaction.end(),res.begin(),res.end());
  }
  return transaction;
}

vector<vector<int>> RGTree::get_item_transaction_memred(){
  vector<vector<int>> transaction;
  transaction.push_back(get_item_list_memred());
  vector<Tree*> cs = get_children();
  for(int i = 0 , n = cs.size();i<n;i++){
    vector<vector<int>> res = ((RGTree*) cs[i])->get_item_transaction_memred();
    transaction.insert(transaction.end(),res.begin(),res.end());
  }
  return transaction;
}

/*
  @brief idリストにあるノードのoccurrenceのandをとって返す
  @param id_list
  @param oc_list
  @detail
  今見ているノードのoccurrenceとはここではitem_listを指す
*/
vector<int> RGTree::filter_rgtree_occurrence(vector<int> id_list,vector<int> oc_list){
  if(find(id_list.begin(),id_list.end(),node->id) == id_list.end()){
    return oc_list;
  }
  vector<int> result;
  set_intersection(item_list.begin(),item_list.end(),oc_list.begin(),oc_list.end(),back_inserter(result));
  for(int i=0, n = get_children().size();i<n;i++){
    vector<int> c = ((RGTree*) get_children()[i])->filter_rgtree_occurrence(id_list,result);
    vector<int> res;
    set_intersection(result.begin(),result.end(),c.begin(),c.end(),back_inserter(res));
    result = res;
  }
  return result;
}

/*
  @brief idリストにあるノードのoccurrenceのandをとって返す
  @param id_list : 解に含まれるRGTree中のノードidのリスト
  @param oc_list
  @detail
  今見ているノードのoccurrenceとはここではitem_listを指す
 */
vector<int> RGTree::filter_rgtree_occurrence_improved(vector<int> id_list){
  vector<RGTree*> all_leaves = get_all_leaves(id_list);
  //  cout << "all_leaves size is " << all_leaves.size() << endl;
  vector<int> result=all_leaves[0]->item_list;
  for(int i =1,n=all_leaves.size();i<n;i++){
    vector<int> occ = all_leaves[i]->item_list;
    vector<int> res;
    set_intersection(result.begin(),result.end(),occ.begin(),occ.end(),back_inserter(res));
    result = res;
  }
  return result;
}
/*
@brief メモリ削減したように書き直した
@sa filter_rgtree_occurrence_improved
 */
 vector<int> RGTree::filter_rgtree_occurrence_memory_improved(vector<int> id_list){
  vector<RGTree*> all_leaves = get_all_leaves(id_list);
  //  cout << "all_leaves size is " << all_leaves.size() << endl;
  vector<int> result=all_leaves[0]->get_item_list_memred();
  for(int i =1,n=all_leaves.size();i<n;i++){
    vector<int> occ = all_leaves[i]->get_item_list_memred();
    vector<int> res;
    set_intersection(result.begin(),result.end(),occ.begin(),occ.end(),back_inserter(res));
    result = res;
  }
  return result;

}

/*
  @brief 今見ているid_listの葉ノードをすべて取ってくる
 @param id_list
 */
vector<RGTree*> RGTree::get_all_leaves(vector<int> id_list){
  vector<RGTree*> res;
  if(!is_child_id_included(children,id_list)){
    res.push_back(this);
    return res;
  }
  vector<RGTree*> cur_children;
  for(Tree* c : children){cur_children.push_back((RGTree*) c);}
 
  for(RGTree* chd : cur_children){
    if(find(id_list.begin(),id_list.end(),chd->get_node()->id) == id_list.end()){
      continue;
    }
    vector<RGTree*> c = chd->get_all_leaves(id_list);
    res.insert(res.end(),c.begin(),c.end());
  }
  return res;
} 

/*
  @brief RGTreeのアイテムリスト(occ_listのindex)を取る
  @detail メモリ節約のため，出現リストの親との差分だけ保持，取る際は根からフィルタをかけながら取る
 */
vector<int> RGTree::get_item_list_memred(){
  //if(this->get_parent() == NULL){return item_list;}
  RGTree* t = this;
  vector<int> root_items,to_remove;
  while(t != NULL){
    if(t->get_parent() == NULL){
      for(int i = 0, n = t->item_list.size();i < n;i++){
	//	cout << "t->itemlist size in for is " << t->item_list.size() << endl;
	root_items.push_back(t->item_list[i]);
      }
      break;
    }else{
      for(int i = 0 , n = t->item_list.size();i<n;i++){
	to_remove.push_back(t->item_list[i]);
      }
    }
    t = (RGTree*) t->get_parent();
  }
  vector<int> r;
  set_difference(root_items.begin(),root_items.end(),to_remove.begin(),to_remove.end(),back_inserter(r));
  return r;
}
/*
  @brief RGTreeのoccurrence_listを取る
  @detail メモリ節約のため，出現リストの親との差分だけ保持，取る際は根からフィルタをかけながら取る
*/
vector<Tree*> RGTree::get_occurrence_list_memred(){
  RGTree* root = (RGTree*) this->get_root();
  vector<int> items = get_item_list_memred();
  vector<Tree*> occs;
  for(int i = 0 ,n = items.size();i<n;i++){
    occs.push_back(root->get_occurrence_list()[items[i]]);
  }
  return occs;
}


/***************************************
以下静的お便利メソッドの実装
****************************************/
/*
@brief 木のリストに，idlistに含まれるidを持つものが存在するかどうか
@param children: tree list
@param id_list
@sa get_all_leaves
@return 含まれていればtrueを返す
 */
bool is_child_id_included(vector<Tree*> children,vector<int> id_list){
  for(Tree* c :children){
    if(find(id_list.begin(),id_list.end(),c->get_node()->id) !=id_list.end()){
      return true;
    }
  }
  return false;
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    if (!item.empty()) {
      elems.push_back(item);
    }
  }
  return elems;
}


Tree* make_tree(string canonical_form ){
  vector<string> labels = split(canonical_form,' ');
  Tree* t = new Tree(0,stoi(labels[0]));
  int current_id = 0;
  Tree* current_tree = t;
  int n = labels.size();
  for(int i = 1;i<n;i++){
    if(stoi(labels[i]) == -1){
      current_tree = current_tree->get_parent();
    }else{
      current_id++;
      Tree* new_tree = new Tree(current_id,stoi(labels[i]));
      current_tree->add_child(new_tree);
      current_tree= new_tree;
    }

  }
  return t;
}

/*
Treeを引数にとって同じ構造のEnumerationTreeを生成
引数にはConstrained-subtreeが渡されることを想定
 */
EnumerationTree* gen_enumeration_tree(Tree *tree,EnumerationTree *parent,vector<Tree*> occ_list){
  EnumerationTree* et = new EnumerationTree(tree->get_node()->id,tree->get_node()->label);
  et->add_occurrence_list(occ_list);
  if(tree->get_parent() != NULL){
    et->set_parent(parent);
  }
  
  for(int i=0,n=tree->get_children().size();i<n;i++){
    vector<Tree*> new_chd_occ_list;
    for(int j=0,m=occ_list.size();j<m; j++ ){
      new_chd_occ_list.push_back(occ_list[j]->get_child(tree->get_children()[i]->get_node()->label));
    }
    et->add_child(gen_enumeration_tree(tree->get_children()[i],et,new_chd_occ_list));
    
  }
  return et;
}

/*
currentに渡されたtree_vecがroot_occurrence_lstのどの要素に対応するのかそのindexの
リストを返す
*/
vector<int> get_correspond_index_list(vector<Tree*> current,vector<Tree*> root_oc_list){
  vector<int> result;
  int current_index = 0;
  for(int i = 0,n = root_oc_list.size() ; i < n ; i++){
    if(current[current_index]->get_node()->id == root_oc_list[i]->get_node()->id &&
       current[current_index]->get_tree_id() == root_oc_list[i] -> get_tree_id()){
      result.push_back(i);
      current_index++;
    }
    if(current_index == current.size()){
      break;
    }
  }
  return result;
}

//@debug
void print_tree_vector(vector<Tree*> tree_vec){
  for(int i = 0,n = tree_vec.size();i<n;i++){
    if(tree_vec[i] == NULL){
      cout << "null" << endl;
      continue;
    }
    tree_vec[i] -> print_node();
    cout << endl;
    cout << "--------------" << endl; 
  }
}

//@debug
void print_vv(vector<vector<int>>vv){
  for(int i = 0 , n = vv.size();i<n;i++){
    print_vec(vv[i]);
  }
}

//debug
void print_vec(vector<int> vec){
  cout << "{";
  for(int i = 0 , n = vec.size(); i < n ; i++){
    cout << vec[i] << ",";
  }
  cout << "}" << endl;
}
