#ifndef SCCMiner
#define SCCMiner
 
#include <iostream>
#include <vector>
#include <map>
#include "tree.h"
#include "file_io.h"
#include "measure.h"
//#include "lcm_cpp/lcm.h"


extern "C" {
  //#include "lcm25.lcm.h"
  #include "lcm53/lcm.h"
} 



/*
struct RPTree{
  vector<Tree*> roots;
  int label;
  RPTree(vector<Tree*> oc,int lb):roots(oc),label(lb){}
  };*/

//debug
void print_Path_OCCL(Path_OCCL* poccl){
  for(int i = 0 ,n = poccl->rp_path.size();i<n;i++){
    cout << poccl->rp_path[i] << ",";
  }
  cout << " : {" ;
  for(int i =0,n = poccl->occ_list.size();i<n;i++){
    cout << poccl->occ_list[i]->get_tree_id();
    cout << ":";
    cout << poccl->occ_list[i]->get_node()->id;
    cout <<",";
  }
  cout << "}:{ ";
  
  for(int i=0,n=poccl->item_list.size();i<n;i++){
    cout << poccl->item_list[i];
    cout << ",";
  }
  cout << "}"<<endl;
}

//debug
void print_POCCL_list(vector<Path_OCCL*> poccl_list){
  for(int i = 0,n = poccl_list.size();i<n;i++){
    print_Path_OCCL(poccl_list[i]);
  }
}




/*
@class RPRGPathItem
@brief RP-pathとRG-pathとid,item_listの4つ組
@sa RG_path_exp
@detail これをRG_path_expで拡張することによって、頻出(飽和)なパスパターンを全列挙し，
Attribute-tree-mining-with-subtree-constrainedをitem-set miningに還元する
 */
class RPRGPathItem{

public:
  int item_id;
  vector<int> rg_path;
  vector<int> rp_path;
  vector<int> item_list;

  RPRGPathItem(RPRGPathItem* rprg){
    for(int i = 0 , n = rprg->rp_path.size();i<n;i++){
      rp_path.push_back(rprg->rp_path[i]);
    }
    for(int i = 0 , n = rprg->rg_path.size();i < n;i++){
      rg_path.push_back(rprg->rg_path[i]);
    }
  }
  RPRGPathItem(Path_OCCL* poc){
    for(int i = 0,n = poc->item_list.size() ; i<n;i++){
      item_list.push_back(poc->item_list[i]);
    }
    for(int i = 0, n= poc->rp_path.size();i < n ;i++){
      rp_path.push_back(poc->rp_path[i]);
    }
    rg_path.push_back(poc->rp_path[rp_path.size()-1]);
  }

  //item_id に値をsetする
  void set_item_id(int item_id){
    this->item_id = item_id; 
  }

};

/*
@brief RPRGPathItemのリストをitemlistのリストに変換
@param rprg_list:変換するRPRGPathItemリスト
@return 変換した結果のリスト
 */
vector<vector<int>> convert_rprg_list(vector<RPRGPathItem*> p_list){
  vector<vector<int>> result;
  for(int i = 0,n = p_list.size(); i< n;i++){
    result.push_back(p_list[i]->item_list);
  }
  return result;
}

/*
@brief 2つのリストから共通するものがあればtrue なければfalse
@param list1
@param list2
 */
bool is_there_sharing_occ(vector<int> list1,vector<int> list2){
  for(int i = 0 ,n = list1.size(); i < n ; i ++){
    for(int j = 0 , m = list2.size();j<m;j++){
      if(list1[i] == list2[j]){return true;}
    }
  }
  return false;
}

/*
@brief rp_path,rg_pathに合わせて木を拡張していく 
@param rprg
@param tree constrained-treeの根の位置を起点にしているため、そのポインタが渡される 
@detail
 */
