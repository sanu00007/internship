#include "utils.h"

CurrencyFormat* get_currecny_format(const json_t *root){
  CurrencyFormat* ret = new CurrencyFormat(); //Note that we anyway returns an object, with defaults.
  do{
    json_t* format_handle = json_object_get(root, "currency_format");
    if(!format_handle){
      break;
    }
    json_t* json_handle = json_object_get(format_handle, "group");
    if(json_handle && json_is_integer(json_handle)){
      ret->group = (CurrencyGroup)json_integer_value(json_handle);
    }

    json_handle = json_object_get(format_handle, "thousand_seperator");
    if(json_handle && json_is_integer(json_handle)){
      ret->thousand_seperator = (CurrencySeperator)json_integer_value(json_handle);
    }

    json_handle = json_object_get(format_handle, "sub_unit_seperator");
    if(json_handle && json_is_integer(json_handle)){
      ret->sub_unit_seperator = (CurrencySeperator)json_integer_value(json_handle);
    }

    json_handle = json_object_get(format_handle, "decimal_places");
    if(json_handle && json_is_integer(json_handle)){
      ret->decimal_places = json_integer_value(json_handle);
    }
  } while (0);
  return ret;
}

char* get_formatted_currency(CurrencyFormat *format, double value){
  char tmp_str[200];
  char *ret = NULL;
  if(format){
    memset(tmp_str, 0, sizeof(tmp_str));
    long long int_val = (long long) value;
    std::list<int> numbers;
    bool started = false;
    while(int_val){
      int tmp_val = 0;
      if(!started || format->group != CG_INDIAN){
        tmp_val = (int)(int_val % 1000);
        int_val = int_val / 1000;
        numbers.push_front(tmp_val);
      } else {
        tmp_val = (int)(int_val % 100);
        int_val = int_val / 100;
        numbers.push_front(tmp_val);
      }
      started = true;
    }
    started = false;
    const char* seperator = format->thousand_seperator == CS_COMMA ? "," : 
                            (format->thousand_seperator == CS_DOT ? "." : " ");
    char tmp_num[10];
    int len = numbers.size();
    for(auto number : numbers){
      if(started){
        strncat(tmp_str, seperator, 199);
        if(format->group != CG_INDIAN || len == 1){
          sprintf(tmp_num,"%03d",number);
        } else {
          sprintf(tmp_num,"%02d",number);
        }
      } else {
        sprintf(tmp_num,"%d",number);
      }
      len--;
      strncat(tmp_str, tmp_num, 199); 
      started = true;
    }
    if(!started){
      strcpy(tmp_str, "0");
    }
    if(format->decimal_places > 0){
      int decimal_val = pow(10,format->decimal_places);
      int_val = ((long long)(value * decimal_val)) % decimal_val;
      seperator = format->sub_unit_seperator == CS_COMMA ? "," : "." ;
      char tmp_formatter[10];
      sprintf(tmp_formatter, "%%0%dd", format->decimal_places);
      strncat(tmp_str, seperator, 199);
      sprintf(tmp_num, tmp_formatter, int_val); 
      strncat(tmp_str, tmp_num, 199);
    }
    ret = strdup(tmp_str);
  }
  return ret;
}

char* get_formated_date(time_t cur_time, int cur_format){
  if(!cur_time){
    return NULL;
  }
  char *ret = NULL;
  struct tm* tmp_date = localtime(&cur_time);
  char date[200];
  memset(date, 0 , sizeof(date));
  const char* format = NULL;

  switch(cur_format){
    case 1: format = "%m-%d-%y"; break;
    case 2: format = "%d-%m-%y"; break;
    case 3: format = "%y-%m-%d"; break;
    case 4: format = "%m-%d-%Y"; break;
    case 5: format = "%d-%m-%Y"; break;
    case 6: format = "%Y-%m-%d"; break;
    case 7: format = "%d %b %Y"; break;
    case 8: format = "%d %B %Y"; break;
    case 9: format = "%B %d, %Y"; break;
    case 10: format = "%a %B %d, %Y"; break;
    case 11: format = "%A %B %d, %Y"; break;
    default:;
  }
  if(format){
    strftime(date,200,format,tmp_date);
    ret = strdup(date);
  }
  return ret;
}

