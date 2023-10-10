#include "common.h"
#include "report_generate.h"
#include "utils.h"
#include "balance_sheet.h"
#include "profit_n_loss.h"
#include "cash_flow.h"
#include "tds_summary.h"
#include "tcs_summary.h"
#include "wkhtmltox/pdf.h"

using namespace std;


enum {
  REPORT_PROFIT_N_LOSS = 1,
  REPORT_BALANCE_SHEET,
  REPORT_CASH_FLOW,
  REPORT_PAYMENTS_RECEIVED,
  REPORT_SALES_REFUND_HISTORY,
  REPORT_TDS_SUMMARY,
  REPORT_TCS_SUMMARY,
  REPORT_INVENTORY_SUMMARY,
};

char* json_to_cstring(const char* jsonString) {
  size_t len = strlen(jsonString);
  char* cString = (char*)malloc(len * 2 + 3); // Add extra space for escaping and formatting

  char* dest = cString;
  *dest++ = '"';

  for (size_t i = 0; i < len; i++) {
    if (jsonString[i] == '"') {
      *dest++ = '\\'; // Prepend a backslash before double quotes
    }

    if (jsonString[i] == '\n') {
      *dest++ = '"';
      *dest++ = '\n';
      *dest++ = '"';
    } else {
      *dest++ = jsonString[i];
    }
  }
  *dest++ = '"';
  *dest = '\0';
  return cString;
}

bool parse_data_map(const json_t *root, vector<data_map*> *accounts, const char *map_name){
  bool ret = false;
  do {
    json_t* json_map = json_object_get(root, map_name);
    if(!json_map || !json_is_array(json_map)){
      ERROR_LOG("mapping array (%s) not found..", map_name);
      break;
    }
    json_t *node;
    int index = 0;
    json_array_foreach(json_map, index, node){
      if(json_is_object(node)){
        int id = 0;
        json_t *json_handle = json_object_get(node, "id");
        if(json_handle && json_is_integer(json_handle)){
          id = json_integer_value(json_handle);
        } else {
          continue;
        }
        json_handle = json_object_get(node, "name");
        if(!json_handle || !json_is_string(json_handle)){
          continue;
        }
        data_map* accnt_map = new data_map(id, json_string_value(json_handle));
        accounts->push_back(accnt_map);
      }
    }
    ret = true;
  } while(0);
  return ret;
}