void exp_enum_tree_on_rprg(RPRGPathItem* rprg,EnumerationTree* tree){
  EnumerationTree* current = tree;
  for(int i = 1 , n = rprg->rp_path.size();i<n;i++){
    if(current->get_parent() == NULL){
      EnumerationTree* next_parent = new EnumerationTree(0,rprg->rp_path[i]);
      current->set_parent(next_parent);
      next_parent->add_child(current);
      current = next_parent;
    }else{
      current = (EnumerationTree*) current->get_parent();
    }
  }
  for(int i = 1 , n = rprg->rg_path.size();i<n;i++){
    EnumerationTree* next_child = (EnumerationTree*) current->get_child(rprg->rg_path[i]);
    if(next_child == NULL){
      next_child = new EnumerationTree(0,rprg->rg_path[i]);
      next_child->set_parent(current);
      current->add_child(next_child);
      current = next_child;
    }else{
      current = next_child;
    }
  }
}

/*
@brief RPRGPathItemのリストとアイテムリストからrp,rg_pathを見て木を生成
@param i_list LCMかけた結果
@param p_list 
@detail
*/
EnumerationTree* merge_item_result(vector<int> i_list, vector<RPRGPathItem*> p_list){
  vector<int> ref_list;
  ref_list.push_back(i_list[0]);
  for(int i = 1,n = i_list.size() ; i<n;i++){
    ref_list.push_back(i_list[i]);
  }
  EnumerationTree* result = new EnumerationTree(0,p_list[ref_list[0]]->rp_path[0]);
  for(int i = 0 , n = ref_list.size();i<n;i++){
    exp_enum_tree_on_rprg(p_list[ref_list[i]],result);

  }
  result = (EnumerationTree*) result->get_root();
  return result;
}



//debug
void print_rprg_path_item(RPRGPathItem* rprg_path_item){
  for(int i =0,n = rprg_path_item->rp_path.size();i<n;i++){
    cout << rprg_path_item->rp_path[i] << ",";
  }
  cout << " : ";
  for(int i = 0,n= rprg_path_item->rg_path.size();i<n;i++){
    cout << rprg_path_item->rg_path[i] << ",";
  }
  cout << " :: ";
  for(int i=0,n = rprg_path_item->item_list.size();i<n;i++){
    cout << rprg_path_item->item_list[i] <<",";
  }
  cout <<endl;
}

//debug
void print_RPRG_path_item_list(vector<RPRGPathItem*> list){
  for(int i = 0,n = list.size() ; i< n;i++){
    print_rprg_path_item(list[i]);
  }
}

struct ListPair{
  vector<Tree*> occurrence_list;
  vector<int> item_list;
  ListPair(Tree* oc,int item){
    occurrence_list.push_back(oc);
    item_list.push_back(item);
  };
};


