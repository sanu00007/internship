#include "cash_flow.h"
using namespace std;
static double get_amount_from_account(json_t* account)
    {
    	double ret = 0;
    	json_t *json_handle = json_object_get(account, "amount");
    	if(json_handle && json_is_number(json_handle))
    	{
    	   ret = json_number_value(json_handle);
  	}
  	json_handle = json_object_get(account, "sub_accounts");
  	if(json_handle && json_is_array(json_handle))
  	{
    	   json_t *node;
    	   int index = 0;
    	   json_array_foreach(json_handle, index, node)
    	   {
      	      if(json_is_object(node))
      	      {
       	         ret += get_amount_from_account(node);
      	      }
    	   }
 	}
        return ret;
     }

static void print_total_for_cf_section(const char* section_name, double total, 
    const char* currency_symbol, CurrencyFormat *currency_format, int level, FILE *fp)
    {
  	const char *first_part = "<tr class=\"mat-row\" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: ";
  	fwrite(first_part, 1, strlen(first_part), fp);
  	char tmp_buff[10];
  	sprintf(tmp_buff, "%d", 25 * level);
  	fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
  	const char *second_part = "px;\"><span class=\"font-weight-600\"> Total for ";
  	fwrite(second_part, 1, strlen(second_part), fp);
  	if(section_name)
  	{
    	   fwrite(section_name, 1, strlen(section_name), fp);
  	}
  	const char *third_part = "</span></span></td><td class=\"mat-cell td.mat-cell \"><span class=\"font-weight-500\"></span></td><td class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\">";
  	fwrite(third_part, 1, strlen(third_part), fp);
  	if(total < 0)
  	{
    	   fwrite("- ", 1, 2, fp);
    	   total = -total;
        }
        if(total==0)
  	{
  	   fwrite("0.00", 1, 4, fp);
  	}
  	if(total)
  	{
    	   if(currency_symbol)
    	      {
      	      fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
              }
    	   char *formatted_total = get_formatted_currency(currency_format, total);
           if(formatted_total)
             {
             fwrite(formatted_total, 1, strlen(formatted_total), fp);
             free(formatted_total);
             }
        }
  	const char *fourth_part = "</span></td></tr>";
  	fwrite(fourth_part, 1, strlen(fourth_part), fp);
    }

static double print_cf_account(json_t* node, int level, CurrencyFormat *currency_format, 
    const char *currency_symbol, FILE *fp) 
    {
  	const char *first_part = "<tr class=\"mat-row hide-bottom-border \" ><td class=\"mat-cell  \"><span class=\"d-flex align-items-center\" style=\"margin-left: ";
  	fwrite(first_part, 1, strlen(first_part), fp);
  	char tmp_buff[10];
  	sprintf(tmp_buff, "%d", 25 * level);
  	fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
  	const char *second_part = "px;\"><span class=\"app-link \">";
  	fwrite(second_part, 1, strlen(second_part), fp);
  	json_t *json_handle = json_object_get(node, "name");
  	const char* name = NULL;
  	if(json_handle && json_is_string(json_handle))
  	{
    	   name = json_string_value(json_handle);
    	   fwrite(name, 1, strlen(name), fp);
  	}
  	const char *third_part = "</span></span></td><td class=\"mat-cell  \"><span class=\"font-weight-500\"></span></td><td class=\"mat-cell report_total \"><span class=\"app-link \">";
  	fwrite(third_part, 1, strlen(third_part), fp);
  	double value = 0;
  	json_handle = json_object_get(node, "amount");
  	if(json_handle && json_is_number(json_handle))
  	{
    	   value = json_number_value(json_handle);
  	}
  	double ret = value; 
  	if(value < 0)
  	{
    	   fwrite("- ", 1, 2, fp);
    	   value = -value;
  	}
  	if(value==0)
  	{
  	   fwrite("0.00", 1, 4, fp);
  	}
  	if(value)
  	{
    	   if(currency_symbol)
    	   {
      	       fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    	   }
           char *formatted_value = get_formatted_currency(currency_format, value);
     	   if(formatted_value)
     	   {
              fwrite(formatted_value, 1, strlen(formatted_value), fp);
      	      free(formatted_value);
           }
        }
  	const char *last_part = "</span></td></tr>";
  	fwrite(last_part, 1, strlen(last_part), fp);
  	json_handle = json_object_get(node, "sub_accounts");
  	if(json_handle && json_is_array(json_handle))
  	{
    	   json_t *sub_node;
    	   int index = 0;
    	   double total = 0;
    	   json_array_foreach(json_handle, index, sub_node)
    	   {
      	      if(json_is_object(sub_node))
      	      {
        	 total += print_cf_account(sub_node, level + 1, currency_format, currency_symbol, fp);
              }
           }
           ret += total;
           print_total_for_cf_section(name, total, currency_symbol, currency_format, level, fp);
        }
       return ret;
    }

