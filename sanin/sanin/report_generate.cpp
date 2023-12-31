#include "common.h"
#include "report_generate.h"
#include "utils.h"
#include "balance_sheet.h"
#include "profit_n_loss.h"
#include "cash_flow.h"
#include "tds_summary.h"
#include "tcs_summary.h"
#include "payments_received.h"
#include "sales_refund_history.h"
#include "account_transactions.h"
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
  REPORT_ACCOUNT_TRANSACTIONS = 11,
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
" \"basis\": \"cash\","
" \"method\": 138,"
" \"currency_symbol\": \"&#8377;\","
" \"report_type\": 11,"
" \"file_name\": \"account_transactions.html\","
" \"date_format\": 1,"
" \"company_name\": \"Kesans\","
" \"currency_format\": {"
"  \"group\": 2,"
"  \"thousand_seperator\": 2,"
"  \"sub_unit_seperator\": 3,"
"  \"decimal_places\": 2"
" },"
" \"report\": {"
"  \"closing_balance\": 0.0,"
"  \"opening_balance\": 0.0,"
"  \"transactions\": ["
"   {"
"    \"id\": 3650,"
"    \"account_id\": 73,"
"    \"transaction_type\": \"Payment Refund\","
"    \"date\": 1649356200,"
"    \"amount\": -800.0,"
"    \"related_id\": 11,"
"    \"related_number\": \"15\","
"    \"reference\": \"number\","
"    \"type\": 11,"
"    \"status\": 2,"
"    \"details\": []"
"   },"
"   {"
"    \"id\": 3649,"
"    \"account_id\": 23,"
"    \"transaction_type\": \"Payment Refund\","
"    \"date\": 1649356200,"
"    \"amount\": 800.0,"
"    \"related_id\": 11,"
"    \"related_number\": \"15\","
"    \"reference\": \"number\","
"    \"type\": 11,"
"    \"status\": 2,"
"    \"details\": []"
"   },"
"   {"
"    \"id\": 2778,"
"    \"account_id\": 13,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1650479400,"
"    \"amount\": 2000.0,"
"    \"related_id\": 20,"
"    \"related_number\": \"8\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Mrs. Ponny\""
"    ]"
"   },"
"   {"
"    \"id\": 2777,"
"    \"account_id\": 23,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1650479400,"
"    \"amount\": -2000.0,"
"    \"related_id\": 20,"
"    \"related_number\": \"8\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Mrs. Ponny\""
"    ]"
"   },"
"   {"
"    \"id\": 3619,"
"    \"account_id\": 13,"
"    \"transaction_type\": \"Card Payments\","
"    \"date\": 1655836200,"
"    \"amount\": -1200.0,"
"    \"related_id\": 2,"
"    \"reference\": \"number\","
"    \"type\": 24,"
"    \"status\": 2,"
"    \"details\": ["
"     \"test card\""
"    ]"
"   },"
"   {"
"    \"id\": 3618,"
"    \"account_id\": 75,"
"    \"transaction_type\": \"Card Payments\","
"    \"date\": 1655836200,"
"    \"amount\": 1200.0,"
"    \"related_id\": 2,"
"    \"reference\": \"number\","
"    \"type\": 24,"
"    \"status\": 2,"
"    \"details\": ["
"     \"test card\""
"    ]"
"   },"
"   {"
"    \"id\": 2782,"
"    \"account_id\": 73,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1666204200,"
"    \"amount\": 2500.0,"
"    \"related_id\": 21,"
"    \"related_number\": \"26\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 2781,"
"    \"account_id\": 23,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1666204200,"
"    \"amount\": -2440.0,"
"    \"related_id\": 21,"
"    \"related_number\": \"26\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 2788,"
"    \"account_id\": 34,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1666204200,"
"    \"amount\": -55.555556000000003,"
"    \"related_id\": 21,"
"    \"related_number\": \"26\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 2785,"
"    \"account_id\": 67,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1666204200,"
"    \"amount\": -1.75,"
"    \"related_id\": 21,"
"    \"related_number\": \"26\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 2784,"
"    \"account_id\": 32,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1666204200,"
"    \"amount\": 0.083333000000000004,"
"    \"related_id\": 21,"
"    \"related_number\": \"26\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 2783,"
"    \"account_id\": 19,"
"    \"transaction_type\": \"Payment Received\","
"    \"date\": 1666204200,"
"    \"amount\": -2.7777780000000001,"
"    \"related_id\": 21,"
"    \"related_number\": \"26\","
"    \"reference\": \"number\","
"    \"type\": 10,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 3475,"
"    \"account_id\": 73,"
"    \"transaction_type\": \"Payment Made\","
"    \"date\": 1668191400,"
"    \"amount\": -111222.0,"
"    \"related_id\": 14,"
"    \"related_number\": \"18\","
"    \"reference\": \"number\","
"    \"type\": 14,"
"    \"status\": 2,"
"    \"details\": ["
"     \"kuttu\""
"    ]"
"   },"
"   {"
"    \"id\": 3474,"
"    \"account_id\": 10,"
"    \"transaction_type\": \"Payment Made\","
"    \"date\": 1668191400,"
"    \"amount\": 111222.0,"
"    \"related_id\": 14,"
"    \"related_number\": \"18\","
"    \"reference\": \"number\","
"    \"type\": 14,"
"    \"status\": 2,"
"    \"details\": ["
"     \"kuttu\""
"    ]"
"   },"
"   {"
"    \"id\": 4298,"
"    \"account_id\": 64,"
"    \"transaction_type\": \"Invoice\","
"    \"date\": 1672079400,"
"    \"amount\": 42.682898999999999,"
"    \"related_id\": 65,"
"    \"related_number\": \"INV-000063\","
"    \"reference\": \"number\","
"    \"type\": 2,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 4297,"
"    \"account_id\": 16,"
"    \"transaction_type\": \"Invoice\","
"    \"date\": 1672079400,"
"    \"amount\": -42.682898999999999,"
"    \"related_id\": 65,"
"    \"related_number\": \"INV-000063\","
"    \"reference\": \"number\","
"    \"type\": 2,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Ethan\""
"    ]"
"   },"
"   {"
"    \"id\": 2865,"
"    \"account_id\": 10,"
"    \"transaction_type\": \"Expense\","
"    \"date\": 1678559400,"
"    \"amount\": -7000.0,"
"    \"related_id\": 23,"
"    \"reference\": \"number\","
"    \"type\": 4,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Advance Tax\""
"    ]"
"   },"
"   {"
"    \"id\": 2864,"
"    \"account_id\": 11,"
"    \"transaction_type\": \"Expense\","
"    \"date\": 1678559400,"
"    \"amount\": 7000.0,"
"    \"related_id\": 23,"
"    \"reference\": \"number\","
"    \"type\": 4,"
"    \"status\": 2,"
"    \"details\": ["
"     \"Advance Tax\""
"    ]"
"   },"
"   {"
"    \"id\": 4333,"
"    \"account_id\": 3,"
"    \"transaction_type\": \"Create WorkOrder\","
"    \"date\": 1697969157,"
"    \"amount\": 8934.1450000000004,"
"    \"related_id\": 1,"
"    \"reference\": \"number\","
"    \"type\": 31,"
"    \"status\": 2,"
"    \"details\": []"
"   },"
"   {"
"    \"id\": 4334,"
"    \"account_id\": 16,"
"    \"transaction_type\": \"Create WorkOrder\","
"    \"date\": 1697969157,"
"    \"amount\": -8934.1450000000004,"
"    \"related_id\": 1,"
"    \"reference\": \"number\","
"    \"type\": 31,"
"    \"status\": 2,"
"    \"details\": []"
"   }"
"  ],"
"  \"count\": 20"
" }"
"}"
 
  ;

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
        case REPORT_PAYMENTS_RECEIVED:
          if(process_payments_received(root, from_date, to_date, date_format, 
              company_name, currency_symbol, currency_format, fp)){
            error = true;
          } 
        case REPORT_SALES_REFUND_HISTORY:
          if(process_sales_refund_history(root, from_date, to_date, date_format, 
              company_name, currency_symbol, currency_format, fp)){
            error = true;
          } 
          case REPORT_ACCOUNT_TRANSACTIONS:
          if(process_account_transactions(root, from_date, to_date, date_format, 
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
