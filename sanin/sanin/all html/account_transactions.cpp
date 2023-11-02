#include "account_transactions.h"
#include "report_generate.h"



double total_debit=0, total_credit=0,total_deb_cred_amount=0;



void print_number_at(double value, CurrencyFormat *currency_format, 
      const char* currency_symbol, FILE *fp){
  if(value < 0){
    fwrite("- ", 1, 2, fp);
    value = -value;
  }
  
  fprintf(fp, "%.2f", value);
}

static void print_at_account(json_t* node, CurrencyFormat *currency_format, const char *currency_symbol,
     FILE *fp, int date_format) {
  int date;
  json_t* json_handle;   
  const char *first_part = "<tr mat-row="" class=\"mat-row ng-star-inserted\" style=\"height:50px\"><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span style=\"margin-left:10px\">";   
  fwrite(first_part, 1, strlen(first_part), fp);
  json_handle = json_object_get(node, "date");
  if(json_handle && json_is_integer(json_handle)){
    date = json_integer_value(json_handle);
  }
  char *str_date = get_formated_date(date, date_format);
  char tmp_buff[100];
  snprintf(tmp_buff, 99, "%s", str_date);
  free(str_date);
  fwrite(tmp_buff, 1, strlen(tmp_buff), fp);



  const char* second_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"><span  class=\"app-link icon-hover\">";
  fwrite(second_part, 1, strlen(second_part), fp);
  
  //find account column
  

 
 
  const char* third_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(third_part, 1, strlen(third_part), fp);

  
  json_handle = json_object_get(node, "details");
  if(json_handle && json_is_array(json_handle)){
    size_t index;
    json_t* value;

    json_array_foreach(json_handle, index, value) {
        if (json_is_string(value)) {
            const char* detail = json_string_value(value);
            fwrite(detail, 1, strlen(detail), fp);
        }
    }  
  
  }
  



  const char* fourth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(fourth_part, 1, strlen(fourth_part), fp);
  json_handle = json_object_get(node, "transaction_type");
  if(json_handle && json_is_string(json_handle)){
    const char *transaction = json_string_value(json_handle);
    fwrite(transaction, 1, strlen(transaction), fp);
  }
  


  const char* fifth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(fifth_part, 1, strlen(fifth_part), fp);
  json_handle = json_object_get(node, "related_number");
  if(json_handle && json_is_string(json_handle)){
    const char *notes = json_string_value(json_handle);
    fwrite(notes, 1, strlen(notes), fp);
  }



  const char* sixth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">";
  fwrite(sixth_part, 1, strlen(sixth_part), fp);  
  json_handle = json_object_get(node, "reference");
  //reference?



  const char* seventh_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\"><span  class=\"app-link icon-hover\">";
  fwrite(seventh_part, 1, strlen(seventh_part), fp);
  double amount=0;
  json_handle = json_object_get(node, "amount");
  if(json_handle && json_is_number(json_handle)){
    amount=json_number_value(json_handle);
    total_deb_cred_amount+=amount;
  }
  
  
  if(amount>0){ //if debit
  total_debit+=amount;
  double debit=amount; 
  if(currency_symbol){
    fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
   }
  char *formatted_value = get_formatted_currency(currency_format, debit);
  if(formatted_value){
   fwrite(formatted_value, 1, strlen(formatted_value), fp);
   free(formatted_value);
   const char* eigth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\"><span  class=\"app-link icon-hover\">";
   fwrite(eigth_part, 1, strlen(eigth_part), fp);
   fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
   const char* last= "0.00";
   fwrite(last, 1, strlen(last), fp);
   
   }
  }
  
  
  else{//if credit
  total_credit+=amount;
  double credit=-amount;
  fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
  const char* eigth_part = "0.00</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\"><span  class=\"app-link icon-hover\">";
  fwrite(eigth_part, 1, strlen(eigth_part), fp);
  
  if(currency_symbol){
   fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
   }
  char *formatted_value = get_formatted_currency(currency_format, credit);
  if(formatted_value){
   fwrite(formatted_value, 1, strlen(formatted_value), fp);
   free(formatted_value);    
   }
  }
  
  
  const char* nineth_part = "</span></td><td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\">";
  fwrite(nineth_part, 1, strlen(nineth_part), fp);
  
  print_number_at(amount, currency_format, currency_symbol, fp);


  const char* last_part = "</span></td></tr>"; 
  fwrite(last_part, 1, strlen(last_part), fp);

}





static void print_at_accnts_from_array(json_t* arr,  CurrencyFormat *currency_format, 
    const char *currency_symbol,  
    FILE *fp, int date_format){
  json_t *node;
  int index = 0;
  json_array_foreach(arr, index, node){
    if(json_is_object(node)){
      print_at_account(node, currency_format, currency_symbol, 
                         fp,date_format);
    }
  } 
}