//debug
void print_label_map(map<int,vector<Tree*>> label_map){
    for(auto itr = label_map.begin();itr != label_map.end();++itr){
      
      vector<Tree*> tree_list = itr->second;
      cout << itr->first << " : " ;
      for(int i = 0,n = tree_list.size() ; i < n;i++){
      cout << "(" << tree_list[i]->get_tree_id()  << ","  << tree_list[i]->get_node()->id << ")";
    }
    cout << endl;

    }

}
/*
@brief 解の根の候補の列挙のためにrp_treeを根から拡張
@param rp-treeの根
@param min_sup ミニマムサポート
@return 
@detail
1.occurrence_listの各occurrenceから上を見る
2.ラベルからoc_listへの連想配列を作って実現する
3.頻出である限り再帰的にまとめて木を作る
*/
void exp_rp_tree(RPTree* rp_tree,int min_sup){
  //ラベルマップを作成，ラベルを渡すとoc_listを返す連想配列
  vector<Tree*> oc_list = rp_tree->get_occurrence_list();
  //  map<int,vector<Tree*>> label_map;
  map<int,ListPair*> label_map;
  for(int i = 0,n = oc_list.size();i<n;i++){
    if(oc_list[i]->get_parent() != NULL){
      int l = oc_list[i]->get_parent()->get_node()->label;
      auto itr = label_map.find(l);
      if(itr == label_map.end()){
	ListPair* lsp = new ListPair(oc_list[i]->get_parent(),rp_tree->item_list[i]);
	label_map[l] = lsp;
	//	if(l == 0){cout << "id i in label 0: " << i <<endl;}	
/*	vector<Tree*> tv;
	tv.push_back(oc_list[i]->get_parent());
	label_map[l] = tv;*/
      }else{
	//if(l == 0){cout << "id i in label 0: " << i <<" "<< rp_tree->item_list[i]<<endl;}
	label_map[l]->occurrence_list.push_back(oc_list[i]->get_parent());
	label_map[l]->item_list.push_back(rp_tree->item_list[i]);
	//	label_map[l].push_back(oc_list[i]->get_parent());
      }
    }
  }
  //再帰的に処理
  for(auto itr = label_map.begin();itr!=label_map.end();++itr){
    if((itr->second->occurrence_list).size() >= min_sup){
      RPTree* rp_child = new RPTree(itr->first,itr->second->occurrence_list,itr->second->item_list);
      rp_child->set_parent(rp_tree);
      rp_tree->add_child(rp_child);
      exp_rp_tree(rp_child,min_sup);
    }
  }
}
/*
@brief 
@param rg_tree : root_candidate中の1ノード
@param min_sup : ミニマムサポート
@return
@sa exp_rp_tree
@detail
rg-tree->occurrence_list の子ノードのラベルを再帰的にまとめ上げていく
*/
void exp_rg_tree(RGTree* rg_tree,int min_sup){
  //ラベルマップを作成，ラベルを渡すとoc_listを返す連想配列
  vector<Tree*> oc_list = rg_tree->get_occurrence_list();
  map<int,ListPair*> label_map;
  for(int i = 0,n = oc_list.size();i<n;i++){
    for(int j = 0 , m = oc_list[i]->get_children().size(); j < m;j++){
      int l = oc_list[i]->get_children()[j]->get_node()->label;
      auto itr = label_map.find(l);
      if(itr == label_map.end()){
	ListPair* lsp = new ListPair(oc_list[i]->get_children()[j],rg_tree->item_list[i]);
	label_map[l] = lsp;
      }else{
	label_map[l]->occurrence_list.push_back(oc_list[i]->get_children()[j]);
	label_map[l]->item_list.push_back(rg_tree->item_list[i]);
      }
      
    }
  }

  //再帰的に処理
  for(auto itr = label_map.begin(); itr!=label_map.end(); ++itr){
    if((itr->second->occurrence_list).size() >= min_sup){
      RGTree* rg_child = new RGTree(itr->first,itr->second->occurrence_list,itr->second->item_list);
      rg_child->set_parent(rg_tree);
      rg_tree->add_child(rg_child);
      exp_rg_tree(rg_child,min_sup);
    }
  }

}


/*
@brief 各rg-treeの根から下に見てパスを再帰的に列挙していく
@param rprg
@oc_list occurrence_list
@param min_sup ミニマムサポート
@param closed_flag closed or maximal??
@return rprg_path_itemのリスト
@sa exp_rg_tree
@detail
 */
vector<RPRGPathItem*> exp_rg_path_rec(RPRGPathItem* rprg,vector<Tree*> oc_list,int min_sup,bool closed_flag){

  //以下のラベルマップ取る操作はモジュール化すべき
  map<int,ListPair*> label_map;
  for(int i = 0,n = oc_list.size();i<n;i++){
    for(int j = 0 , m = oc_list[i]->get_children().size(); j < m;j++){
      int l = oc_list[i]->get_children()[j]->get_node()->label;
      auto itr = label_map.find(l);
      if(itr == label_map.end()){
	ListPair* lsp = new ListPair(oc_list[i]->get_children()[j],rprg->item_list[i]);
	label_map[l] = lsp;
      }else{
	label_map[l]->occurrence_list.push_back(oc_list[i]->get_children()[j]);
	label_map[l]->item_list.push_back(rprg->item_list[i]);
      }
      
    }
  }

  vector<RPRGPathItem*> result;
  //子供に同じサポートのものがあればtrueを入れる
  bool same_support_flag = false;
  for(auto itr = label_map.begin();itr != label_map.end();++itr){
    //rp-pathを逆に辿らない
    if(rprg->rp_path.size()>1 && rprg->rg_path.size() == 1){
      if(rprg->rp_path[rprg->rp_path.size()-2] == itr->first){
	continue;
      }
    }
    if(itr->second->item_list.size() == rprg->item_list.size()){same_support_flag = true;}
    RPRGPathItem* next_rprg = new RPRGPathItem(rprg);
    next_rprg->rg_path.push_back(itr->first);
    next_rprg->item_list = itr->second->item_list;
    if(next_rprg->item_list.size() >= min_sup){
      vector<RPRGPathItem*> res = exp_rg_path_rec(next_rprg,itr->second->occurrence_list,min_sup,closed_flag);
      result.insert(result.end(),res.begin(),res.end());
    }
  }
  
  if(!closed_flag || !same_support_flag){
    result.push_back(rprg);
  }
  
  return result;
}


