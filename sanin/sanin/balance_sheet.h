#ifndef _BALANCE_SHEET_H_
#define _BALANCE_SHEET_H_
#include "utils.h"
bool process_balance_sheet(const json_t *root, int date,int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_BALANCE_SHEET_H_