bool process_account_transactions(const json_t *root, int from_date,int to_date, int date_format, 
  const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp){
  
  bool ret=false;
  do{
  
  const char* first_part = "<style type=\"text/css\">.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;border-bottom:1px solid black}.label-cell{text-align:left}.mat-cell{border-bottom:solid 1px lightgray; font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:20em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right} .mat-header-row th {border-bottom: 1px solid #000;  }</style><div class=\"text-center\"> <h2 class=\"mt-0 mb-0 text-muted\">"; 
  fwrite(first_part, 1, strlen(first_part), fp);
  
  if(company_name){
      fwrite(company_name, 1, strlen(company_name), fp);
    }
    
    const char* second_part = "<h2  class=\"mt-0 mb-0\">Account Transactions</h2> <h3  class=\"mt-0 mb-0 ng-star-inserted\">Basis: Accural</h3> <h3  class=\"mt-0 mb-0 ng-star-inserted\"></h3><h4  class=\"text-muted mt-0 font-weight-normal\">";
    fwrite(second_part, 1, strlen(second_part), fp); 
    
    char *str_date1 = get_formated_date(from_date, date_format);
    char *str_date2= get_formated_date(to_date,date_format);
    char tmp_buff[100];
    snprintf(tmp_buff, 99, "%s to %s", str_date1,str_date2);
  
    fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
    
    const char* third_part =  "</h4></div><div class=\"report-holder wide-report-holder\"><table class=\"mat-table  mat-sort w-100\" role=\"table\" style=\"border-collapse: separate; border-spacing: 10px;\"><table  mat-table=\"\" matsort=\"\" class=\"mat-table  mat-sort w-100\"><thead \"> <tr mat-header-row=\"\" class=\"mat-header-row  ng-star-inserted\" style=\"\"> <th mat-header-cell=\"\" class=\"mat-header-cell\" style=\"height:50px; top: 0px; z-index: 100;\">Date</th><th mat-header-cell=\"\" class=\"mat-header-cell  \" style=\"top: 0px; z-index: 100;\">Account</th><th  mat-header-cell=\"\" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Transaction Details</th> <th mat-header-cell=\"\" class=\"mat-header-cell  p\" style=\"top: 0px; z-index: 100;\">Transaction Type</th> <th mat-header-cell=\"\" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Transaction#</th> <th style=\"text-align: left; mat-header-cell=\"\" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\">Reference#</th> <th  style=\"text-align: right; mat-header-cell=\"\" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Debit</th> <th style=\"text-align: right; mat-header-cell=\"\" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Credit</th><th  style=\"text-align: right; mat-header-cell=\"\" class=\"mat-header-cell \" style=\"top: 0px; z-index: 100;\">Amount</th></tr></thead>";
    fwrite(third_part, 1, strlen(third_part), fp);
  
   json_t* report_handle = json_object_get(root, "report"); 
    if(!report_handle || !json_is_object(report_handle)){
      ERROR_LOG("Unable to find \"report\" tag");
      break;
    }
    
    
    
    const char* fourth_part = "<tr class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"> <td class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\"></span>";
   fwrite(fourth_part, 1, strlen(fourth_part), fp);
   snprintf(tmp_buff, 99, "%s", str_date1);
   fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
   free(str_date1);
   
  const char* fourth_remaining = "</td><td  class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:left\">Opening Balance</td>  <td colspan=\"5\" class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\"><span class=\"font-weight-600\">";
    fwrite(fourth_remaining, 1, strlen(fourth_remaining), fp);
    json_t* json_handle = json_object_get(report_handle, "opening_balance");
    double open_bal=0;
    if(json_handle && json_is_number(json_handle)){
    open_bal = json_number_value(json_handle);
    print_number_at(open_bal, currency_format, currency_symbol, fp);
    }
    
    const char* fifth_part ="</span></td><td  class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\"><span class=\"font-weight-600\">";
    fwrite(fifth_part, 1, strlen(fifth_part), fp);
    json_handle = json_object_get(report_handle, "closing_balance");
    double close_bal=0;
    if(json_handle && json_is_number(json_handle)){
    close_bal = json_number_value(json_handle);
    print_number_at(close_bal, currency_format, currency_symbol, fp);
    }
    
    const char* sixth_part ="</span></td><td  class=\"mat-cell\" style=\"top: 0px; z-index: 100; text-align:right\"><span class=\"font-weight-600\">";
    fwrite(sixth_part, 1, strlen(sixth_part), fp);
    double initial_amount = open_bal-close_bal;
    print_number_at(initial_amount, currency_format, currency_symbol, fp);
    const char* last_part="</span></td></tr>";
    fwrite(last_part, 1, strlen(last_part), fp);
  
    
    json_t* arr_handle = json_object_get(report_handle, "transactions");
    if(arr_handle && json_is_array(arr_handle)){
         print_at_accnts_from_array( arr_handle,currency_format, 
                               currency_symbol, fp,date_format);
      }
  
  
  
  
   /*const char* fourth_part = "<tr mat-footer-row="" class=\"mat-footer-row cdk-footer-row \"> <td mat-footer-cell="" class=\"mat-footer-cell    \"><span class=\"font-weight-600 \"></span>";
   fwrite(fourth_part, 1, strlen(fourth_part), fp);
   snprintf(tmp_buff, 99, "%s", str_date2);
   fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
   free(str_date2);
   
  const char* fourth_remaining = "</td><td  mat-footer-cell="" class=\"mat-footer-cell    \">Closing Balance</td>  <td colspan=\"5\" style=\"text-align: right; mat-footer-cell="" class=\"mat-footer-cell  cdk-column-amount mat-column-amount \"><span class=\"font-weight-600\">";
    fwrite(fourth_remaining, 1, strlen(fourth_remaining), fp);
    print_number_at(total_debit, currency_format, currency_symbol, fp);
    
    
    const char* fifth_part ="</span></td><td style=\"text-align: right; mat-footer-cell="" class=\"mat-footer-cell  cdk-column-balance \"><span class=\"font-weight-600\">";
    fwrite(fifth_part, 1, strlen(fifth_part), fp);
    print_number_at(total_credit, currency_format, currency_symbol, fp);
    
    const char* sixth_part ="</span></td><td style=\"text-align: right; mat-footer-cell="" class=\"mat-footer-cell  cdk-column-balance \"><span class=\"font-weight-600\">";
    fwrite(sixth_part, 1, strlen(sixth_part), fp);
    print_number_at(total_deb_cred_amount, currency_format, currency_symbol, fp);
    
    const char* last_part="</span></td></tr>";
    fwrite(last_part, 1, strlen(last_part), fp);
  
    */
    
  ret=true;  
  }while(0);



}
