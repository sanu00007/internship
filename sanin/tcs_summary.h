#ifndef _TCS_SUMMARY_H_
#define _TCS_SUMMARY_H_
#include "utils.h"
bool process_tcs_summary(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format, FILE *fp);

#endif //_TCS_SUMMARY_H_
