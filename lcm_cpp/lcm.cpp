#include "lcm.h"

/*********************
ここからアイテムセットクラスの定義
@brief LCMに必要な関数群
 ******************/

/*
@brief アイテムセットのコンストラクタ
@param vec
@detail
vecはソートされている前提
 */
Itemset::Itemset(vector<int> vec){
  for(int i = 0 ,n = vec.size();i<n;i++){
    items.push_back(vec[i]);
  }
  
}
/*
@brief X(i) = X and {1,2,...,i}
@param i
 */
Itemset* Itemset::get_prefix(int i){
  vector<int> v;
  int k = 0;
  int item_size = items.size();
  while(items[k]<=i && k < item_size){
    v.push_back(items[k]);
    k++;
  }
  return new Itemset(v);
}

/*
@brief 末尾のアイテムtail({1,2,...,k})=k
*/
int Itemset::get_tail(){
  return items[items.size()-1];
}
/*
@brief このアイテムが引数のアイテムを包含していたとき,trueを返す
 */
bool Itemset::is_included(vector<int> it){
  vector<int> r;
  set_intersection(items.begin(),items.end(),it.begin(),it.end(),back_inserter(r));
  return r.size() == it.size();
}

//debug
void Itemset::print_items(){
  cout << "{";
  for(int i = 0 , n = items.size();i<n;i++){
    cout << items[i] << ",";
  }
  cout << "}" << endl;
}



/********************
ここからアイテムセットDBクラスの定義
@brief LCMに必要な関数群
 ******************/
ItemsetDB::ItemsetDB(vector<vector<int>> items){
  for(int i =0,n = items.size();i < n ; i++){
    Itemset* it = new Itemset(items[i]);
    itemsets.push_back(it);
  }
}


/*
@brief アイテムのリストを受け取って，そのリストを含むtransaction idを返す
つまりoccurrence setを返す
 */
vector<int> ItemsetDB::get_occurrences(vector<int> items){
  vector<int> result;
  for(int i = 0 , n = itemsets.size();i<n;i++){
    if(itemsets[i]->is_included(items)){
      result.push_back(i);
    }
  }
  return result;
}


/*
@brief occurrenceで指定されたアイテムセットのandをとって返す
@detail
空のoccurrenceが渡されることは想定していない
 */
vector<int> ItemsetDB::get_occ_intersection(vector<int> occurrences){
  vector<int> result = itemsets[occurrences[0]]->items;
  for(int i = 1 , n = occurrences.size();i < n; i++){
    int co = occurrences[i];
    result = get_intersection(result,itemsets[co]->items);
  }

  return result;
}


/******************
以下，お便利メソッド群
 ****************/
/*
  @brief 2つのリストのandを返す
 */
vector<int> get_intersection(vector<int> a,vector<int> b){
  vector<int> r;
  set_intersection(a.begin(),a.end(),b.begin(),b.end(),back_inserter(r));
  return r;
}
/*
@brief2個のリストが同じものかどうかを返す，同じならtrue
*/
bool is_same_vec(vector<int> a,vector<int> b){
  vector<int> r;
  set_intersection(a.begin(),a.end(),b.begin(),b.end(),back_inserter(r));
  return r.size() ==a.size() && r.size() == b.size();
}

//debug
void print_vec(vector<int> a){
  cout << "{" ;
  for(int i = 0,n= a.size();i<n;i++){
    cout<< a[i] << ",";
  }
  cout << "}" << endl;
}


/****アルゴリズム本体*****/
vector<Itemset*> LCM_Algorithm(Itemset* fc_item,int min_sup,ItemsetDB* itemdb){
  vector<Itemset*> result;
  //todo
  return result;
}




/*********外から呼ばれる***********/

vector<CP*> Mine_Closed_Pattern(vector<vector<int>> itemsets,int min_sup){
  
  vector<CP*> result;
  //todo
  return result;
}

vector<vector<int>> Mine_Closed_Itemsets(vector<vector<int>> itemsets,int min_sup){
    vector<vector<int>> result;
    //todo
    return result;
}
