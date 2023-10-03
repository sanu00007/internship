#include "common.h"
#include "report_generate.h"
#include "utils.h"
#include "balance_sheet.h"
#include "profit_n_loss.h"
#include "wkhtmltox/pdf.h"

using namespace std;


enum {
  REPORT_PROFIT_N_LOSS = 1,
  REPORT_BALANCE_SHEET=2,
  REPORT_CASH_FLOW,
};

struct account_map{
  int id;
  char *name;
  account_map(int id, const char *name):id(id){
    name = strdup(name);
  }
  ~account_map(){
    free(name);
  }
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

bool parse_account_map(const json_t *root, vector<account_map*> *accounts){
  bool ret = false;
  do {
    json_t* json_map = json_object_get(root, "account_mapping");
    if(!json_map || !json_is_array(json_map)){
      ERROR_LOG("account mapping array not found..");
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
        account_map* accnt_map = new account_map(id, json_string_value(json_handle));
        accounts->push_back(accnt_map);
      }
    }
    ret = true;
  } while(0);
  return ret;
}

void handle_get_html(json_t *old_root, struct client *client){
 
  const char* my_input = 
  /*"{"
" \"basis\": \"cash\","
" \"date\": 1795839400,"
" \"method\": 138,"
" \"currency_symbol\": \"&#8377;\","
" \"report_type\": 2,"
" \"file_name\": \"balance_sheet.html\","
" \"date_format\": 1,"
" \"company_name\": \"Kesans\","
" \"currency_format\": {"
"  \"group\": 2,"
"  \"thousand_seperator\": 2,"
"  \"sub_unit_seperator\": 3,"
"  \"decimal_places\": 2"
" },"
" \"report\": {"
"  \"assets\": {"
"   \"current_assets\": {"
"    \"cash\": ["
"     {"
"      \"amount\": -44328.0,"
"      \"name\": \"Demo account\","
"      \"id\": 73"
"     },"
"     {"
"      \"amount\": -7504040.6875510002,"
"      \"name\": \"Petty Cash\","
"      \"id\": 13"
"     },"
"     {"
"      \"amount\": 31164404.480469,"
"      \"name\": \"Undeposited Funds\","
"      \"id\": 12"
"     }"
"    ],"
"    \"banks\": ["
"     {"
"      \"amount\": 35334.0,"
"      \"name\": \"Test account\","
"      \"id\": 71"
"     }"
"    ]"
"   },"
"   \"other_current_assets\": ["
"    {"
"     \"amount\": 4000.0,"
"     \"name\": \"Advance Tax\","
"     \"id\": 11"
"    },"
"    {"
"     \"amount\": 0.0,"
"     \"name\": \"Input Tax Credits\","
"     \"id\": 5,"
"     \"sub_accounts\": ["
"      {"
"       \"amount\": 305.80952500000001,"
"       \"name\": \"Input CGST\","
"       \"id\": 7"
"      },"
"      {"
"       \"amount\": 1158002.956092,"
"       \"name\": \"Input IGST\","
"       \"id\": 6"
"      },"
"      {"
"       \"amount\": 305.80952500000001,"
"       \"name\": \"Input SGST\","
"       \"id\": 8"
"      }"
"     ]"
"    },"
"    {"
"     \"amount\": 147226.36187399999,"
"     \"name\": \"Inventory Asset\","
"     \"id\": 16"
"    },"
"    {"
"     \"amount\": 113892.2,"
"     \"name\": \"Prepaid Expenses\","
"     \"id\": 10"
"    },"
"    {"
"     \"amount\": 1454.770538,"
"     \"name\": \"Reverse Charge Tax Input but not due\","
"     \"id\": 9"
"    },"
"    {"
"     \"amount\": 5478.1059070000001,"
"     \"name\": \"Sales To Customers (Cash)\","
"     \"id\": 64"
"    },"
"    {"
"     \"amount\": -1728.648236,"
"     \"name\": \"TCS Receivable\","
"     \"id\": 69"
"    },"
"    {"
"     \"amount\": -131434.584546,"
"     \"name\": \"TDS Receivable\","
"     \"id\": 67"
"    }"
"   ],"
"   \"fixed_assets\": ["
"    {"
"     \"amount\": -8988.0,"
"     \"name\": \"Furniture and Equipment\","
"     \"id\": 15"
"    }"
"   ]"
"  },"
"  \"liabilities_n_expenses\": {"
"   \"liabilities\": {"
"    \"current_liabilities\": ["
"     {"
"      \"amount\": 224295.0,"
"      \"name\": \"Opening Balance Adjustments\","
"      \"id\": 22"
"     },"
"     {"
"      \"amount\": 0.0,"
"      \"name\": \"GST Payable\","
"      \"id\": 18,"
"      \"sub_accounts\": ["
"       {"
"        \"amount\": 400.0,"
"        \"name\": \"Output Cess\","
"        \"id\": 62"
"       },"
"       {"
"        \"amount\": 1220.450713,"
"        \"name\": \"Output CGST\","
"        \"id\": 20"
"       },"
"       {"
"        \"amount\": 850.22425599999997,"
"        \"name\": \"Output IGST\","
"        \"id\": 19"
"       },"
"       {"
"        \"amount\": 1220.450713,"
"        \"name\": \"Output SGST\","
"        \"id\": 21"
"       }"
"      ]"
"     },"
"     {"
"      \"amount\": 1979.5697230000001,"
"      \"name\": \"TDS Payable\","
"      \"id\": 24"
"     },"
"     {"
"      \"amount\": -1200.0,"
"      \"name\": \"test card\","
"      \"id\": 75"
"     },"
"     {"
"      \"amount\": 402007.0,"
"      \"name\": \"Unearned Revenue\","
"      \"id\": 23"
"     }"
"    ],"
"    \"other_liabilities\": [],"
"    \"long_term_liabilities\": []"
"   },"
"   \"equities\": ["
"    {"
"     \"amount\": -0.0,"
"     \"name\": \"Current Year Earnings\""
"    },"
"    {"
"     \"amount\": -31100.0,"
"     \"name\": \"Drawings\","
"     \"id\": 28"
"    },"
"    {"
"     \"amount\": 24340211.878197011,"
"     \"name\": \"Retained Earnings\""
"    }"
"   ]"
"  },"
"  \"start_time\": 1775017000"
" }"
"}";*/
   "{"
" \"basis\": \"cash\","
" \"to_date\": 1795839400,"
" \"from_date\": 1795839400,"
" \"method\": 138,"
" \"currency_symbol\": \"&#8377;\","
" \"report_type\": 1,"
" \"file_name\": \"profit_n_loss.html\","
" \"date_format\": 1,"
" \"company_name\": \"Kesans\","
" \"currency_format\": {"
"  \"group\": 2,"
"  \"thousand_seperator\": 2,"
"  \"sub_unit_seperator\": 3,"
"  \"decimal_places\": 2"
" },"
" \"report\": {"
"  \"op_income\": ["
"   {"
"    \"amount\": -961757.14753999992,"
"    \"name\": \"Discount\","
"    \"id\": 38"
"   },"
"   {"
"    \"amount\": 15723.515109,"
"    \"name\": \"General Income\","
"    \"id\": 35"
"   },"
"   {"
"    \"amount\": 10116.0,"
"    \"name\": \"Interest Income\","
"    \"id\": 36"
"   },"
"   {"
"    \"amount\": -0.0,"
"    \"name\": \"Late Fee Income\","
"    \"id\": 37"
"   },"
"   {"
"    \"amount\": 22.479383000000009,"
"    \"name\": \"Other Charges\","
"    \"id\": 32"
"   },"
"   {"
"    \"amount\": 32193313.181402005,"
"    \"name\": \"Sales\","
"    \"id\": 34"
"   },"
"   {"
"    \"amount\": 86.355739999999997,"
"    \"name\": \"Shipping Charge\","
"    \"id\": 33"
"   }"
"  ],"
"  \"cog_expense\": ["
"   {"
"    \"amount\": 6551954.7714140005,"
"    \"name\": \"Cost of Goods Sold\","
"    \"id\": 59"
"   }"
"  ],"
"  \"op_expense\": ["
"   {"
"    \"amount\": 21428.571426000002,"
"    \"name\": \"Advertising And Marketing\","
"    \"id\": 56"
"   },"
"   {"
"    \"amount\": 798.0,"
"    \"name\": \"Automobile Expense\","
"    \"id\": 41"
"   },"
"   {"
"    \"amount\": 1000.0,"
"    \"name\": \"Bad Debt\","
"    \"id\": 46"
"   },"
"   {"
"    \"amount\": 1008.0,"
"    \"name\": \"Bank Fees and Charges\","
"    \"id\": 57"
"   },"
"   {"
"    \"amount\": 235546.0,"
"    \"name\": \"Consultant Expense\","
"    \"id\": 51"
"   },"
"   {"
"    \"amount\": 3952.3809510000001,"
"    \"name\": \"Credit Card Charges\","
"    \"id\": 58"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Depreciation Expense\","
"    \"id\": 50"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"IT and Internet Expenses\","
"    \"id\": 42"
"   },"
"   {"
"    \"amount\": -8000.0,"
"    \"name\": \"Janitorial Expense\","
"    \"id\": 44"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Lodging\","
"    \"id\": 61"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Meals and Entertainment\","
"    \"id\": 49"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Office Supplies\","
"    \"id\": 55"
"   },"
"   {"
"    \"amount\": 13.122742000000001,"
"    \"name\": \"Other Expenses\","
"    \"id\": 53"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Postage\","
"    \"id\": 45"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Printing and Stationery\","
"    \"id\": 47"
"   },"
"   {"
"    \"amount\": 322.0,"
"    \"name\": \"Purchase Discounts\","
"    \"id\": 66"
"   },"
"   {"
"    \"amount\": -3600.0,"
"    \"name\": \"Rent Expense\","
"    \"id\": 43"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Repairs and Maintenance\","
"    \"id\": 52"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Salaries and Employee Wages\","
"    \"id\": 48"
"   },"
"   {"
"    \"amount\": 26045.948105000003,"
"    \"name\": \"Tax Paid Expense\","
"    \"id\": 65"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Telephone Expense\","
"    \"id\": 40"
"   },"
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Travel Expense\","
"    \"id\": 39"
"   },"
"   {"
"    \"amount\": 86823.711259000003,"
"    \"name\": \"Uncategorized\","
"    \"id\": 54"
"   }"
"  ],"
"  \"other_income\": [],"
"  \"other_expense\": ["
"   {"
"    \"amount\": 0.0,"
"    \"name\": \"Exchange Gain or Loss\","
"    \"id\": 60"
"   }"
"  ]"
" }"
"}";
  json_error_t err;
  json_t *root = json_loads(my_input, 0, &err);
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

    CurrencyFormat *currency_format = get_currecny_format(root);
    
    json_handle = json_object_get(root, "report_type");
    bool error = false;
    if(json_handle && json_is_integer(json_handle)){
      int report_type = json_integer_value(json_handle);
      switch(report_type){
        /*case REPORT_BALANCE_SHEET:
          if(!process_balance_sheet(root, from_date, date_format, 
                company_name, currency_symbol, currency_format, fp)){
            error = true;
          }
          break;*/
        case REPORT_PROFIT_N_LOSS:
            if(!process_profit_n_loss(root, from_date,to_date,date_format, 
                company_name, currency_symbol, currency_format, fp)){
            error = true;
          }
          break;
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
  char *json_txt = json_dumps(root, 1);
  //printf("Got the json - %s\n", json_txt);
  free(json_txt);
  //write_response(success_json, client);
}

void handle_get_pdf(json_t *root, struct client *client){
  char *json_txt = json_dumps(root, 1);
  printf("Got the json (in pdf) - %s\n", json_txt);
  free(json_txt);
  write_response(success_json, client);
}

