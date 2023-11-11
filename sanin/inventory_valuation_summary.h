#ifndef _INVENTORY_VALUATION_SUMMARY_H
#define _INVENTORY_VALUATION_SUMMARY_H
#include "utils.h"
bool process_inventory_valuation_summary(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_INVENTORY_VALUATION_SUMMARY_H