/*
@brief POCCLのvectorを受け取って，頻出なパスを全てリストにして返す
@param poc_list rp-tree各ノードに対応したrp-pathとitem_listのペアのリスト
@param closed_flag closedなら同じoccurrenceを持つパスを列挙しなくてよい
@return rprg_path_itemのリスト
@detail
 */
vector<RPRGPathItem*> enum_rprg_item_list(vector<Path_OCCL*> poc_list,int min_sup,bool closed_flag){
  vector<RPRGPathItem*> result;
  
  for(int i = 0,n = poc_list.size();i<n;i++){
    RPRGPathItem *rprg = new RPRGPathItem(poc_list[i]);
    vector<RPRGPathItem*> res = exp_rg_path_rec(rprg,poc_list[i]->occ_list,min_sup,closed_flag);
    //    cout << "res size is " << res.size()<<endl;
    result.insert(result.end(),res.begin(),res.end());
  }

  return result;
}


 /*
@brief 2つの木(ノード)が渡されるとそれが同じ位置にあるかどうか判定
@param t1 木1
@param t2 木2
@return 木1と木2の位置が等しい(同じ場所を参照している)ならばtrueそうでなければfalse
@sa is_occurrence_included()
@detail
*/
bool is_same_occurrence(Tree* t1,Tree* t2){
  return (t1->get_tree_id() == t2->get_tree_id() &&
	  t1->get_node()->id == t2->get_node()->id);
}

/*
@brief 2つの木が渡されて，t1の位置がt2よりも進んでしまっているときtrue
@sa is_occurrence_included()
 */
bool is_occurrence_exceeded(Tree* t1,Tree* t2){
  return (t1->get_tree_id() > t2->get_tree_id()) ||
    ((t1->get_tree_id() == t2->get_tree_id()) & (t1->get_node()->id > t2->get_node()->id));
}


/*
@brief 2つのoccurrence_listが包含関係になっているかどうかを判定
@param occs1 occurrence_list1
@param occs2 occurrence_list2 
@return ocls1がocls2に包含されていればtrueそうでなければfalse
@sa 
@detail occurrence_list内部のノードは既に深さ優先順にすでに並んでいるという前提
*/
bool is_included(vector<Tree*> occs1,vector<Tree*> occs2){
 int current_occurrence_list1_pointer = 0;
 int oc1_size = occs1.size();
 for(int i =0,n = occs2.size();i<n;i++){

   if(is_occurrence_exceeded(occs2[i],occs1[current_occurrence_list1_pointer])){
     return false;
   }
   if(is_same_occurrence(occs1[current_occurrence_list1_pointer],occs2[i] )){
     current_occurrence_list1_pointer++;
   }
   if(current_occurrence_list1_pointer == oc1_size){
     return true;
   }
 }
 return false;

}

/*
@brief rp-treeの各子供と同じサイズのoccurrence_listがあるかどうかをチェック
@param rptree
@return あればtrue無ければfalse 
*/
bool is_children_same_occ(RPTree* rptree){
  if(rptree->get_children().size() != 1){return false;}
  if(rptree->get_occurrence_list().size() == ((RPTree*)rptree->get_children()[0])->get_occurrence_list().size()){
    return true;
  }
  return false;
}

/*
@brief rptreeのノードの中に第1引数のoccurrence_listと包含関係になっているものは無いか
@param rptree
@param rptree_checks checkする方のrptree,こちら側を再帰的にチェック
@return あればtrue なければfalse
@detail
再帰的にrptreeを根から見ていってis_includedでチェックする.
ただし、チェックしてるRP-Treeのノードが同じノードを見ている場合はfalseを返す
 */
