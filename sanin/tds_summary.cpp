#include "tds_summary.h"
using namespace std;


double tds=0,total=0,total_after_tds=0;


static void print_bl_account(json_t* node,  CurrencyFormat *currency_format, 
    const char *currency_symbol, FILE *fp) {
  const char *second_part = 
  "<tr   mat-header-row="" class=\"mat-header-row \" style=""><th   mat-header-cell="" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\"><span  class=\"app-link icon-hover\">Section"; 
  fwrite(second_part, 1, strlen(second_part), fp);
 
  json_t *json_handle = json_object_get(node, "section");
  const char* name = NULL;
  if(json_handle && json_is_string(json_handle)){
  name = json_string_value(json_handle);
  fwrite(name, 1, strlen(name), fp);
  }
  const char *third_part = "</span></td><td style=\"text-align: center\"; role=\"columnheader\" mat-header-cell="" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\">";
  fwrite(third_part, 1, strlen(third_part), fp);
  double value1 = 0;
  json_handle = json_object_get(node, "tds");
  if(json_handle && json_is_number(json_handle)){
  value1 = json_number_value(json_handle);
  }
  tds+=value1;
  
  if(value1 < 0){
  fwrite("- ", 1, 2, fp);
  value1 = -value1;
  }
  if(value1){
    if(currency_symbol){
      fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, value1);
    if(formatted_value){
      fwrite(formatted_value, 1, strlen(formatted_value), fp);
      free(formatted_value);
    }
  }
  
  
  
  const char *forth_part = "</span></td><td  style=\"text-align: center\"class=\"mat-cell cdk-cell cdk-column-tds mat-column-tds ng-star-inserted\"><span  class=\"font-weight-500\">";
  fwrite(forth_part, 1, strlen(forth_part), fp);
  
  
  
    double value2 = 0;
  json_handle = json_object_get(node, "total");
  if(json_handle && json_is_number(json_handle)){
    value2 = json_number_value(json_handle);
  }
  total+=value2; 
  if(value2 < 0){
    fwrite("- ", 1, 2, fp);
    value2 = -value2;
  }
  if(value2){
    if(currency_symbol){
      fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, value2);
    if(formatted_value){
      fwrite(formatted_value, 1, strlen(formatted_value), fp);
      free(formatted_value);
    }
  }
  
  const char *fifth_part = "</span></td><td  style=\"text-align: center\";role=\"cell\" mat-cell="" class=\"mat-cell cdk-cell cdk-column-tds mat-column-tds ng-star-inserted\"><span  class=\"font-weight-500\">";
  fwrite(fifth_part, 1, strlen(fifth_part), fp);
  
    double value3 = 0;
  json_handle = json_object_get(node, "total_after_tds");
  if(json_handle && json_is_number(json_handle)){
    value3 = json_number_value(json_handle);
  }
   total_after_tds+=value3;
  if(value3 < 0){
    fwrite("- ", 1, 2, fp);
    value3 = -value3;
  }
  if(value3){
    if(currency_symbol){
      fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, value3);
    if(formatted_value){
      fwrite(formatted_value, 1, strlen(formatted_value), fp);
      free(formatted_value);
    }
  }
  
  const char *sixth_part = "</span></td></tr>";
  fwrite(sixth_part, 1, strlen(sixth_part), fp);

}



static void print_bl_accnts_from_array(json_t* arr,  CurrencyFormat *currency_format, 
  const char *currency_symbol, FILE *fp){
  json_t *node;
  int index = 0;
  
  json_array_foreach(arr, index, node){
    if(json_is_object(node)){
      print_bl_account(node, currency_format, currency_symbol, fp);
    }

    } 
}



