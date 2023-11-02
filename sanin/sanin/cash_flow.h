#ifndef _CASH_FLOW_H_
#define _CASH_FLOW_H_
#include "utils.h"
bool process_cash_flow(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_CASH_FLOW_H_
