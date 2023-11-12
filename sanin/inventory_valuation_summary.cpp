#include "inventory_valuation_summary.h"
#include "report_generate.h"
#include <string>

double total_inventory_asset_value=0;
void print_number_ivs(double value, CurrencyFormat *currency_format, 
      const char* currency_symbol, FILE *fp){
  if(value < 0){
    fwrite("- ", 1, 2, fp);
    value = -value;
  }
  if(currency_symbol){
    fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
  }
  char *formatted_value = get_formatted_currency(currency_format, value);
  if(formatted_value){
    fwrite(formatted_value, 1, strlen(formatted_value), fp);
    free(formatted_value);
  }
}

static void print_ivs_account(json_t* node, CurrencyFormat *currency_format, const char *currency_symbol,
    vector<data_map *> *item_mapping, FILE *fp, int date_format, bool is_group) {
  
  json_t* json_handle; 
  const char *first_part = "<tr mat-row="" class=\"mat-row ng-star-inserted\" style=\"height:50px\"><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span  class=\"app-link icon-hover\">";   
  fwrite(first_part, 1, strlen(first_part), fp);
  
  json_handle = json_object_get(node, "item_id");
  if(json_handle && json_is_integer(json_handle)){
    int item_id = json_integer_value(json_handle);
    for(auto item : *item_mapping){
      if(item->id == item_id){
        if(item->name){
          fwrite(item->name, 1, strlen(item->name), fp);
        }
        break;
      }
    }
  }
  
  const char* second_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\">";
  fwrite(second_part, 1, strlen(second_part), fp);
  double quantity=0;
  json_handle = json_object_get(node, "quantity");
  if(json_handle && json_is_number(json_handle)){
    quantity=json_number_value(json_handle);
  }
  fprintf(fp, "%.1f", quantity);
  
   const char* eigth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\">";
  fwrite(eigth_part, 1, strlen(eigth_part), fp);

  double asset_value = 0;
  json_handle = json_object_get(node, "value");
  if(json_handle && json_is_number(json_handle)){
    asset_value=json_number_value(json_handle);
    total_inventory_asset_value+=asset_value;
  }
  print_number_ivs(asset_value, currency_format, currency_symbol, fp);

  const char* last_part = "</span></td></tr>"; 
  fwrite(last_part, 1, strlen(last_part), fp);
  	
  
  

}

static void print_ivs_accnts_from_array(json_t* arr,  CurrencyFormat *currency_format, 
    const char *currency_symbol, vector<data_map *> *item_mapping, 
    FILE *fp, int date_format, bool is_group){
  json_t *node;
  int index = 0;
  json_array_foreach(arr, index, node){
    if(json_is_object(node)){
      print_ivs_account(node, currency_format, currency_symbol, 
                        item_mapping, fp,date_format, is_group);
    }
  } 
}


