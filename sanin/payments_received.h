#ifndef _PAYMENTS_RECEIVED_H_
#define _PAYMENTS_RECEIVED_H_
#include "utils.h"
bool process_payments_received(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_PAYMENTS_RECEIVED_H_
