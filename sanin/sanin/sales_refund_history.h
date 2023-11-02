#ifndef _SALES_REFUND_HISTORY_H_
#define _SALES_REFUND_HISTORY_H_
#include "utils.h"
bool process_sales_refund_history(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_SALES_REFUND_HISTORY_H_