bool is_exist_occurrence_included(RPTree* rptree,RPTree* rptree_checks){

   
  if(rptree->get_node()->id == rptree_checks->get_node()->id){
    return false;
  }
  if(rptree->get_occurrence_list().size() > rptree_checks->get_occurrence_list().size()){
    return false;
  }
  //  cout << "before is_included" << endl;
  bool r = is_included(rptree->get_occurrence_list(),rptree_checks->get_occurrence_list());
  //cout << "after is_included" << endl;
  if(r){return r;}
  for(int i = 0,n = rptree_checks->get_children().size(); i < n;i++){
    r = r || is_exist_occurrence_included(rptree,(RPTree*)rptree_checks->get_children()[i]);
    if(r){return r;}
  }
  return false;
}




/*
@brief rp-treeの各ノードに解の根の候補としてフィルターをかける
@param rptree
@param rproot rptreeの根ノード，occlistが包含関係となっているrp-nodeが存在するかチェックする際に用いられる
@param is_improved 新しく修正した方式を適用するかどうかのフラグ
@return rp-tree内のノードのベクター，各要素に対してトップダウン手法を用いることで
closed-patternを列挙
@detail
1. rp-treeの全てのノードを上から再帰的に見る
2. occ(rp-node).size == occ(rp-node.child).sizeならばそのノードのrp-nodeは根の候補に入れない
3. occ(rp-node)のスーパセットとなる他のocc(rp-node_other)があるならrp-treeからそのrp-node以下を削除
4. 2,3に該当しないならば解に追加
*/
vector<RPTree*> get_root_candidates(RPTree* rptree,RPTree* rproot,bool is_improved){
  vector<RPTree*> root_candidates;

  //occ(rp-node)を包含するrp-node2があればrp-node以下を削除かつ見ない
  if(is_improved && is_exist_occurrence_included(rptree,rproot)){
    //  cout << "before rm_dec in rp cutter" << endl;
    rptree->rm_dec();
    //cout << "after rm_dec in rp cutter" << endl;
    //    rproot->print_tree(); cout << endl;
    return root_candidates;
  }

  if(!is_children_same_occ(rptree)){
    root_candidates.push_back(rptree);
  }

  //  cout << "before for in rp cutter" << endl;
  for(int i = 0, n = rptree->get_children().size();i<n;i++){
    //    cout << i << " " << n <<" "<<rptree->get_children().size()<<endl; 
    vector<RPTree*> res = get_root_candidates((RPTree*)rptree->get_children()[i],rproot,is_improved);
    root_candidates.insert(root_candidates.end(),res.begin(),res.end());
    n = rptree->get_children().size();
  }
  //  cout << "after for in rp cutter" << endl;
  return root_candidates;
}

/*
@brief RP-Tree内を探索して，そのRP-Nodeが持っているOccurrence_listのスーパーセットをもつものが存在すれば
そのrp-node
@param rptree 今見てるrp-treeのノード
@param rptreeの根、探索の起点
@sa get_root_candidate
@detail
 */
void rp_tree_cutter(RPTree* rptree,RPTree* rproot){
  if(is_exist_occurrence_included(rptree,rproot)){
    //cout << "before rmdec" << endl;
    rptree->rm_dec();
    //cout << "after rmdec " << endl;
    return;
  }
  for(int i = 0, n = rptree->get_children().size();i<n;i++){
    int k = rptree->get_children().size();
    if(k < n){
      i--;
      n = rptree->get_children().size();
    }
    rp_tree_cutter((RPTree*)rptree->get_children()[i],rproot);
    //    n = rptree->get_children().size();
  }
}