void handle_get_html(json_t *old_root, struct client *client){

  const char* my_input =
  "{"
" \"to_date\": 1795839400,"
" \"from_date\": 1795839400,"
" \"contact_mapping\": ["
"  {"
"   \"id\": 1,"
"   \"name\": \"Mrs. Ponny\""
"  },"
"  {"
"   \"id\": 2,"
"   \"name\": \"Flashter Inc.\""
"  },"
"  {"
"   \"id\": 3,"
"   \"name\": \"Ethan\""
"  },"
"  {"
"   \"id\": 4,"
"   \"name\": \"Ganesh\""
"  },"
"  {"
"   \"id\": 5,"
"   \"name\": \"Ram\""
"  },"
"  {"
"   \"id\": 6,"
"   \"name\": \"Unitech Hi-tech Structures Ltd\""
"  },"
"  {"
"   \"id\": 7,"
"   \"name\": \"Mr. Jabu Ran\""
"  },"
"  {"
"   \"id\": 8,"
"   \"name\": \"VIRAT KOHLI\""
"  },"
"  {"
"   \"id\": 9,"
"   \"name\": \"Abc\""
"  },"
"  {"
"   \"id\": 10,"
"   \"name\": \"Gannu\""
"  },"
"  {"
"   \"id\": 11,"
"   \"name\": \"Sabu\""
"  },"
"  {"
"   \"id\": 12,"
"   \"name\": \"Mrs. Exporter\""
"  },"
"  {"
"   \"id\": 13,"
"   \"name\": \"Mr. aji dharman\""
"  },"
"  {"
"   \"id\": 14,"
"   \"name\": \"kuttu\""
"  }"
" ],"
" \"method\": 138,"
" \"currency_symbol\": \"&#8377;\","
" \"report_type\": 7,"
" \"file_name\": \"tcs_summary.html\","
" \"date_format\": 1,"
" \"company_name\": \"Kesans\","
" \"currency_format\": {"
"  \"group\": 2,"
"  \"thousand_seperator\": 2,"
"  \"sub_unit_seperator\": 3,"
"  \"decimal_places\": 2"
" },"
" \"report\": {"
"  \"items\": ["
"   {"
"    \"date\": 1536949800,"
"    \"number\": \"INV-000001\","
"    \"id\": 2,"
"    \"type\": 2,"
"    \"customer_id\": 1,"
"    \"pan\": \"AAACV5492Q\","
"    \"rate\": 10.0,"
"    \"total\": 21255.640000409232,"
"    \"tcs_amount\": 2125.56005859375,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1607193000,"
"    \"number\": \"INV-000002\","
"    \"id\": 4,"
"    \"type\": 2,"
"    \"customer_id\": 4,"
"    \"rate\": 3.0,"
"    \"total\": 138702.0,"
"    \"tcs_amount\": 1500.0,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1607625000,"
"    \"number\": \"INV-000005\","
"    \"id\": 7,"
"    \"type\": 2,"
"    \"customer_id\": 1,"
"    \"pan\": \"AAACV5492Q\","
"    \"rate\": 3.0,"
"    \"total\": 658.71713176992603,"
"    \"tcs_amount\": 19.760000228881836,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1607711400,"
"    \"number\": \"CN-00004\","
"    \"id\": 3,"
"    \"type\": 5,"
"    \"customer_id\": 3,"
"    \"pan\": \"GSPTN0371G\","
"    \"rate\": 3.0,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1610994600,"
"    \"number\": \"INV-000007\","
"    \"id\": 9,"
"    \"type\": 2,"
"    \"customer_id\": 1,"
"    \"pan\": \"AAACV5492Q\","
"    \"rate\": 3.0,"
"    \"total\": 29.400000005960464,"
"    \"tcs_amount\": 0.87999999523162842,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1622226600,"
"    \"number\": \"INV-000016\","
"    \"id\": 17,"
"    \"type\": 2,"
"    \"customer_id\": 10,"
"    \"rate\": 3.0,"
"    \"total\": 50858.099999769198,"
"    \"tcs_amount\": 1525.739990234375,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1681324200,"
"    \"number\": \"CN-00013\","
"    \"id\": 10,"
"    \"type\": 5,"
"    \"customer_id\": 3,"
"    \"pan\": \"GSPTN0371G\","
"    \"rate\": 3.0,"
"    \"collection_code\": \"6CR\""
"   },"
"   {"
"    \"date\": 1681324200,"
"    \"number\": \"CDN-000008\","
"    \"id\": 6,"
"    \"type\": 6,"
"    \"customer_id\": 3,"
"    \"pan\": \"GSPTN0371G\","
"    \"rate\": 3.0,"
"    \"total\": 104.99999991059303,"
"    \"tcs_amount\": 3.1500000953674316,"
"    \"collection_code\": \"6CR\""
"   }"
"  ]"
" }"
"}";

  json_error_t err;
  json_t *root = json_loads(my_input, 0, &err);
  char *json_txt = json_dumps(root, 1);
  char *c_string = json_to_cstring(json_txt); 
  printf("Got the json - %s\n", c_string);
  free(c_string);
  free(json_txt);
  json_t *json_handle = json_object_get(root, "file_name");
  CurrencyFormat *currency_format = NULL;
  char *currency_symbol= NULL;
  char *company_name = NULL;
  int from_date = 0;
  int to_date =0;
  int date_format = 0;
  FILE *fp = NULL;
  do {
    if(!json_handle || !json_is_string(json_handle)){
      write_response(failed_json, client);
      break;
    }
    fp = fopen(json_string_value(json_handle), "w");
    if(!fp){
      ERROR_LOG("Not able to open file -> %s\n", json_string_value(json_handle));
      write_response(failed_json, client);
      break;
    }
    json_handle = json_object_get(root, "company_name");
    if(json_handle && json_is_string(json_handle)){
      company_name = strdup(json_string_value(json_handle));
    }
    json_handle = json_object_get(root, "from_date");
    if(json_handle && json_is_integer(json_handle)){
      from_date = json_integer_value(json_handle);
    }
    json_handle = json_object_get(root, "to_date");
    if(json_handle && json_is_integer(json_handle)){
      to_date = json_integer_value(json_handle);
    }
    json_handle = json_object_get(root, "date");
    if(json_handle && json_is_integer(json_handle)){
      from_date = json_integer_value(json_handle);
    }
    json_handle = json_object_get(root, "date_format");
    if(json_handle && json_is_integer(json_handle)){
      date_format = json_integer_value(json_handle);
    }
    json_handle = json_object_get(root, "currency_symbol");
    if(json_handle && json_is_string(json_handle)){
      currency_symbol = strdup(json_string_value(json_handle));
    }

    currency_format = get_currecny_format(root);
    
    json_handle = json_object_get(root, "report_type");
    bool error = false;
    if(json_handle && json_is_integer(json_handle)){
      int report_type = json_integer_value(json_handle);
      switch(report_type){
        case REPORT_BALANCE_SHEET:
          if(!process_balance_sheet(root, from_date, date_format, 
                company_name, currency_symbol, currency_format, fp)){
            error = true;
          }
          break;
        case REPORT_PROFIT_N_LOSS:
            if(!process_profit_n_loss(root, from_date,to_date,date_format, 
                company_name, currency_symbol, currency_format, fp)){
            error = true;
          }
        case REPORT_CASH_FLOW:
            if(!process_cash_flow(root, from_date,to_date,date_format, 
                company_name, currency_symbol, currency_format, fp)){
            error = true;
          }

        case REPORT_TDS_SUMMARY:
          if(process_tds_summary(root, from_date, to_date, date_format, 
              company_name, currency_symbol, currency_format, fp)){
            error = true;
          }
        
        case REPORT_TCS_SUMMARY:
          if(process_tcs_summary(root, from_date, to_date, date_format, 
              company_name, currency_symbol, currency_format, fp)){
            error = true;
          }
        default:;
      }
    }
    if(error){
      write_response(failed_json, client);
    } else {
      write_response(success_json, client);
    }
  } while (0);
  delete currency_format;
  free(company_name);
  free(currency_symbol);
  fclose(fp);
  //write_response(success_json, client);
}

void handle_get_pdf(json_t *root, struct client *client){
  char *json_txt = json_dumps(root, 1);
  printf("Got the json (in pdf) - %s\n", json_txt);
  free(json_txt);
  write_response(success_json, client);
}

