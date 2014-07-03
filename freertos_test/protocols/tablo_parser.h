#ifndef TABLO_PARSER_H
#define TABLO_PARSER_H
#include <stdint.h>
#include "indicator.h"

#define SYM_TAB_LEN 31


void tablo_proto_parser(uint8_t *proto_buf);//
uint8_t str_to_ind(struct indicator *ind,uint8_t *str);
void ln_to_ind(struct indicator *ind,uint8_t *buf, uint8_t len);//
void ln_redraw(struct indicator *ind,uint8_t inverse);//перерисуем графический индикатор

#define FRAME_MAX_LEN   150//
#define IND_OPEN    1//[
#define IND_CLOSE   0//]


#endif
