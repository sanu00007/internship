#ifndef _ACCOUNT_TRANSACTIONS_H_
#define _ACCOUNT_TRANSACTIONS_H_
#include "utils.h"
bool process_account_transactions(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_ACCOUNT_TRANSACTIONS_H_