bool process_inventory_valuation_summary(const json_t *root, int from_date,int to_date, int date_format, 
  const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp){
   bool ret=false;
  vector<data_map *> item_mapping;
  vector<data_map *> account_mapping;
  do{
  const char* first_part = "<style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;border-bottom:1px solid black}.label-cell{text-align:left}.mat-cell{border-bottom:solid 1px lightgray; font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:20em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right} .mat-header-row th {border-bottom: 1px solid #000;  }</style><div class=\"text-center\"> <h2 class=\"mt-0 mb-0 text-muted\">"; 
    
    fwrite(first_part, 1, strlen(first_part), fp);
    if(company_name){
      fwrite(company_name, 1, strlen(company_name), fp);
    }
    
    const char* second_part = "</h2> <h2 class=\"mt-0 mb-0\">Inventory Valuation Summary</h2><h4 class=\"text-muted mt-0 font-weight-normal\">";
    fwrite(second_part, 1, strlen(second_part), fp);
    
    char *str_date1 = get_formated_date(from_date, date_format);
    char *str_date2= get_formated_date(to_date,date_format);
    char tmp_buff[100];
    snprintf(tmp_buff, 99, "%s to %s", str_date1,str_date2);
    free(str_date1);
    free(str_date2);
    fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
    
    const char* third_part = "</h4></div><div class=\"report-holder wide-report-holder\"><table mat-table="" matsort="" class=\"mat-table cdk-table mat-sort w-100\"><thead> <tr mat-header-row="" class=\"mat-header-row cdk-header-row \"> <h4></h4></div><div class=\"report-holder wide-report-holder\"><table mat-table=\"\" matsort=\"\" class=\"mat-table cdk-table mat-sort w-100\"><thead> <tr mat-header-row=\"\" class=\"mat-header-row cdk-header-row \"> <th mat-header-cell=\"\" mat-sort-header=\"\" class=\"mat-sort-header mat-header-cell    \" aria-sort=\"none\" style=\"top: 0px; z-index: 100;\"><div class=\"mat-sort-header-container mat-focus-indicator \" aria-describedby=\"cdk-describedby-message-20\" cdk-describedby-host=\"0\" tabindex=\"0\" role=\"button\"> <div class=\"mat-sort-header-content \">Name</div><div class=\"mat-sort-header-arrow ng-trigger ng-trigger-arrowPosition  \" style=\"transform: translateY(25%); opacity: 0;\"> <div class=\"mat-sort-header-stem \"></div> <div class=\"mat-sort-header-indicator  ng-trigger ng-trigger-indicator\" style=\"transform: translateY(0px);\"><div class=\"mat-sort-header-pointer-left  ng-trigger ng-trigger-leftPointer\" style=\"transform: rotate(-45deg);\"></div> <div class=\"mat-sort-header-pointer-right  ng-trigger ng-trigger-rightPointer\" style=\"transform: rotate(45deg);\"></div> <div class=\"mat-sort-header-pointer-middle \"></div></div></div></div> </th> <th mat-header-cell=\"\" class=\"mat-header-cell  cdk-column-amount mat-column-amount\" style=\"top: 0px; z-index: 100;text-align:right;\">Quantity</th><th mat-header-cell=\"\" class=\"mat-header-cell  cdk-column-balance\" style=\"top: 0px; z-index: 100; text-align:right;\">Inventory Asset Value</th></tr></thead></style>";
    
    fwrite(third_part, 1, strlen(third_part), fp);
    
    json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
      ERROR_LOG("Unable to find \"report\" tag");
      break;
    }
    parse_data_map(root, &item_mapping, "item_mapping");
    
    json_t* group_handle = json_object_get(report_handle, "groups");
    if(group_handle && json_is_array(group_handle)){      
      json_t *node;
      int index = 0;
      json_array_foreach(group_handle, index, node){
    if(json_is_object(node)){
      json_t * field_handle = json_object_get(node, "field");
      json_t * accnt_handle = json_object_get(field_handle, "name");
      const char* field_name=NULL;
      if(accnt_handle && json_is_string(accnt_handle)){
         field_name=json_string_value(accnt_handle);          
          }          
      if (field_name && strcmp(field_name, "custom_label") == 0) {        
            json_t* arr_handle = json_object_get(node, "items");
            print_ivs_accnts_from_array(arr_handle, currency_format,
                                         currency_symbol, &item_mapping, fp, date_format, true);
        }
        
      else if(field_name && strcmp(field_name, "sales_account") == 0){                        
        json_t * arr_handle = json_object_get(node, "items");            
        parse_data_map(root, &account_mapping, "account_mapping");
        json_t* value_handle = json_object_get(field_handle,"value");
        int value=0;
        const char* val_str=json_string_value(value_handle);
      if(val_str && strcmp(val_str, "") == 0){
         const char* group_by ="<tr mat-row="" class=\"mat-row ng-star-inserted\" style=\"height:50px\"><td colspan=\"3\" class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span  class=\"app-link icon-hover\">>Sales Account: ";     
         fwrite(group_by, 1, strlen(group_by), fp); 
         print_ivs_accnts_from_array( arr_handle,currency_format, 
                                  currency_symbol, &item_mapping, fp,date_format, true);  
         }
         
     else{
     
         const char* group_by ="<tr mat-row="" class=\"mat-row ng-star-inserted\" style=\"height:50px\"><td colspan=\"3\" class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span  class=\"app-link icon-hover\">>Sales Account: [";     
         fwrite(group_by, 1, strlen(group_by), fp); 
         value = atoi(val_str);
         char * account_name=NULL ; 
         for(auto account : account_mapping){
           if(account->id == value){
            if(account->name){
             account_name = account->name;
             }
             break;
           }
         }
         fwrite(account_name, 1, strlen(account_name), fp);
         fprintf(fp, "]");
         print_ivs_accnts_from_array( arr_handle,currency_format, 
                                  currency_symbol, &item_mapping, fp,date_format, true); 
                                  
           }
         }                 
        }              
      }
    }
    
  
  
  const char* fourth_part = "<tr mat-footer-row="" class=\"mat-footer-row cdk-footer-row \"> <td mat-footer-cell="" class=\"mat-footer-cell    \"><span class=\"font-weight-600 \">Total</span></td>  <td mat-footer-cell="" class=\"mat-footer-cell   \"></td> <td style=\"top: 0px; z-index: 100; text-align:right\" mat-footer-cell="" class=\"mat-footer-cell  cdk-column-amount mat-column-amount \"><span class=\"font-weight-600\">";
    fwrite(fourth_part, 1, strlen(fourth_part), fp);
    print_number_ivs(total_inventory_asset_value, currency_format, currency_symbol, fp);  
    
    
    
    
  
  ret=true;
  }while(0);
  
  for(auto item : item_mapping){
    delete item;
  }
  for(auto account : account_mapping){
    delete account;
  }
  return ret;
  }
