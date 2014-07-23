#ifndef TABLO_PARSER_H
#define TABLO_PARSER_H
#include <stdint.h>
#include "indicator.h"

#define SYM_TAB_LEN 31
uint8_t str_to_ind(struct indicator *ind,uint8_t *str);
#endif
