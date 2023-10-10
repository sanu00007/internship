#include "tds_summary.h"
#include "report_generate.h"


static double tds=0,total=0,total_after_tds=0;

void print_number(double value, CurrencyFormat *currency_format, 
      const char* currency_symbol, FILE *fp){
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
}

double find_and_print_number(json_t* node, const char* name, CurrencyFormat *currency_format, 
      const char* currency_symbol, FILE *fp){
  double ret = 0;
  double value = 0;
  json_t *json_handle = json_object_get(node, name);
  if(json_handle && json_is_number(json_handle)){
    value = ret = json_number_value(json_handle);
  }
  print_number(value, currency_format, currency_symbol, fp);
  return ret;
}

static void print_tds_account(json_t* node,  CurrencyFormat *currency_format, bool is_by_vendor,
  const char *currency_symbol, vector<data_map *> *contact_mapping, FILE *fp) {
  const char *first_part = 
  "<tr mat-header-row="" class=\"mat-header-row \"><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span  class=\"app-link icon-hover\">"; 
  fwrite(first_part, 1, strlen(first_part), fp);

  json_t *json_handle;
  if(is_by_vendor){
    json_handle = json_object_get(node, "vendor_id");
    if(json_handle && json_is_integer(json_handle)){
      int vendor_id = json_integer_value(json_handle);
      for(auto contact : *contact_mapping){
        if(contact->id == vendor_id){
          if(contact->name){
            fwrite(contact->name, 1, strlen(contact->name), fp);
          }
          break;
        }
      }
    }
    const char *second_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\">";
    fwrite(second_part, 1, strlen(second_part), fp);
    json_handle = json_object_get(node, "pan");
    if(json_handle && json_is_string(json_handle)){
      const char *pan = json_string_value(json_handle);
      fwrite(pan, 1, strlen(pan), fp);

    }
    second_part = "</span></td>";
    fwrite(second_part, 1, strlen(second_part), fp);

  } else {
    fwrite("Section ", 1, strlen("Section "), fp);
    json_handle = json_object_get(node, "section");
    const char* name = NULL;
    if(json_handle && json_is_string(json_handle)){
      name = json_string_value(json_handle);
      fwrite(name, 1, strlen(name), fp);
    }
    const char *second_part = "</span></td>";
    fwrite(second_part, 1, strlen(second_part), fp);
    
  }
 
  const char *third_part = "<td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\">";
  fwrite(third_part, 1, strlen(third_part), fp);

  tds += find_and_print_number(node, "tds", currency_format, currency_symbol, fp);
  
  const char *forth_part = "</span></td><td  style=\"text-align: right; \"class=\"mat-cell\"><span  class=\"font-weight-500\">";
  fwrite(forth_part, 1, strlen(forth_part), fp);

  total += find_and_print_number(node, "total", currency_format, currency_symbol, fp);
  
  const char *fifth_part = "</span></td><td style=\"text-align: right\"; class=\"mat-cell\" style=\"padding: 25px;\"><span  class=\"font-weight-500\">";
  fwrite(fifth_part, 1, strlen(fifth_part), fp);
  
  total_after_tds += find_and_print_number(node, "total_after_tds", currency_format, currency_symbol, fp);
  
  const char *sixth_part = "</span></td></tr>";
  fwrite(sixth_part, 1, strlen(sixth_part), fp);

}

static void print_tds_accnts_from_array(json_t* arr,  CurrencyFormat *currency_format, 
  bool is_by_vendor, const char *currency_symbol, vector<data_map *> *contact_mapping, FILE *fp){
  json_t *node;
  int index = 0;
  json_array_foreach(arr, index, node){
    if(json_is_object(node)){
      print_tds_account(node, currency_format, is_by_vendor, currency_symbol, contact_mapping, fp);
    }
  } 
}

