#include "profit_n_loss.h"
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



static double print_bl_account(json_t* node, int level, CurrencyFormat *currency_format, 
    const char *currency_symbol, FILE *fp) {
  const char *first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""> <td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  					class=\"font-weight-600\" style=\"margin-left: ";
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
  const char *third_part = "<td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\"> ";
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
  if(value == 0){
    fwrite("0.00", 1, 4, fp);
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
  
  return total;
} 

bool process_profit_n_loss(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp)
{
   bool ret=false;
   do
   {
     const char* first_part= "<style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;font-weight:500}.mat-cell{font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:25em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}</style><div  class=\"text-center\"> <h2  class=\"mt-0 mb-0 text-muted\">";
     
     fwrite(first_part, 1, strlen(first_part), fp);
     if(company_name){
      fwrite(company_name, 1, strlen(company_name), fp);
    }
    
    const char* second_part="</h2><h2  class=\"mt-0 mb-0\">Profit and Loss</h2><h4  class=\"text-muted mt-0 font-weight-normal\">";
     fwrite(second_part, 1, strlen(second_part), fp);
     char *str_date = get_formated_date(date, date_format);
     char *str_date2= get_formated_date(to_date,date_format);
    char tmp_buff[100];
    snprintf(tmp_buff, 99, "%s to %s", str_date,str_date2);
    free(str_date);
    free(str_date2);
    fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
    
    const char* third_part= " </h4></div><div  class=\"report-holder wide-report-holder\"><table    class=\"mat-table cdk-table mat-sort w-100\"><thead ><tr    class=\"mat-header-row cdk-header-row ng-star-inserted\"><th    class=\"mat-header-cell cdk-header-cell cdk-column-account mat-column-account ng-star-inserted mat-table-sticky mat-table-sticky-border-elem-top\" style=\"top: 0px; z-index: 100;\">Account </th><th    class=\"mat-header-cell cdk-header-cell cdk-column-account_code mat-column-account_code ng-star-inserted mat-table-sticky mat-table-sticky-border-elem-top\" style=\"top: 0px; z-index: 100;\"></th> <th    class=\"mat-header-cell cdk-header-cell cdk-column-total mat-column-total ng-star-inserted mat-table-sticky mat-table-sticky-border-elem-top\" style=\"top: 0px; z-index: 100;\">Total</th></tr></thead><tbody ><tr    class=\"mat-row cdk-row hide-bottom-border ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-500 text-muted\" style=\"margin-left: 0px;\"> Operating Income </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\app-link icon-hover\"></span></td> </tr><tr    class=\"mat-row cdk-row hide-bottom-border ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"app-link icon-hover\" style=\"margin-left: 25px;\"> ";
     
     fwrite(third_part, 1, strlen(third_part), fp);
     
     json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
      ERROR_LOG("Unable to find \"report\" tag");
      break;
    }
    
    json_t* arr_handle = json_object_get(report_handle, "op_income");
    double total_op_income=0;
    
    if(arr_handle && json_is_array(arr_handle)){
          
          total_op_income += print_bl_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Operating Income", fp);
        }
    const char* to_first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-600\" style=\"margin-left: 0px;\"> Total Operating Expense </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(to_first_part, 1, strlen(to_first_part), fp);
    if(total_op_income < 0){
      fwrite("- ", 1, 2, fp);
      total_op_income = -total_op_income;
    }
    if(total_op_income){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_op_income = get_formatted_currency(currency_format, total_op_income);
      if(formatted_total_op_income){
        fwrite(formatted_total_op_income, 1, strlen(formatted_total_op_income), fp);
        free(formatted_total_op_income);
      }
    }
    const char* last_part = "</span></td></tr>";
    fwrite(last_part, 1, strlen(last_part), fp);    
    
    arr_handle = json_object_get(report_handle, "cog_expense"); 
    double total_cog_expense=0;
    
     if(arr_handle && json_is_array(arr_handle)){
          const char *cog = "<tr    class=\"mat-row cdk-row hide-bottom-border ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-500 text-muted\" style=\"margin-left: 0px;\"> Cost of Goods Sold </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\"></span></td></tr>";
        
        fwrite(cog, 1, strlen(cog), fp);
          total_cog_expense += print_bl_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Cost of Goods sold", fp);
        }
        
        const char* cog_first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""> <td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  					class=\"font-weight-600\" style=\"margin-left: 0px;\"> Total Cost of Goods Sold </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(cog_first_part, 1, strlen(cog_first_part), fp);
    if(total_cog_expense < 0){
      fwrite("- ", 1, 2, fp);
      total_cog_expense = -total_cog_expense;
    }
    if(total_cog_expense){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_cog_expense = get_formatted_currency(currency_format, total_cog_expense);
      if(formatted_total_cog_expense){
        fwrite(formatted_total_cog_expense, 1, strlen(formatted_total_cog_expense), fp);
        free(formatted_total_cog_expense);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp);
    
    double gross_profit=total_op_income-total_cog_expense;
    
    const char* gross_first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"app-link icon-hover\" style=\"margin-left: 0px;\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-600\"> Gross Profit </span></td> <td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(gross_first_part, 1, strlen(gross_first_part), fp);
    if(gross_profit < 0){
      fwrite("- ", 1, 2, fp);
      gross_profit = -gross_profit;
    }
    if(gross_profit){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_gross_profit = get_formatted_currency(currency_format, gross_profit);
      if(formatted_gross_profit){
        fwrite(formatted_gross_profit, 1, strlen(formatted_gross_profit), fp);
        free(formatted_gross_profit);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp);
    
    arr_handle = json_object_get(report_handle, "op_expense");
    double total_op_expense=0;
    
    if(arr_handle && json_is_array(arr_handle)){
          const char *exp ="  <tr    class=\"mat-row cdk-row hide-bottom-border ng-star-inserted\"><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-500 text-muted\" style=\"margin-left: 0px;\"> Operating Expense </span></td><td   class=mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\"></span></td> </tr>";       
        fwrite(exp, 1, strlen(exp), fp);
          total_op_expense += print_bl_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Operating Expense", fp);
        }
        
        
     const char* exp_first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-600\" style=\"margin-left: 0px;\"> Total Operating Expense </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\"> ";
    fwrite(exp_first_part, 1, strlen(exp_first_part), fp);
    if(total_op_expense < 0){
      fwrite("- ", 1, 2, fp);
      total_op_expense = -total_op_expense;
    }
    if(total_op_expense){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_op_expense = get_formatted_currency(currency_format, total_op_expense);
      if(formatted_total_op_expense){
        fwrite(formatted_total_op_expense, 1, strlen(formatted_total_op_expense), fp);
        free(formatted_total_op_expense);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp);
    
     
    double op_profit= gross_profit-total_op_expense; 
     
    const char* op_first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""> <td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"app-link icon-hover\" style=\"margin-left: 0px;\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-600\"> Operating Profit</span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(op_first_part, 1, strlen(op_first_part), fp);
    if(op_profit < 0){
      fwrite("- ", 1, 2, fp);
      op_profit = -op_profit;
    }
    if(op_profit){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_op_profit = get_formatted_currency(currency_format, op_profit);
      if(formatted_op_profit){
        fwrite(formatted_op_profit, 1, strlen(formatted_op_profit), fp);
        free(formatted_op_profit);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp); 
     
     
     arr_handle = json_object_get(report_handle, "other_income");
     
    double total_other_income=0;
    
    if(arr_handle && json_is_array(arr_handle)){
          const char *other_income ="<tr    class=\"mat-row cdk-row hide-bottom-border ng-star-inserted\" style=""> <td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-500 text-muted\" style=\"margin-left: 0px;\"> Non Operating Income </span><td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\"></span></td> </tr>";       
        fwrite(other_income, 1, strlen(other_income), fp);
          total_other_income+= print_bl_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Non Operating Income", fp);
        }
      
      
        const char* other_income_first_part ="<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-600\" style=\"margin-left: 0px;\"> Total Non Operating Income </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(other_income_first_part, 1, strlen(other_income_first_part), fp);
    if(total_other_income < 0){
      fwrite("- ", 1, 2, fp);
      total_other_income = -total_other_income;
    }
     if(total_other_income == 0){
    fwrite("0.00", 1, 4, fp);
    }
    if(total_other_income){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_other_income = get_formatted_currency(currency_format, total_other_income);
      if(formatted_total_other_income){
        fwrite(formatted_total_other_income, 1, strlen(formatted_total_other_income), fp);
        free(formatted_total_other_income);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp); 
        
        
        
        
     
      arr_handle = json_object_get(report_handle, "other_expense");
    double total_other_expense=0;
    
    if(arr_handle && json_is_array(arr_handle)){
          const char *other_expense ="<tr    class=\"mat-row cdk-row hide-bottom-border ng-star-inserted\" style=""> <td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-500 text-muted\" style=\"margin-left: 0px;\"> Non Operating Expense </span><td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\"></span></td> </tr>";       
        fwrite(other_expense, 1, strlen(other_expense), fp);
          total_other_expense+= print_bl_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Non Operating Expense", fp);
        }
     
           const char* other_expense_first_part ="<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""><td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"font-weight-600\" style=\"margin-left: 0px;\"> Total Non Operating Expense </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-500\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(other_expense_first_part, 1, strlen(other_expense_first_part), fp);
    if(total_other_expense == 0){
    fwrite("0.00", 1, 4, fp);
    }
    if(total_other_expense < 0){
      fwrite("- ", 1, 2, fp);
      total_other_expense = -total_other_expense;
    }
     
    
    if(total_other_expense){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_total_other_expense = get_formatted_currency(currency_format, total_other_expense);
      if(formatted_total_other_expense){
        fwrite(formatted_total_other_expense, 1, strlen(formatted_total_other_expense), fp);
        free(formatted_total_other_expense);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp); 
        
     
     double net_profit= op_profit+total_other_income-total_other_expense ; 
     
    const char* net_first_part = "<tr    class=\"mat-row cdk-row ng-star-inserted\" style=""> <td   class=\"mat-cell cdk-cell cdk-column-account mat-column-account ng-star-inserted\"><span  class=\"app-link icon-hover\" style=\"margin-left: 0px;\">  </span></td><td   class=\"mat-cell cdk-cell cdk-column-account_code mat-column-account_code ng-star-inserted\"><span  class=\"font-weight-600\"> Net Profit/Loss</span></td><td   class=\"mat-cell cdk-cell cdk-column-total mat-column-total ng-star-inserted\"><span  class=\"app-link icon-hover\">";
    fwrite(net_first_part, 1, strlen(net_first_part), fp);
    if(net_profit < 0){
      fwrite("- ", 1, 2, fp);
      net_profit = -net_profit;
    }
    if(net_profit){
      if(currency_symbol){
        fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      }
      char *formatted_net_profit = get_formatted_currency(currency_format, net_profit);
      if(formatted_net_profit){
        fwrite(formatted_net_profit, 1, strlen(formatted_net_profit), fp);
        free(formatted_net_profit);
      }
    }
 
    fwrite(last_part, 1, strlen(last_part), fp); 
     
     
     
     
 
   ret=true;
   }
   while(0);

  return ret;
}