/*
@brief closed-patternが与えられてそれが上にoccurrence_matchedなノードが無いかどうか判定 
@param current_root　現在見ている解の根の候補
@param closed_pattern current_rootからtop-down-methodで導かれたclosed-pattern
@return closed-patternsから上を見てoccurrence-matchedなものがあるか
@detail
current-rootからoccurrence_list
closed_patternからocc-listを見る
各親に同じラベルを持つノードがくっついているならtrueそうでなければfalse
 */
 bool is_there_occurrence_matched(RPTree* current_root,CP* closed_pattern){
   vector<Tree*> current;
   /*
   for(int i= 0,n= closed_pattern->occ_list.size();i<n;i++ ){
     cout << "add closed pattern in is_there_oc_matched " << i << " / " << n << endl;
     current.push_back(current_root->get_occurrence_list()[closed_pattern->occ_list[i]]);
     }*/
   vector<Tree*> r_occ = current_root->get_occurrence_list();
   Tree* zero_parent = r_occ[closed_pattern->occ_list[0]]->get_parent();
   if(zero_parent  == NULL){return false;}
   int size = closed_pattern->occ_list.size();
   for(int i = 1;i<size;i++){
     //cout << " in is there occ_matched" << i << " / " << size << endl;
     Tree* c_parent = r_occ[closed_pattern->occ_list[i]]->get_parent();
     if(c_parent== NULL){return false;}
     if(zero_parent->get_node()->label != c_parent->get_node()->label){
       return false;
     }
   }
   return true;
 }
 /*
@brief　再帰的に呼び出してclosed-patternを生成
@sa get_closed_tree
  */
 void make_closed_tree_rec(RGTree* rg_tree,vector<int> node_id_list,EnumerationTree* parent){
   //cout << "node id size is" << node_id_list.size()<<endl;
   for(int i=0,n = rg_tree->get_children().size();i < n;i++){
     if(find(node_id_list.begin(),node_id_list.end(),rg_tree->get_children()[i]->get_node()->id) != node_id_list.end()){
       EnumerationTree* ne = new EnumerationTree(rg_tree->get_children()[i]->get_node()->id,
						 rg_tree->get_children()[i]->get_node()->label);
       ne->set_parent(parent);
       parent->add_child(ne);
       make_closed_tree_rec((RGTree*)rg_tree->get_children()[i],node_id_list,ne);
     }
   }
 }

/*
@brief idリストのリストを渡して，それに対応したrg-treeのocclistとのペアのリストにして返す
@param rg_tree
@param id_lists
@return vector<CP*> 
 */
vector<CP*> get_cp_list_corresponding_to_ids(RGTree* rg_tree,vector<vector<int>>id_lists){
    vector<CP*> result;
    for(int i = 0, n = id_lists.size();i<n;i++){
      vector<int> occ = rg_tree->filter_rgtree_occurrence(id_lists[i],rg_tree->item_list);
      result.push_back(new CP(id_lists[i],occ));
    }
    return result;
}


/*
@brief rg-treeにノードidでフィルターをかけてclosed-treeを生成
@param rg-tree
@param closed-pattern
@return closed-tree(EnumerationTree)
@detail
根ノードだけ先に作っておいてそれを再帰的に拡張
*/
 EnumerationTree* get_closed_tree(RGTree* rg_tree,CP* closed_pattern){
   EnumerationTree* ne = new EnumerationTree(rg_tree->get_node()->id,rg_tree->get_node()->label);
   //cout << "before make_closed" << (closed_pattern->occ_list).size()<< endl;
   make_closed_tree_rec(rg_tree,closed_pattern->id_list,ne);
   //cout << "after make_closed " << endl;
   return ne;
 }


/*
@brief C言語で書かれたLCMの呼び出し
@sa SCC_Path_Miner
 */
vector<vector<int>> calling_LCM(vector<vector<int>> in,int min_sup){
 //他LCMの呼び出し，ファイル入出力は時間に入れない  
  algorithm_stop();
  write_file_to_item_transactions(in);
  algorithm_restart();
  LCM_start();
  Mine_Closed_Itemsets(min_sup);
  algorithm_stop();
  LCM_end();
  vector<vector<int>> r = read_result_of_lcm();
  algorithm_restart();
  return r;
}

/*
@brief C言語で書かれたLCMの呼び出し
@sa SCC_Miner SCC_Miner_Improved
 */