bool process_tds_summary(const json_t *root, int from_date,int to_date, int date_format, 
  const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp)
  {
    bool ret=false;
    do{
  
  const char* first_part="<style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body  		[class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;font-weight:500}.mat-cell{font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:25em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}</style><div class=\"text-center\"><h2 class=\"mt-0 mb-0 text-muted\">";
  
    fwrite(first_part, 1, strlen(first_part), fp);
    if(company_name){
      fwrite(company_name, 1, strlen(company_name), fp);
    }
    
    const char* second_part= "</h2><h2 class=\"mt-0 mb-0\">TDS Summary</h2><h4 class=\"text-muted mt-0 font-weight-normal\">";
    fwrite(second_part, 1, strlen(second_part), fp);

    char *str_date1 = get_formated_date(from_date, date_format);
    char *str_date2= get_formated_date(to_date,date_format);
    char tmp_buff[100];
    snprintf(tmp_buff, 99, "%s to %s", str_date1,str_date2);
    free(str_date1);
    free(str_date2);
    fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
    
    const char* third_part= "</h4></div><div  class=\"report-holder wide-report-holder\"><table    class=\"mat-table cdk-table mat-sort w-100\" role=\"table\"><thead ><tr   mat-header-row="" class=\"mat-header-row \" style=""><th   mat-header-cell="" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\">TDS Section</th><th  role=\"columnheader\" mat-header-cell="" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\">TDS</th><th  role=\"columnheader\" mat-header-cell="" class=\"mat-header-cell  \" style=\"top: 0px; z-index: 100;\">Total</th><th  mat-header-cell="" class=\"mat-header-cell  \" style=\"top: 0px; z-index: 100;\">Total after TDS</th></tr></thead>";
    
    fwrite(third_part, 1, strlen(third_part), fp);
  
    json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
    ERROR_LOG("Unable to find \"report\" tag");
    break;
    }
    
    json_t* arr_handle = json_object_get(report_handle, "items");
    if(arr_handle && json_is_array(arr_handle)){
    print_bl_accnts_from_array( arr_handle,currency_format, currency_symbol, fp) ;        
    }
    
    const char* fourth_part="<tr   mat-header-row="" class=\"mat-header-row \" style=""><th   mat-header-cell="" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\"><span  class=\"ng-star-inserted\">Total</span></td><td style=\"text-align: center\"; role=\"cell\" mat-footer-cell="" class=\"mat-footer-cell cdk-footer-cell cdk-column-tds mat-column-tds ng-star-inserted\"><span  class=\"font-weight-600\">"; 
    fwrite(fourth_part, 1, strlen(fourth_part), fp); 
    if(tds < 0){
    fwrite("- ", 1, 2, fp);
    tds = -tds;
    }
    if(tds){
    if(currency_symbol){
    fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, tds);
    if(formatted_value){
    fwrite(formatted_value, 1, strlen(formatted_value), fp);
    free(formatted_value);
    }
  }
  
  const char* fifth_part="</span></td><td style=\"text-align: center\"; role=\"cell\" mat-footer-cell="" class=\"mat-footer-cell cdk-footer-cell cdk-column-tds_total mat-column-tds_total ng-star-inserted\"><span  class=\"font-weight-600\">";
  
   fwrite(fifth_part, 1, strlen(fifth_part), fp); 
   if(total < 0){
   fwrite("- ", 1, 2, fp);
   total = -total;
  }
   if(total){
    if(currency_symbol){
    fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, total);
    if(formatted_value){
    fwrite(formatted_value, 1, strlen(formatted_value), fp);
    free(formatted_value);
    }
  }
  
    fwrite(fifth_part, 1, strlen(fifth_part), fp); 
    if(total_after_tds < 0){
    fwrite("- ", 1, 2, fp);
    total_after_tds = -total_after_tds;
  }
     if(total_after_tds){
     if(currency_symbol){
     fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    }
    char *formatted_value = get_formatted_currency(currency_format, total_after_tds);
    if(formatted_value){
     fwrite(formatted_value, 1, strlen(formatted_value), fp);
     free(formatted_value);
    }
  }
  
   const char* sixth_part="</span></td></tr>";
   fwrite(sixth_part, 1, strlen(sixth_part), fp); 
  
   ret=true;
  }
   
   while(0);
  
  return ret;
  }
