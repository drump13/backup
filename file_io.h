#ifndef FILE_IO
#define FILE_IO


#include "misc.h"
#include "tree.h"

static string INPUT_FILE = "input.txt";
static string EXCHANGE_OUT_FILE = "DB.txt";
static string EXCHANGE_IN_FILE = "out.txt";
static string OUTPUT_FILE = "output.txt";
static string CS_LOGS = "cslogs.asc";

struct CP{
  vector<int> id_list;
  vector<int> occ_list;
  CP(vector<int> id_list,vector<int> occ_list){
    this->id_list = id_list;
    this->occ_list = occ_list;
  }
};

void write_file_to_item_transactions(vector<vector<int>> transactions);
vector<string> read_tree_db();
vector<vector<int>> read_result_of_lcm();
vector<vector<int>> read_from_LCM_ver2_result();
vector<CP*> read_CP_from_LCM_result();
vector<CP*> convert_to_CP(vector<vector<int>>id_lists,RGTree* rgtree,bool is_memred);
void write_final_result(vector<EnumerationTree*> ets ,string filename);
void write_experiment_time_result(string filename,vector<int> minsup_list,vector<double> timelist);
void write_experiment_memory_result(string filename,vector<int> minsup_list,vector<int> memroylist);
vector<string> read_cslogs();
vector<string> read_tree_file(string filename);
string to_csstr(vector<string> sv);
vector<vector<int>> convert(vector<vector<int>> vvi);
TreeDB* get_treedb_from_file(string filename);

//debug
void print_CP(CP* cp);
void print_CP_list(vector<CP*> cp_list);
void print_vvi(vector<vector<int>> vvi);
#endif //FILE_IO
