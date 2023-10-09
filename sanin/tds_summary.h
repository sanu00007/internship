#ifndef _TDS_SUMMARY_H_
#define _TDS_SUMMARY_H_
#include "utils.h"
bool process_tds_summary(const json_t *root, int date,int to_date, int date_format, 
    const char* company_name, const char* currency_symbol, CurrencyFormat *currency_format,bool vendor_value, FILE *fp);

#endif //_TDS_SUMMARY_H_
