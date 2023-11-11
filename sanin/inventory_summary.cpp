#include "inventory_summary.h"


static void print_is_account(json_t* node, FILE *fp) {
  
  json_t* json_handle; 
  const char *first_part = "<tr mat-row="" class=\"mat-row ng-star-inserted\" style=\"height:50px\"><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span  class=\"app-link icon-hover\">";   
  fwrite(first_part, 1, strlen(first_part), fp);
 
  json_handle = json_object_get(node, "name");
  const char *name=NULL;
  if(json_handle && json_is_string(json_handle)){
    name = json_string_value(json_handle);
    fwrite(name, 1, strlen(name), fp);
  }
 

  const char* second_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(second_part, 1, strlen(second_part), fp);

  json_handle = json_object_get(node, "quantity_in");
  if(json_handle && json_is_number(json_handle)){
    int quantity_in = json_integer_value(json_handle);
    fprintf(fp, "%d", quantity_in);
  }

  const char* third_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(third_part, 1, strlen(third_part), fp);
  
  json_handle = json_object_get(node, "quantity_out");
  if(json_handle && json_is_number(json_handle)){
    int quantity_out = json_integer_value(json_handle);
    fprintf(fp, "%d", quantity_out);
  }
  
  const char* fourth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(fourth_part, 1, strlen(fourth_part), fp);
  
  json_handle = json_object_get(node, "quantity_out");
  if(json_handle && json_is_number(json_handle)){
    int quantity_out = json_integer_value(json_handle);
    fprintf(fp, "%d", quantity_out);
  }
  
  const char* fifth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(fifth_part, 1, strlen(fifth_part), fp);
  
  json_handle = json_object_get(node, "quantity_out");
  if(json_handle && json_is_number(json_handle)){
     int quantity_out = json_integer_value(json_handle);
    fprintf(fp, "%d", quantity_out);
  }
  
  const char* sixth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(sixth_part, 1, strlen(sixth_part), fp);
  
  json_handle = json_object_get(node, "quantity_out");
  if(json_handle && json_is_number(json_handle)){
     int quantity_out = json_integer_value(json_handle);
    fprintf(fp, "%d", quantity_out);
  }
   const char* seventh_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(seventh_part, 1, strlen(seventh_part), fp);
  
  json_handle = json_object_get(node, "quantity_out");
  if(json_handle && json_is_number(json_handle)){
     int quantity_out = json_integer_value(json_handle);
    fprintf(fp, "%d", quantity_out);
  }

  const char* last_part = "</span></td></tr>"; 
  fwrite(last_part, 1, strlen(last_part), fp);

}

static void print_is_accnts_from_array(json_t* arr,   
    FILE *fp){
  json_t *node;
  int index = 0;
  json_array_foreach(arr, index, node){
    if(json_is_object(node)){
      print_is_account(node, 
                         fp);
    }
  } 
}


bool process_inventory_summary(const json_t *root, int date, int date_format, 
  const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp){
  
  
  bool ret=false;
  
  
  do{
  
  const char* first_part = "<style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;border-bottom:1px solid black}.label-cell{text-align:left}.mat-cell{border-bottom:solid 1px lightgray; font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:20em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right} .mat-header-row th {border-bottom: 1px solid #000;  }</style><div class=\"text-center\"> <h2 class=\"mt-0 mb-0 text-muted\">"; 
  fwrite(first_part, 1, strlen(first_part), fp);
  
  if(company_name){
      fwrite(company_name, 1, strlen(company_name), fp);
    }
    
    const char* second_part = "</h2> <h2 class=\"mt-0 mb-0\">Inventory Summary</h2><h4 class=\"text-muted mt-0 font-weight-normal\">";
    fwrite(second_part, 1, strlen(second_part), fp);
    
    char *str_date1 = get_formated_date(date, date_format);
    
    char tmp_buff[100];
    snprintf(tmp_buff, 99, "As of %s", str_date1);
    free(str_date1);
    
    fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
    
    
    const char* third_part = "</h4></div><div class=\"report-holder wide-report-holder\"><table mat-table="" matsort="" class=\"mat-table cdk-table mat-sort w-100\"><thead> <tr mat-header-row="" class=\"mat-header-row cdk-header-row \"> <h4></h4></div><div class=\"report-holder wide-report-holder\"><table mat-table=\"\" matsort=\"\" class=\"mat-table cdk-table mat-sort w-100\"><thead> <tr mat-header-row=\"\" class=\"mat-header-row cdk-header-row \"> <th mat-header-cell=\"\" mat-sort-header=\"\" class=\"mat-sort-header mat-header-cell    \" aria-sort=\"none\" style=\"top: 0px; z-index: 100;\"><div class=\"mat-sort-header-container mat-focus-indicator \" aria-describedby=\"cdk-describedby-message-20\" cdk-describedby-host=\"0\" tabindex=\"0\" role=\"button\"> <div class=\"mat-sort-header-content \">Name</div><div class=\"mat-sort-header-arrow ng-trigger ng-trigger-arrowPosition  \" style=\"transform: translateY(25%); opacity: 0;\"> <div class=\"mat-sort-header-stem \"></div> <div class=\"mat-sort-header-indicator  ng-trigger ng-trigger-indicator\" style=\"transform: translateY(0px);\"><div class=\"mat-sort-header-pointer-left  ng-trigger ng-trigger-leftPointer\" style=\"transform: rotate(-45deg);\"></div> <div class=\"mat-sort-header-pointer-right  ng-trigger ng-trigger-rightPointer\" style=\"transform: rotate(45deg);\"></div> <div class=\"mat-sort-header-pointer-middle \"></div></div></div></div> </th> <th mat-header-cell=\"\" class=\"mat-header-cell   \" style=\"top: 0px; z-index: 100; \">Quantity IN</th> <th mat-header-cell=\"\" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\">Quantity OUT</th> <th mat-header-cell=\"\" class=\"mat-header-cell  \" style=\"top: 0px; z-index: 100;\">Current Stock</th> <th mat-header-cell=\"\" class=\"mat-header-cell  cdk-column-notes mat-column-notes\" style=\"top: 0px; z-index: 100;\">Ordered</th><th mat-header-cell=\"\" class=\"mat-header-cell  \" style=\"top: 0px; z-index: 100;\">Committed</th><th mat-header-cell=\"\" class=\"mat-header-cell  cdk-column-amount mat-column-amount\" style=\"top: 0px; z-index: 100;\">Available for Sale</th></tr></thead></style>";
    
    fwrite(third_part, 1, strlen(third_part), fp);
    
    json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
      ERROR_LOG("Unable to find \"report\" tag");
      break;
    }
    
    json_t * arr_handle = json_object_get(report_handle, "items");
      if(arr_handle && json_is_array(arr_handle)){
         print_is_accnts_from_array( arr_handle, fp);
        }
  
  ret=true;
  }while(0);
  
  return ret;
  }