vector<CP*> callingLCM(vector<vector<int>> in ,int min_sup,RGTree* rg_tree){
 //他LCMの呼び出し，ファイル入出力は時間に入れない  
  /*  algorithm_stop();
  write_file_to_item_transactions(in);
  algorithm_restart();
  LCM_start();
  Mine_Closed_Itemsets(min_sup);
  algorithm_stop();
  LCM_end();
  vector<vector<int>> r = read_from_LCM_ver2_result();
  algorithm_restart();*/
  vector<vector<int>> r = calling_LCM(in,min_sup);
  algorithm_stop();
  //アイテムセットのOccurrenceを取る，本来ならLCMの中でやるがOccurrenceListの取り方が分からない
  vector<CP*> cp = convert_to_CP(r,rg_tree);
  algorithm_restart();
  return cp;
}




 /*
@brief SCC-Minerアルゴリズム本体
@param1 db 木のデータベースへのポインタ
@param2 constrainedTree 部分木制約の木(ここは木リストに変更することも考えられる)
@param3 minimum_support ミニマムサポート
@param4 rpcut_flag RPTreeを削減するかどうか
@return 部分木制約を満たす頻出飽和な木パターンを全て含んだvector
@detail
1. db中のconstrainedTreeのポインタを全部列挙する
2. rp-treeを作る
3. 見られるべきrootを列挙する
4. 各rootに対してRG-Treeを作る
5. 各RG-TreeにLCMを呼び出す
6. 5で得られたパタンに対してrp-treeを見ながらフィルターをかける
 */
vector<EnumerationTree*> SCC_Miner(TreeDB* db,EnumerationTree* constrainedTree, int minimum_support,bool rpcut_flag){
  vector<Tree*> oc_list = db->get_subtree_list(constrainedTree);
  
  vector<EnumerationTree*> result;
  if(oc_list.size() < minimum_support){return result;}
  //calc rp_tree
  RPTree* rp_tree = new RPTree(constrainedTree->get_node()->label,oc_list);
  exp_rp_tree(rp_tree,minimum_support);
  rp_tree->reindexing(0);
  //vector<RPTree*> root_candidates = get_root_candidates(rp_tree,rp_tree,false);
  //  cout << "RPTree size before cut is " << rp_tree->get_num_of_nodes() << endl;
  //cout << "root candidates size before cut is " << root_candidates.size() << endl;
  if(rpcut_flag){ 
    rp_tree_cutter(rp_tree,rp_tree);
    rp_tree->reindexing(0);
  }
  vector<RPTree*> root_candidates = get_root_candidates(rp_tree,rp_tree,false);
  //cout << "RPTree size after cut is " << rp_tree->get_num_of_nodes() << endl;
  //cout << "root candidates size after cut is " << root_candidates.size() << endl;
  set_num_of_root_candidate(root_candidates.size());
  //generate rg_tree
  for(int i = 0,n = root_candidates.size() ; i < n ; i++){
    RGTree* rg_tree = new RGTree(root_candidates[i]);
    exp_rg_tree(rg_tree,minimum_support);
    rg_tree->reindexing(0);

    //他作LCMの呼び出し
    vector<CP*> closed_patterns = callingLCM(convert(rg_tree->get_item_transaction()),minimum_support,rg_tree);
  
    //自作LCM呼び出し
    /*    vector<vector<int>> vv =  convert(rg_tree->get_item_transaction());
    vector<vector<int>> id_lists = Mine_Closed_Itemsets(vv,minimum_support);
    vector<CP*> closed_patterns = get_cp_list_corresponding_to_ids(rg_tree,id_lists);    */
    
    for(int j = 0 , m = closed_patterns.size();j<m;j++){
      //cout << "is_there_occ_matched " << j << "  root_c size is" << n<< endl;
      if(!is_there_occurrence_matched(root_candidates[i],closed_patterns[j])){
	//	cout << "closed !!" << endl;
	result.push_back(get_closed_tree(rg_tree,closed_patterns[j]));
      }
    }
    set_current_memory();
    delete rg_tree;

  }

  delete rp_tree;
  return result;
}


 /*
@brief SCC-Miner-improvedアルゴリズム本体
@param1 db 木のデータベースへのポインタ
@param2 constrainedTree 部分木制約の木(ここは木リストに変更することも考えられる)
@param3 minimum_support ミニマムサポート
@return 部分木制約を満たす頻出飽和な木パターンを全て含んだvector
@detail
1. db中のconstrainedTreeのポインタを全部列挙する
2. rp-treeを作る
3. 見られるべきrootを列挙する(ここでrp-treeのoccurrence_listの包含関係をチェック)
4. 各rootに対してRG-Treeを作る
5. 各RG-TreeにLCMを呼び出す
6. 5で得られたパタンに対してrp-treeを見ながらフィルターをかける
 */
