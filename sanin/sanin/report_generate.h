#ifndef _REPORT_GENERATE_H_
#define _REPORT_GENERATE_H_
#include <vector>
using namespace std;

struct data_map{
  int id;
  char *name;
  data_map(int id, const char *name):id(id){
    this->name = strdup(name);
  }
  ~data_map(){
    free(name);
  }
};

void handle_get_html(json_t *root, struct client *client);
void handle_get_pdf(json_t *root, struct client *client);
bool parse_data_map(const json_t *root, vector<data_map*> *accounts, const char *map_name);
#endif //_REPORT_GENERATE_H_
