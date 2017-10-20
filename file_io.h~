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
vector<CP*> read_CP_from_LCM_ver2_result(RGTree* rgtree);
vector<CP*> read_CP_from_LCM_result();
void write_final_result(vector<EnumerationTree*> ets ,string filename);
vector<string> read_cslogs();
string to_csstr(vector<string> sv);
vector<vector<int>> convert(vector<vector<int>> vvi);
TreeDB* get_treedb_from_file(string filename);

//debug
void print_CP(CP* cp);
void print_CP_list(vector<CP*> cp_list);
void print_vvi(vector<vector<int>> vvi);
#endif //FILE_IO
