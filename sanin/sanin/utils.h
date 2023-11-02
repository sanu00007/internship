#ifndef _UTILS_H_
#define _UTILS_H_
#include "common.h"

enum CurrencyGroup{
  CG_DEFAULT =1,
  CG_INDIAN,
};

enum CurrencySeperator{
  CS_SPACE = 1,
  CS_COMMA,
  CS_DOT,
};

struct CurrencyFormat {
  int id;
  CurrencyGroup group;
  CurrencySeperator thousand_seperator;
  CurrencySeperator sub_unit_seperator;
  int decimal_places;
};

CurrencyFormat* get_currecny_format(const json_t *root);
char* get_formatted_currency(CurrencyFormat *format, double value);
char* get_formated_date(time_t cur_time, int cur_format);

#endif //_UTILS_H_ 
