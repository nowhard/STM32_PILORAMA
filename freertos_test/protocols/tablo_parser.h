#ifndef TABLO_PARSER_H
#define TABLO_PARSER_H
#include <stdint.h>
#include "indicator.h"

#define SYM_TAB_LEN 31
enum
{
	IND_1=0,
	IND_2,
};
uint8_t str_to_ind(uint8_t ind_num,uint8_t *str);
#endif
