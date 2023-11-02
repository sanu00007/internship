#include "balance_sheet.h"
using namespace std;

static double get_amount_from_account(json_t* account){
  double ret = 0;
  json_t *json_handle = json_object_get(account, "amount");
  if(json_handle && json_is_number(json_handle)){
    ret = json_number_value(json_handle);
  }
  json_handle = json_object_get(account, "sub_accounts");
  if(json_handle && json_is_array(json_handle)){
    json_t *node;
    int index = 0;
    json_array_foreach(json_handle, index, node){
      if(json_is_object(node)){
        ret += get_amount_from_account(node);
      }
    }
  }
  return ret;
}

static void print_total_for_bl_section(const char* section_name, double total, 
    const char* currency_symbol, CurrencyFormat *currency_format, int level, FILE *fp){
  const char *first_part = "<tr class=\"mat-row\" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: ";
  fwrite(first_part, 1, strlen(first_part), fp);
  char tmp_buff[10];
  sprintf(tmp_buff, "%d", 25 * level);
  fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
  const char *second_part = "px;\"><span class=\"font-weight-600\"> Total for ";
  fwrite(second_part, 1, strlen(second_part), fp);
  if(section_name){
    fwrite(section_name, 1, strlen(section_name), fp);
  }
  const char *third_part = "</span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\">";
  fwrite(third_part, 1, strlen(third_part), fp);
  if(total < 0){
    fwrite("- ", 1, 2, fp);
    total = -total;
  }
  if(total){
    if(currency_symbol){
      fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_total = get_formatted_currency(currency_format, total);
    if(formatted_total){
      fwrite(formatted_total, 1, strlen(formatted_total), fp);
      free(formatted_total);
    }
  }
  const char *fourth_part = "</span></td></tr>";
  fwrite(fourth_part, 1, strlen(fourth_part), fp);
}

static double print_bl_account(json_t* node, int level, CurrencyFormat *currency_format, 
    const char *currency_symbol, FILE *fp) {
  const char *first_part = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: ";
  fwrite(first_part, 1, strlen(first_part), fp);
  char tmp_buff[10];
  sprintf(tmp_buff, "%d", 25 * level);
  fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
  const char *second_part = "px;\"><span class=\"app-link \">";
  fwrite(second_part, 1, strlen(second_part), fp);
  json_t *json_handle = json_object_get(node, "name");
  const char* name = NULL;
  if(json_handle && json_is_string(json_handle)){
    name = json_string_value(json_handle);
    fwrite(name, 1, strlen(name), fp);
  }
  const char *third_part = "</span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"app-link \">";
  fwrite(third_part, 1, strlen(third_part), fp);
  double value = 0;
  json_handle = json_object_get(node, "amount");
  if(json_handle && json_is_number(json_handle)){
    value = json_number_value(json_handle);
  }
  double ret = value; 
  if(value < 0){
    fwrite("- ", 1, 2, fp);
    value = -value;
  }
  if(value){
    if(currency_symbol){
      fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, value);
    if(formatted_value){
      fwrite(formatted_value, 1, strlen(formatted_value), fp);
      free(formatted_value);
    }
  }
  const char *last_part = "</span></td></tr>";
  fwrite(last_part, 1, strlen(last_part), fp);
  json_handle = json_object_get(node, "sub_accounts");
  if(json_handle && json_is_array(json_handle)){
    json_t *sub_node;
    int index = 0;
    double total = 0;
    json_array_foreach(json_handle, index, sub_node){
      if(json_is_object(sub_node)){
        total += print_bl_account(sub_node, level + 1, currency_format, currency_symbol, fp);
      }
    }
    ret += total;
    print_total_for_bl_section(name, total, currency_symbol, currency_format, level, fp);
  }
  return ret;
}

static double print_bl_accnts_from_array(json_t* arr, int level, CurrencyFormat *currency_format, 
  const char *currency_symbol, const char *section_name, FILE *fp){
  json_t *node;
  int index = 0;
  double total = 0;
  json_array_foreach(arr, index, node){
    if(json_is_object(node)){
      print_bl_account(node, level, currency_format, currency_symbol, fp);
    }
    total += get_amount_from_account(node);
  }
  print_total_for_bl_section(section_name, total, currency_symbol, currency_format, level, fp);
  return total;
} 

bool process_balance_sheet(const json_t *root, int date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp){
  bool ret = false;
  do{

    const char* first_part = "<html><head><style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;font-weight:500}.mat-cell{font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:25em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}</style></head><body><div ><div class=\"text-center\"><h2 class=\"mt-0 mb-0 text-muted\">";

    fwrite(first_part, 1, strlen(first_part), fp);
    if(company_name){
      fwrite(company_name, 1, strlen(company_name), fp);
    }

    const char* second_part = "</h2><h2 class=\"mt-0 mb-0\">Balance Sheet</h2><h4 class=\"text-muted mt-0 font-weight-normal\">";
    fwrite(second_part, 1, strlen(second_part), fp);
    char *str_date = get_formated_date(date, date_format);
    char tmp_buff[100];
    snprintf(tmp_buff, 99, "As of %s", str_date);
    free(str_date);
    fwrite(tmp_buff, 1, strlen(tmp_buff), fp);

    const char* third_part = "</h4></div><div class=\"report-holder\"><table class=\"mat-table mat-sort w-100\"><thead ><tr class=\"mat-header-row \"><th class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Account</th><th mat-header-cell=\"\" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Account Code</th><th class=\"mat-header-cell_total report_total\" style=\"top: 0px; z-index: 100;\">Total</th></tr></thead><tbody ><tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span class=\"font-weight-500 text-muted\"> ASSETS </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr><tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 25px;\"><span class=\"font-weight-500 text-muted\"> Current Assets </span></span></td><td class=\"mat-cell \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr><tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 50px;\"><span class=\"font-weight-500 text-muted\"> Cash </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";

    fwrite(third_part, 1, strlen(third_part), fp);

    json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
      ERROR_LOG("Unable to find \"report\" tag");
      break;
    }

    json_t* json_handle = json_object_get(report_handle, "assets");
    double total_assets = 0;
    if(json_handle && json_is_object(json_handle)){
      json_t* ca_handle = json_object_get(json_handle, "current_assets");
      if(ca_handle && json_is_object(ca_handle)){
        json_t* arr_handle = json_object_get(ca_handle, "cash");
        if(arr_handle && json_is_array(arr_handle)){
          //We don't need to write header for cash as it is already in third_part
          total_assets += print_bl_accnts_from_array(arr_handle, 3, currency_format, currency_symbol, "Cash", fp);
        }
        arr_handle = json_object_get(ca_handle, "banks");
        if(arr_handle && json_is_array(arr_handle)){
          const char *bank_head = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 50px;\"><span class=\"font-weight-500 text-muted\"> Bank </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
          
          fwrite(bank_head, 1, strlen(bank_head), fp);
          total_assets += print_bl_accnts_from_array(arr_handle, 3, currency_format, currency_symbol, "Bank", fp);
        }
      }
      ca_handle = json_object_get(json_handle, "other_current_assets");
      if(ca_handle && json_is_array(ca_handle)){
        const char *other_ca = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 50px;\"><span class=\"font-weight-500 text-muted\"> Other Current Assets </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
        
        fwrite(other_ca, 1, strlen(other_ca), fp);
        total_assets += print_bl_accnts_from_array(ca_handle, 3, currency_format, currency_symbol, "Other Current Assets", fp);
      }
      ca_handle = json_object_get(json_handle, "fixed_assets");
      if(ca_handle && json_is_array(ca_handle)){
        const char *fixed_assets = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 25px;\"><span class=\"font-weight-500 text-muted\"> Fixed Assets </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
        
        fwrite(fixed_assets, 1, strlen(fixed_assets), fp);
        total_assets += print_bl_accnts_from_array(ca_handle, 2, currency_format, currency_symbol, "Fixed Assets", fp);
      }
    }
    const char* ta_first_part = "<tr class=\"mat-row  \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 25px;\"><span class=\"font-weight-600\"> Total Assets </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\">";
    fwrite(ta_first_part, 1, strlen(ta_first_part), fp);
    if(total_assets < 0){
      fwrite("- ", 1, 2, fp);
      total_assets = -total_assets;
    }
    if(total_assets){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_assets = get_formatted_currency(currency_format, total_assets);
      if(formatted_total_assets){
        fwrite(formatted_total_assets, 1, strlen(formatted_total_assets), fp);
        free(formatted_total_assets);
      }
    }
    const char* last_part = "</span></td></tr>";
    fwrite(last_part, 1, strlen(last_part), fp);

    double total_liabilities_equities = 0;
    json_handle = json_object_get(report_handle, "liabilities_n_expenses");
    if(json_handle && json_is_object(json_handle)){
      json_t* ca_handle = json_object_get(json_handle, "liabilities");
      if(ca_handle && json_is_object(ca_handle)){
        json_t* arr_handle = json_object_get(ca_handle, "current_liabilities");
        if(arr_handle && json_is_array(arr_handle)){
          const char *current_liabilities = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 50px;\"><span class=\"font-weight-500 text-muted\"> Current Liablities </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
          fwrite(current_liabilities, 1, strlen(current_liabilities), fp);
          total_liabilities_equities += print_bl_accnts_from_array(arr_handle, 3, currency_format, currency_symbol, "Current Liabilities", fp);
        }

        arr_handle = json_object_get(ca_handle, "other_liabilities");
        if(arr_handle && json_is_array(arr_handle)){
          const char *other_liabilities = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 50px;\"><span class=\"font-weight-500 text-muted\"> Other Liablities </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
          fwrite(other_liabilities, 1, strlen(other_liabilities), fp);
          total_liabilities_equities += print_bl_accnts_from_array(arr_handle, 3, currency_format, currency_symbol, "Other Liabilities", fp);
        }

        arr_handle = json_object_get(ca_handle, "long_term_liabilities");
        if(arr_handle && json_is_array(arr_handle)){
          const char *long_term_liabilities = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 50px;\"><span class=\"font-weight-500 text-muted\"> Long Term Liablities </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
          fwrite(long_term_liabilities, 1, strlen(long_term_liabilities), fp);
          total_liabilities_equities += print_bl_accnts_from_array(arr_handle, 3, currency_format, currency_symbol, "Long Term Liabilities", fp);
        }
      }
      ca_handle = json_object_get(json_handle, "equities");
      if(ca_handle && json_is_array(ca_handle)){
        const char *equities = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 25px;\"><span class=\"font-weight-500 text-muted\"> Equities </span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\"> </span></td></tr>";
        
        fwrite(equities, 1, strlen(equities), fp);
        total_liabilities_equities += print_bl_accnts_from_array(ca_handle, 2, currency_format, currency_symbol, "Equities", fp);
      }
    }
    const char* tle_first_part = "<tr class=\"mat-row  \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: 25px;\"><span class=\"font-weight-600\"> Total Liabilities & Equities</span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"font-weight-600\">";
    fwrite(tle_first_part, 1, strlen(tle_first_part), fp);
    if(total_liabilities_equities < 0){
      fwrite("- ", 1, 2, fp);
      total_liabilities_equities = -total_liabilities_equities;
    }
    if(total_liabilities_equities){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_liabilities_equities = get_formatted_currency(currency_format, total_liabilities_equities);
      if(formatted_total_liabilities_equities){
        fwrite(formatted_total_liabilities_equities, 1, strlen(formatted_total_liabilities_equities), fp);
        free(formatted_total_liabilities_equities);
      }
    }
    fwrite(last_part, 1, strlen(last_part), fp);
    last_part = "</tbody></table> </div> </body> </html>";
    fwrite(last_part, 1, strlen(last_part), fp);
    //Do you stuff here.
    ret = true;
  } while(0);
  return ret;
}


