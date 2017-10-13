#include "file_io.h"


/*
@brief LCMに渡すファイルを生成する
@param RG-Treeを走査して得られたitem_vectorのvector
@detail
occurrenceのvectorにするためにまずは2次元配列の行と列を入れ替える操作を行う
*/
void write_file_to_item_transactions(vector<vector<int>> tr){
  cout << "before convert " << endl;
  vector<vector<int>> transactions = convert(tr);
  cout << "before output file stream" << endl;
  ofstream output_file(EXCHANGE_OUT_FILE);
  cout << "before write " << endl;
  for(int i = 0,n=transactions.size(); i<n;i++){
    for(int j =0,m=transactions[i].size();j<m;j++){
      output_file << transactions[i][j] << " ";
    }
    output_file << "\n";
  }
  output_file.close();
}

vector<string> read_tree_db(){
  vector<string> result;
  //todo
  

  return result;
}

/*
@brief LCMの結果を読むための関数
 */
vector<vector<int>> read_result_of_lcm(){
  vector<vector<int>> result;
  ifstream ifs(EXCHANGE_IN_FILE);
  if(ifs.fail()){
    cerr << "file input is failed" << endl;
  }
  string s;
  int i = 0;
  while(getline(ifs,s)){
    ++i;
    if(i%2 != 1){
      continue;
    }
    vector<string> sv = split(s,' ');
    if(sv.size() == 0){continue;}
    vector<int> current;
    for(int j = 1 ,m = sv.size();j<m;j++){
      current.push_back(stoi(sv[j]));
    }
    sort(current.begin(),current.end());
    result.push_back(current);
  }

  return result;
}

/*
@brief LCMの結果をDB.txtから読む
@param min_sup
@return CP occ_list,idlistのペア
@detail occがmin_sup以下のclosed-patternを無視する
 */
vector<CP*> read_CP_from_LCM_result(int min_sup){
  ifstream ifs(EXCHANGE_IN_FILE);
  if(ifs.fail()){
    cerr << "file input is failed" << endl;
  }
  string s;
  bool skip_flag = false;
  vector<vector<int>> occ_lists;
  vector<vector<int>> id_lists;
  int line = 0;
  while(getline(ifs,s)){
    vector<string> sv = split(s,' ');
    if(line%2 == 0){
      vector<int> current;
      for(int i = 1,n=sv.size();i<n;i++){
	current.push_back(stoi(sv[i]));
      }
      sort(current.begin(),current.end());
      id_lists.push_back(current);

    }else{
      vector<int> current;
      for(int i = 1,n = sv.size();i<n;i++){
	current.push_back(stoi(sv[i]));
      }
      sort(current.begin(),current.end());
      occ_lists.push_back(current);
    }
    
    ++line;
  }
  
  vector<CP*> result;
  for(int i = 0 , n = id_lists.size(); i < n; i++){
    CP* cp = new CP(id_lists[i],occ_lists[i]);
    result.push_back(cp);
  }
  return result;
  
}
//@debug
void print_CP(CP* cp){
    cout << "[";
    for(int i = 0, n =cp->id_list.size();i<n;i++){
      cout << cp->id_list[i]<< ",";
    }
    cout << "]:{";
    for(int i = 0 ,n= cp->occ_list.size();i<n;i++){
      cout << cp->occ_list[i] << ",";
    }
    cout << "}" << endl;
  }
  //debug
  void print_CP_list(vector<CP*> cp_list){
    for(int i = 0,n=cp_list.size() ; i < n ; i++){
      cout << "---"<<endl;
      print_CP(cp_list[i]);
    }
  }

void write_final_result(vector<EnumerationTree*> result,string filename = OUTPUT_FILE){
  ofstream output_file(filename);
  for(int i = 0,n=result.size(); i<n;i++){
    output_file << result[i]->get_tree_string();
    output_file << "\n";
  }
  output_file.close();
  
  //todo
} 

/*
cs_logs専用のリーダー
cs_logs.ascの最初の2要素がtree_idであることに注意
 */
vector<string> read_cslogs(){
  ifstream input_file(CS_LOGS);
  if(input_file.fail()){
    cerr << "file input is failed" << endl;
  }
  string s;
  vector<string> result;
  while(getline(input_file,s)){
    vector<string> sv = split(s,' ');
    result.push_back(to_csstr(sv));
  }
  return result;
}


string to_csstr(vector<string> sv){
  string result="";
  for(int i = 3,n = sv.size();i<n;i++){
    result += sv[i]+" "; 
  }
  return result;
}

/*
@brief 中のベクターのmax sizeを返す
 */
int get_max_size(vector<vector<int>> transaction){
    int max=0;
    for(int i = 0 , n = transaction.size();i<n;i++){
      int current_size = transaction[i].size();
      if(max < current_size){max = current_size;}
    }
    return max;
}

/*
  @brief 2次元配列の行と列を入れ替える
 */
vector<vector<int>> convert(vector<vector<int>> transaction){
  int max = get_max_size(transaction);  
  vector<vector<int>> result;
  for(int j = 0;j<max;j++){
    vector<int> v;
    result.push_back(v);
  }
  for(int i = 0 , n=transaction.size();i<n;i++){
    for(int j =0, m = transaction[i].size();j<m;j++){
      result[transaction[i][j]].push_back(i);
    }
  }
    //    print_vvi(result);
    return result;
}

/*
@brief 指定されたファイルから木DBを読んで返す
@param filename
 */
TreeDB* get_treedb_from_file(string filename){
  ifstream input_file(filename);
  if(input_file.fail()){
    cerr << "file input is failed" << endl;
  }
  string s;
  vector<string> tree_strings;
  while(getline(input_file,s)){
    tree_strings.push_back(s);
  }

  TreeDB* result = new TreeDB();
  for(int i = 0, n = tree_strings.size();i < n;i++){
    result->add_tree(make_tree(tree_strings[i]));    
  }

  return result;
  
}


//debug
void print_vvi(vector<vector<int>> vvi){
    cout << "print vector vector int" << endl;
    for(int i = 0,n = vvi.size();i<n;i++){
      for(int j = 0,m = vvi[i].size();j<m;j++){
	cout << vvi[i][j] << "," ;
      }
      cout << endl;
    }
}