vector<EnumerationTree*> SCC_Miner_Improved(TreeDB* db, EnumerationTree* constrainedTree, int minimum_support){
  vector<Tree*> oc_list = db->get_subtree_list(constrainedTree);
  vector<EnumerationTree*> result;
  if(oc_list.size() < minimum_support){return result;}
  RPTree* rp_tree = new RPTree(constrainedTree->get_node()->label,oc_list);
  exp_rp_tree(rp_tree,minimum_support);
  rp_tree->reindexing(0);  
  //  rp_tree->print_tree();
  vector<RPTree*> root_candidates = get_root_candidates(rp_tree,rp_tree,true);
  cout << "after enumerate root_candidates"  << endl;
  rp_tree->reindexing(0);
  set_num_of_root_candidate(root_candidates.size());
  //generate rg_tree
  for(int i = 0,n = root_candidates.size() ; i < n ; i++){
    RGTree* rg_tree = new RGTree(root_candidates[i]);
    exp_rg_tree(rg_tree,minimum_support);
    rg_tree->reindexing(0);
    
    //他作LCM
    vector<CP*> closed_patterns = callingLCM(convert(rg_tree->get_item_transaction()),minimum_support,rg_tree);


    //自作
    /*    vector<vector<int>> vv =  convert(rg_tree->get_item_transaction());
    vector<vector<int>> id_lists = Mine_Closed_Itemsets(convert(vv),minimum_support);
    vector<CP*> closed_patterns = get_cp_list_corresponding_to_ids(rg_tree,id_lists);*/

    for(int j = 0 , m = closed_patterns.size();j<m;j++){
      if(!is_there_occurrence_matched(root_candidates[i],closed_patterns[j])){
	result.push_back(get_closed_tree(rg_tree,closed_patterns[j]));
      }
    }
    set_current_memory();
    delete rg_tree;
  }
  delete rp_tree;
  return result;

}

 /*
@brief SCC-Path-Minerアルゴリズム本体
@param1 db 木のデータベースへのポインタ
@param2 constrainedTree 部分木制約の木(ここは木リストに変更することも考えられる)
@param3 minimum_support ミニマムサポート
@param4 rpcut_flag RPTree削減手法を適用するかどうか
@return 部分木制約を満たす頻出飽和な木パターンを全て含んだvector
@detail
1. db中のconstrainedTreeのポインタを全部列挙する
2. rp-treeを作る
3. 見られるべきrootを列挙する(ここでrp-treeのoccurrence_listの包含関係をチェック)
4. constrained-treeの根からrp-pathを列挙
5. rp-pathの先からrg-pathを列挙
6. LCMをかける
7. 各アイテムをマージする
 */
vector<EnumerationTree*> SCC_Path_Miner(TreeDB* db,EnumerationTree* constrainedTree, int minimum_support,bool rpcut_flag){
  vector<Tree*> oc_list = db->get_subtree_list(constrainedTree);
  vector<EnumerationTree*> result;
  if(oc_list.size() < minimum_support){return result;}
  RPTree* rp_tree = new RPTree(constrainedTree->get_node()->label,oc_list);
  exp_rp_tree(rp_tree,minimum_support);
  rp_tree->reindexing(0);

  if(rpcut_flag){
    rp_tree_cutter(rp_tree,rp_tree);
    rp_tree->reindexing(0);
  }
  vector<int> dummy;
  vector<Path_OCCL*> p = rp_tree->get_POCCL_list(dummy);
  vector<RPRGPathItem*> rprg_list = enum_rprg_item_list(p,minimum_support,true);


  vector<vector<int>> r = convert_rprg_list(rprg_list);
  r = calling_LCM(convert(r),minimum_support);

  //自作LCM
  //r = Mine_Closed_Itemsets(r,minimum_support);

  for(int i = 0,n = r.size();i<n;i++){
    result.push_back(merge_item_result(r[i],rprg_list));
  }
  set_current_memory();
  return result;
}



#endif //SCCMiner
