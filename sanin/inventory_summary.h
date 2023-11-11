#ifndef _INVENTORY_SUMMARY_H_
#define _INVENTORY_SUMMARY_H_
#include "utils.h"
bool process_inventory_summary(const json_t *root, int date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_INVENTORY_SUMMARY_H_
