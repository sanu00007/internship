#ifndef _PROFIT_N_LOSS_H_
#define _PROFIT_N_LOSS_H_
#include "utils.h"
bool process_profit_n_loss(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_PROFIT_AND_LOSS_H_