bool process_tds_summary(const json_t *root, int from_date,int to_date, int date_format, 
  const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp){
  bool ret=false;
  vector<data_map *> contact_mapping;
  do{
    const char* first_part="<style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;font-weight:500}.label-cell{text-align:left}.mat-cell{border-bottom:solid 1px lightgray; font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:20em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}</style><div ><div  class=\"text-center\"> <h2  class=\"mt-0 mb-0 text-muted\">";
  
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

    bool is_by_vendor = false;
    json_t* json_handle = json_object_get(root, "is_by_vendor"); 
    if(json_handle && json_is_boolean(json_handle)){
      is_by_vendor = json_boolean_value(json_handle);
    }
    
    const char* third_part=  "</h4></div><div class=\"report-holder wide-report-holder\"><table class=\"mat-table mat-sort w-100\" role=\"table\" style=\"border-collapse: separate; border-spacing: 0px 25px;\"><thead ><tr   mat-header-row="" class=\"mat-header-row \" style=""><th  mat-header-cell="" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;text-align:left; border-bottom:solid 1px gray;\">";
    fwrite(third_part, 1, strlen(third_part), fp);

    const char* type_name = is_by_vendor ? "Payee Name" : "TDS Section";
    fwrite(type_name, 1, strlen(type_name), fp);

    third_part = "</th>";
    fwrite(third_part, 1, strlen(third_part), fp);
    if(is_by_vendor){
      third_part = "<th  role=\"columnheader\" mat-header-cell="" class=\"mat-header-cell  \" style=\"border-bottom:solid 1px gray;top: 0px; z-index: 100;text-align:right\">PAN</th>";
      fwrite(third_part, 1, strlen(third_part), fp);
    }
    third_part = "<th  role=\"columnheader\" mat-header-cell="" class=\"mat-header-cell \" style=\"border-bottom:solid 1px gray;top: 0px; z-index: 100;text-align:right\">TDS</th><th  role=\"columnheader\" mat-header-cell="" class=\"mat-header-cell  \" style=\"border-bottom:solid 1px gray;top: 0px; z-index: 100;text-align:right\">Total</th><th mat-header-cell="" class=\"mat-header-cell  \" style=\"border-bottom:solid 1px gray;top: 0px; z-index: 100;text-align:right\">Total after TDS</th></tr></thead>";
    
    fwrite(third_part, 1, strlen(third_part), fp);
  
    json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
      ERROR_LOG("Unable to find \"report\" tag");
      break;
    }
    if(is_by_vendor){
      parse_data_map(root, &contact_mapping, "contact_mapping"); 
    }
    json_t* arr_handle = json_object_get(report_handle, "items");
    if(arr_handle && json_is_array(arr_handle)){
      print_tds_accnts_from_array( arr_handle,currency_format, is_by_vendor, 
                                    currency_symbol, &contact_mapping, fp);
    }
    
    const char* fourth_part="<tr mat-header-row="" class=\"mat-header-row \" style=""><th style=\"border-top:1px; border-bottom:1px;text-align:left\";  mat-header-cell="" class=\"mat-header-cell \"><span  class=\"ng-star-inserted\">Total</span></th>"; 
    fwrite(fourth_part, 1, strlen(fourth_part), fp); 

    if(is_by_vendor){
      fourth_part = "<td></td>";
      fwrite(fourth_part, 1, strlen(fourth_part), fp); 
    }

    fourth_part = "<td style=\"text-align: right\"; role=\"cell\" mat-footer-cell="" class=\"mat-footer-cell mat-column-tds ng-star-inserted\"><span  class=\"font-weight-600\">";
    fwrite(fourth_part, 1, strlen(fourth_part), fp); 
    print_number(tds, currency_format, currency_symbol, fp);
  
    const char* fifth_part="</span></td><td style=\"text-align: right\"; role=\"cell\" mat-footer-cell="" class=\"mat-footer-cell _total mat-column-tds_total ng-star-inserted\"><span  class=\"font-weight-600\">";
  
    fwrite(fifth_part, 1, strlen(fifth_part), fp); 
    print_number(total, currency_format, currency_symbol, fp);

    fwrite(fifth_part, 1, strlen(fifth_part), fp); 
    print_number(total_after_tds, currency_format, currency_symbol, fp);

    const char* sixth_part="</span></td></tr>";
    fwrite(sixth_part, 1, strlen(sixth_part), fp); 

    ret=true;
  } while(0);

  for(auto contact : contact_mapping){
    delete contact;
  }
  
  return ret;
}
