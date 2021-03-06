#ifndef TREE
#define TREE

#include "misc.h"
struct Node{
  int label;
  int id;
};

/*struct RGNode{
  int label;
  int id;
  struct RGNode *parent;
  vector<RGNode*> children;
  vector<Node*> occurence_list; 
  };*/

struct Pair{
  int left;
  int right;
};


/*
Treeクラス
最も基本的な木のクラス，DB中の木は全てこの型
また，アルゴリズム中に登場する木もすべてこの型を継承するものとする．
*/
class Tree{
public:
  Tree(int id,int label);
  Tree(Tree* root);
  Tree(string canical_form);
  Tree(vector<string> tag_vec,string exception_root);
  Tree(int current_index,vector<string> sv,Tree* parent);
  //  Tree* make_tree(string canonical_form);
  Node* new_node(int lab);
  
  virtual ~Tree();
  Node* new_node();
  Node* get_node();
  Tree* get_parent();
  vector<Tree*> get_children();
  void set_children(vector<Tree*> new_children);
  Tree* get_tree(int node_id);

  void add_child(Tree* new_child);
  void add_node(Tree *parent,Node *node);
  void set_parent(Tree *tree);
  virtual void print_tree();
  int reindexing(int current);
  bool is_subtree(Tree* tree);
  Tree* get_child(int label);
  void update_tree_id(int id);
  void to_attribute_tree_by_cutting();
  void to_attribute_tree_by_merging();
  int get_tree_id();
  vector<Tree*> enumerate_subtree(Tree* subtree);
  Tree* get_root();
  int get_num_of_nodes();

  string get_tree_string();
  void print_node();
  
protected:
  Node *node;
  Tree *parent;
  int tree_id;
  vector<Tree *> children;

private:
  string get_string();
  bool is_exist_mbipart();
};

/***
Treeクラスのvectorを持つクラス
パターンに対する頻度を返すメソッドやパターンマッチングを行うメソッドを持つ
***/
class TreeDB{
public:
	TreeDB();
	void add_tree(Tree* tree);
	Tree *get_tree(int tree_id,int node_id);
	bool is_frequent(Tree* tree,int minimum_sup);
	vector<Tree*> get_subtree_list(Tree* tree);
	void to_attribute_tree_by_cutting();
	void to_attribute_tree_by_merging();
	
	int get_num_of_nodes();
	int get_num_of_trees();
	Tree* get_tree(int i);

	void set_tree_id();
	void print_tree_db();


private:	
	vector<Tree*> treedb;
};



/*
EnumerationTreeクラス
tree-expansion-strategyアルゴリズムに用いられる木を定義したクラス．
より複雑なstrategyを用いる場合は適宜このクラスを継承する．
*/
class EnumerationTree : public Tree{
public:
  EnumerationTree(int id,int label);
  EnumerationTree(int id,int label,vector<Tree*> occ_list);
  EnumerationTree(EnumerationTree* et);
  virtual ~EnumerationTree(){};
  
  void add_occurrence_list(vector<Tree*> tree_vec);
  EnumerationTree* expand_occurrence_matched();
  EnumerationTree* exp_om_parent();
  void exp_om_children();
  vector<Tree*> get_parent_occurrences();
  vector<Tree*> get_children_occurrences();
  vector<Tree*> get_occurrence_list();
  vector<Tree*> filter_occurrence_list(vector<int> occ_id_list);
  int get_transaction_support();
  vector<int> get_occurrence_parent_ids(int label);
  vector<int> get_occurrence_ids(int label);
  void update_occurrence_list(vector<int> list);
  vector<Tree*> get_root_occurrence_list(vector<Tree*> occurrence_list);

  //labelを指定してそのラベルを持つ子を削除
  void remove_child(int label);


  void print_tree()override;
  void print_occurrence_list();

private:
  vector<Tree*> occurrence_list;
  vector<Tree*> make_new_child_occ(int label);

};

struct Item{
  int item_id;
  int label;
  bool is_parent;
  EnumerationTree* et;
  vector<Tree*> next_occurrences;
  vector<int> next_occurrence_indices;
};





/*
@struct Path_OCCL
@brief ラベル列とoccurrence_listのペア
@sa get_POCC_list
 */
struct Path_OCCL{
  vector<int> rp_path;//label's path
  vector<Tree*> occ_list; //occ_lst 
  vector<int> item_list;
  

  Path_OCCL(vector<int> p,vector<Tree*> ocl){
    for(int i = 0 , n = p.size();i<n;i++){
      rp_path.push_back(p[i]);
    }
    for(int i = 0,n = ocl.size();i<n;i++){
      occ_list.push_back(ocl[i]);
      item_list.push_back(i);
    }
  }

  Path_OCCL(vector<int> p,vector<Tree*> ocl,vector<int> vlist){
    for(int i = 0 , n = p.size();i<n;i++){
      rp_path.push_back(p[i]);
    }
    for(int i = 0,n = ocl.size();i<n;i++){
      occ_list.push_back(ocl[i]);
    }
    for(int i=0,n=vlist.size();i<n;i++){
      item_list.push_back(vlist[i]);
    }
  }
  
  
};


/*
SCC_Minerによって呼ばれる
解の根の列挙するのに用いられる
 */
class RPTree : public EnumerationTree{
public:
  vector<int> item_list;
  RPTree(int label,vector<Tree*> occ_list);
  RPTree(int label, vector<Tree*> occ_list,vector<int> item_list);
  ~RPTree();
  void rm_dec();
  vector<Path_OCCL*> get_POCCL_list(vector<int> label_path);

  //debug
  void print_tree()override;
};


/*
SCC_Minerによって呼ばれる
これをアイテムセット列挙問題に還元しLCMをかける
 */
class RGTree : public EnumerationTree{
public:
  RGTree(RPTree* rp_tree);
  ~RGTree();
  RGTree(int label,vector<Tree*> occ_list,vector<int> item_list);
  RGTree(int label, vector<int> item_list);
  vector<int> item_list;
  //debug
  void print_item_list();
  //debug
  void print_tree()override;
  vector<vector<int>> get_item_transaction();
  vector<vector<int>> get_item_transaction_memred();
  vector<int> filter_rgtree_occurrence(vector<int> id_list,vector<int> oc_list);
  vector<int> filter_rgtree_occurrence_improved(vector<int> id_list);
  vector<int> filter_rgtree_occurrence_memory_improved(vector<int> id_list);
  vector<RGTree*> get_all_leaves(vector<int> id_list);
  vector<int> get_item_list_memred();
  vector<Tree*> get_occurrence_list_memred();
};



bool is_child_id_included(vector<Tree*> children,vector<int> id_list);
vector<string> split(const string &s, char delim);
Tree* make_tree(string canonical_form);
EnumerationTree* gen_enumeration_tree(Tree *tree,EnumerationTree *parent,vector<Tree*> occ_list);
vector<int> get_correspond_index_list(vector<Tree*> current_list,vector<Tree*> root_occurrence_list);
//debug
void print_tree_vector(vector<Tree*> tree_vec);
void print_vv(vector<vector<int>> vv);
void print_vec(vector<int> vec);
#endif //TREE