static double print_cf_accnts_from_array(json_t* arr, int level, CurrencyFormat *currency_format, 
    const char *currency_symbol, const char *section_name, FILE *fp)
    {
  	json_t *node;
  	int index = 0;
  	double total = 0;
  	json_array_foreach(arr, index, node)
  	{
           if(json_is_object(node))
           {
      	      print_cf_account(node, level, currency_format, currency_symbol, fp);
           }
         total += get_amount_from_account(node);
        }
  	
  	return total;
    } 

bool process_cash_flow(const json_t *root, int from_date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp)
    {
       bool ret=false;
       do
        {
         
           const char* first_part= "<style type=\"text/css\"> .td.mat-cell[_ngcontent-wqh-c266], td.mat-footer-cell[_ngcontent-wqh-c266], th.mat-header-cell[_ngcontent-wqh-c266] {border-bottom-style: solid !important;}.report-header[]{display:flex;justify-content:center}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.report-actions-holder[]{display:flex;justify-content:space-between;padding:1em 1em 0}.app-link[]:hover{text-decoration:underline}.mat-column-balance[],.mat-column-cess[],.mat-column-cgst[],.mat-column-credit[],.mat-column-igst[],.mat-column-inter[],.mat-column-intra[],.mat-column-sgst[],.mat-column-tax[],.mat-column-total[],.report_total[]{text-align:end}.mat-column-pos[]{text-align:center}td.mat-column-desc[]{width:25%}.hide-bottom-border[]>td[]{border-bottom-width:0!important}.light-table-header[]{background-color:var(--app-secondary-color)}td.mat-column-nos[]{width:30%}tr.mat-row[]:hover{background:#f5f5f5}.mat-cell[],.mat-header-cell[]{min-width:175px!important}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}.mat-body-2{font:500 14px/24px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-body,.mat-body-1{font:400 14px/20px Roboto,Helvetica Neue,sans-serif;letter-spacing:normal}.mat-table{font-family:Roboto,Helvetica Neue,sans-serif}.mat-header-cell{font-size:12px;font-weight:500}.mat-cell{font-size:14px}:root{--app-background-dark-color:#e1e1e1;--app-foreground-color:rgba(0, 0, 0, .87);--app-background-color:#f5f5f599;--app-primary-color:#112b3d;--app-secondary-color:lavender;--app-secondary-light-color:#e6e6fa82;--app-link-color:#0b72c5;--app-card-color:white;--app-template-card-color:white;--app-muted-text-color:#616161;--app-skeleton-loader-color:#647e942e;--app-dark-color:black;--app-light-color:white;--app-text-primary:#0d6efd;--app-text-secondary:#6c757d;--app-text-success:#28a745;--app-text-info:#17a2b8;--app-light-blue:aliceblue;--app-form-label-color:#000000ad}:root .mat-table{background:#fff}:root .mat-table tbody,:root .mat-table thead,:root [mat-footer-row],:root [mat-header-row],:root [mat-row],:root mat-header-row,:root mat-row{background:inherit}.w-100{width:100%}.font-weight-normal{font-weight:400!important}.font-weight-500{font-weight:500!important}.font-weight-600{font-weight:600!important}.text-left{text-align:left!important}.text-center{text-align:center!important}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{box-sizing:border-box;flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.d-flex{display:flex!important}.align-items-center{align-items:center!important}body{background-color:var(--app-background-color)!important}body,html{height:100%;overscroll-behavior-y:contain}body{margin:0;font-family:Comfortaa,cursive;font-size:15px}body [class*=\" mat-\"],body [class^=mat-]{font-family:Comfortaa,cursive;font-size:15px}.text-body{color:#212529!important}.app-link{color:var(--app-link-color);font-weight:700}.text-normal{font-size:16px!important}.text-muted{color:var(--app-muted-text-color)!important;font-weight:500}.report_total{text-align:end}.report-holder{margin-top:2em!important;margin-bottom:2em!important;margin-left:20em!important;margin-right:25em!important}.hide{display:none!important}.header-holder{min-height:75px;padding-left:1em;padding-right:1em}.header-holder{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap}.mat-header-row,.mat-table{border-top-left-radius:20px;border-top-right-radius:20px}mat-table{display:block}mat-header-row{min-height:56px}mat-row{min-height:48px}mat-header-row,mat-row{display:flex;border-width:0;border-bottom-width:1px;border-style:solid;align-items:center;box-sizing:border-box}table.mat-table{border-spacing:0}tr.mat-header-row{height:56px}tr.mat-row{height:48px}th.mat-header-cell{text-align:left}[dir=rtl] th.mat-header-cell{text-align:right}</style><div  class=\"text-center\"><h2  class=\"mt-0 mb-0 text-muted\">"; 
                  
           fwrite(first_part, 1, strlen(first_part), fp);
     	   if(company_name)
     	   {
              fwrite(company_name, 1, strlen(company_name), fp);
    	   }
    	   
    	   const char* second_part= "</h2><h2  class=\"mt-0 mb-0\">Cash Flow Statement</h2><h4  class=\"text-muted mt-0 font-weight-normal\">";
    	   fwrite(second_part, 1, strlen(second_part), fp);
    	   
    	   char *str_date1 = get_formated_date(from_date, date_format);
    	   char *str_date2= get_formated_date(to_date,date_format);
   	   char tmp_buff[100];
  	   snprintf(tmp_buff, 99, "%s to %s", str_date1,str_date2);
   	   free(str_date1);
 	   free(str_date2);
  	   fwrite(tmp_buff, 1, strlen(tmp_buff), fp);
  	   
  	   
  	   const char* third_part= "</h4></div><div class=\"report-holder\"><table class=\"mat-table mat-sort w-100\"><thead ><tr class=\"mat-header-row \"><th class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Account</th><th mat-header-cell=\"\" class=\"mat-header-cell\" style=\"top: 0px; z-index: 100;\">Account Code</th><th class=\"mat-header-cell_total report_total\" style=\"top: 0px; z-index: 100;\">Total</th></tr></thead><tbody \"> <tr             class=\"mat-row\" > <td    class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-600\"> 1) Beginning Cash Balance </span></span></td>"; 
        
           fwrite(third_part, 1, strlen(third_part), fp);
       
           json_t* report_handle = json_object_get(root, "report"); 
    	   if(!report_handle || !json_is_object(report_handle))
    	   {
              ERROR_LOG("Unable to find \"report\" tag");
      	      break;
          
           }
           
           
           const char* beg_cash_part= "</span></span></td><td class=\"mat-cell td.mat-cell \"><span class=\"font-weight-500\"></span></td><td class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\">";
           fwrite(beg_cash_part, 1, strlen(beg_cash_part), fp);
           double beg_cash_balance=0;
           json_t* cash_handle= json_object_get(report_handle, "cash_balance");
           if(cash_handle && json_is_number(cash_handle))
           {
              beg_cash_balance = json_number_value(cash_handle);   
           }
           
           if(beg_cash_balance < 0)
           {
    	      fwrite("- ", 1, 2, fp);
    	      beg_cash_balance= -beg_cash_balance;
  	   }
  	   if(beg_cash_balance==0)
     	   { 
  	      fwrite("0.00", 1, 4, fp);
  	   }
  	   if(beg_cash_balance)
  	   {
    	      if(currency_symbol)
    	      {
      	          fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    	      }
              char *formatted_value = get_formatted_currency(currency_format, beg_cash_balance);
     	      if(formatted_value)
     	      {
                 fwrite(formatted_value, 1, strlen(formatted_value), fp);
      	         free(formatted_value);
              }
           }
           const char *last_part_beg = "</span></td></tr>";
  	   fwrite(last_part_beg, 1, strlen(last_part_beg), fp);
           
           const char *cash_flow= "<tr             class=\"mat-row \" ><td    class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-500 text-muted\"> A. Cash Flow from Operating Activities </span></span></td>";
           fwrite(cash_flow, 1, strlen(cash_flow), fp);
           
           double total_op_activities=0;
           json_t* arr_handle = json_object_get(report_handle, "operating_activities");
           if(arr_handle && json_is_array(arr_handle))
           {
              total_op_activities += print_cf_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Operating Activities", fp);
        
           }
           
           const char* net_cash_op_part= "   <trclass=\"mat-row  \" ><td style=\"padding: 10px; ; border-top: 1px solid #ccc;border-bottom: 1px solid #ccc;  font-size: 15px; font-weight: bold;\"   class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-600\"> Net cash provided by Operating Activities </span></span> </td><td style=\"padding: 10px; ; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; text-align: center; font-size: 15px; font-weight: bold;\"class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td><td style=\" text-align: right; padding: 10px; ; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; font-size: 15px; font-weight: bold;\" class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\"> "; 
          
           fwrite(net_cash_op_part, 1, strlen(net_cash_op_part), fp);
           if(total_op_activities < 0)
           {
    	      fwrite("- ", 1, 2, fp);
    	      total_op_activities= -total_op_activities;
  	   }
  	   if(total_op_activities==0)
     	   { 
  	      fwrite("0.00", 1, 4, fp);
  	   }
  	   if(total_op_activities)
  	   {
    	      if(currency_symbol)
    	      {
      	          fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    	      }
              char *formatted_value = get_formatted_currency(currency_format, total_op_activities);
     	      if(formatted_value)
     	      {
                 fwrite(formatted_value, 1, strlen(formatted_value), fp);
      	         free(formatted_value);
              }
           }
           
           
  	   const char *net_cash_last = "</span></td></tr>";
  	   fwrite(net_cash_last, 1, strlen(net_cash_last), fp);
           
           arr_handle= json_object_get(report_handle, "investing_activities");
           double total_investing_activities=0;
           if(arr_handle && json_is_array(arr_handle))
           {
              const char* inv_act_part = "<tr class=\"mat-row \" ><td    class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-500 text-muted\"> B. Cash Flow from Investing Activities </span></span></td> <td class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td> <td class=\"mat-cel\"> <span  class=\"font-weight-600  \">  </span></td></tr>";
              fwrite(inv_act_part, 1, strlen(inv_act_part), fp);  
              total_investing_activities += print_cf_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Investing Activities", fp);
           
           
           }
           
           const char* net_cash_inv_part= "   <trclass=\"mat-row  \" ><td style=\"padding: 10px; ; border-top: 1px solid #ccc;border-bottom: 1px solid #ccc;  font-size: 15px; font-weight: bold;\"   class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-600\"> Net cash provided by Investing Activities </span></span> </td><td style=\"padding: 10px; ; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; text-align: center; font-size: 15px; font-weight: bold;\"class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td><td style=\"padding: 10px; ; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; font-size: 15px; font-weight: bold;\" class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\"> "; 
          
           fwrite(net_cash_inv_part, 1, strlen(net_cash_inv_part), fp);
           if(total_investing_activities < 0)
           {
    	      fwrite("- ", 1, 2, fp);
    	      total_investing_activities= -total_investing_activities;
  	   }
  	   if(total_investing_activities==0)
     	   { 
  	      fwrite("0.00", 1, 4, fp);
  	   }
  	   if(total_investing_activities)
  	   {
    	      if(currency_symbol)
    	      {
      	          fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    	      }
              char *formatted_value = get_formatted_currency(currency_format, total_investing_activities);
     	      if(formatted_value)
     	      {
                 fwrite(formatted_value, 1, strlen(formatted_value), fp);
      	         free(formatted_value);
              }
           }
           
           fwrite(net_cash_last, 1, strlen(net_cash_last), fp);
  	   
           arr_handle= json_object_get(report_handle, "financing_activities");
           double total_financing_activities=0;
           if(arr_handle && json_is_array(arr_handle))
           {
              const char* fin_act_part= "<br></br><tr class=\"mat-row \" ><td    class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-500 text-muted\"> C. Cash Flow from Financing Activities </span></span></td> <td class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td> <td class=\"mat-cel\"> <span  class=\"font-weight-600  \">  </span></td></tr>";
              fwrite(fin_act_part, 1, strlen(fin_act_part), fp);  
              total_financing_activities += print_cf_accnts_from_array(arr_handle, 1, currency_format, currency_symbol, "Financing Activities", fp);
              
           
           }
           
           const char* net_cash_fin_part= "   <trclass=\"mat-row  \" ><td style=\"padding: 10px; ; border-top: 1px solid #ccc;border-bottom: 1px solid #ccc;  font-size: 15px; font-weight: bold;\"   class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"><span  class=\"font-weight-600\"> Net cash provided by Financing Activities </span></span> </td><td style=\"padding: 10px; ; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; text-align: center; font-size: 15px; font-weight: bold;\"class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td><td style=\"padding: 10px; ; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; font-size: 15px; font-weight: bold;\" class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\"> "; 
          
           fwrite(net_cash_fin_part, 1, strlen(net_cash_fin_part), fp);
           if(total_financing_activities < 0)
           {
    	      fwrite("- ", 1, 2, fp);
    	      total_financing_activities= -total_financing_activities;
  	   }
  	   if(total_financing_activities==0)
     	   { 
  	      fwrite("0.00", 1, 4, fp);
  	   }
  	   if(total_financing_activities)
  	   {
    	      if(currency_symbol)
    	      {
      	          fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
    	      }
              char *formatted_value = get_formatted_currency(currency_format, total_financing_activities);
     	      if(formatted_value)
     	      {
                 fwrite(formatted_value, 1, strlen(formatted_value), fp);
      	         free(formatted_value);
              }
           }
           
           fwrite(net_cash_last, 1, strlen(net_cash_last), fp);
           const char* net_change_part= " <tr class=\"mat-row \" > <td class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"> <span  class=\"font-weight-600\"> 2) Net Change in cash (A) + (B) + (C) </span></span></td><td class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td><td class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\">";
           fwrite(net_change_part, 1, strlen(net_change_part), fp);
           double net_change_cash=0;  
           net_change_cash= total_op_activities+ total_investing_activities+ total_financing_activities;
           if(net_change_cash < 0)
           {
      	      fwrite("- ", 1, 2, fp);
      	      net_change_cash = -net_change_cash;
    	   }
    	   if(net_change_cash)
    	   {
      	      if(currency_symbol)
      	      {
                 fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      	      }
      	      char *formatted_total_net_change_cash = get_formatted_currency(currency_format, net_change_cash);
      	      if(formatted_total_net_change_cash)
      	      {
                 fwrite(formatted_total_net_change_cash, 1, strlen(formatted_total_net_change_cash), fp);
                 free(formatted_total_net_change_cash);
      	      }
    	  }
          const char *net_part = "</span></td></tr>";
  	  fwrite(net_part, 1, strlen(net_part), fp);


          const char *end_cash_part= "<tr class=\"mat-row \" > <td  class=\"mat-cell \"><span  class=\"d-flex align-items-center\" style=\"margin-left: 0px;\"> <span  class=\"font-weight-600\">  Ending Cash Balance = 1 - 2</span></span></td><td class=\"mat-cell\"><span  class=\"font-weight-500\"></span></td><td class=\"mat-celltd.mat-cell report_total \"><span class=\"font-weight-600\">";
           fwrite(end_cash_part, 1, strlen(end_cash_part), fp);
             
           double end_cash_balance=0;  
           end_cash_balance= net_change_cash-beg_cash_balance;
           if(end_cash_balance < 0)
           {
      	      fwrite("- ", 1, 2, fp);
      	      end_cash_balance = -end_cash_balance;
    	   }
    	   if(end_cash_balance)
    	   {
      	      if(currency_symbol)
      	      {
                 fwrite(currency_symbol, 1, strlen(currency_symbol), fp);
      	      }
      	      char* formatted_total_end_cash_balance = get_formatted_currency(currency_format, end_cash_balance);
      	      if(formatted_total_end_cash_balance)
      	      {
                 fwrite(formatted_total_end_cash_balance, 1, strlen(formatted_total_end_cash_balance), fp);
                 free(formatted_total_end_cash_balance);
      	      }
    	   }
          
  	   fwrite(net_part, 1, strlen(net_part), fp);
           
           
           ret=true;
       
           }
       
         while(0);
       
         return ret;
     }
