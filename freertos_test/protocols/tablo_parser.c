#include "tablo_parser.h"
#include <stdio.h>
#include <string.h>
#include "tablo.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "spi_bus.h"

extern xSemaphoreHandle xSPI_Buf_Mutex;
//extern xSemaphoreHandle xSPI2_Buf_Mutex;

extern struct tablo tab;

const uint8_t Sym_table[2][SYM_TAB_LEN]={{'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F','h','I','i','J','L','O','P','r','t','U','u','.','-','_',' '},
                                         {0x7E/*0*/,0x30/*1*/,0x6D/*2*/,0x79/*3*/,0x33/*4*/,0x5B/*5*/,0x5F/*6*/,0x70/*7*/,0x7F/*8*/,0x7B/*9*/,0x77/*A*/,
                                          0x1F/*b*/,0x4E/*C*/,0x3D/*d*/,0x4F/*E*/,0x47/*F*/,0x17/*h*/,0x30/*I*/,0x10/*i*/,0x3C/*J*/,0xE/*L*/,0x7E/*O*/,
                                          0x67/*P*/,0x05/*r*/,0x0F/*t*/,0x3E/*U*/,0x1C/*u*/,0x80/*.*/,0x01/*-*/,0x08/*_*/,0x00/* */}};

extern struct tablo tab;

uint8_t str_to_ind(uint8_t ind_num,uint8_t *str)
{
    int8_t i=0,j=0;
    uint8_t buf_count=0;//
    int8_t str_len=0;
    struct indicator *ind;


    ind=&tab.indicators[ind_num];

    if(ind->number_in_bus>IND_ALL_NUM)//такого индикатора на шине нет
    {
    	return 0;
    }


    str_len=strlen(str);//!!!
		if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
		{
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][0]=ind->shutdown;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][1]=ind->display_test;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][2]=ind->scan_limit;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][3]=ind->brightness;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][4]=ind->decode_mode;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][5]=0x100;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][6]=0x200;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][7]=0x300;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][8]=0x400;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][9]=0x500;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][10]=0x600;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][11]=0x700;
			tab.buses[ind->bus].bus_buf[ind->number_in_bus][12]=0x800;

			buf_count+=5;

			if(str_len>10)//максимальная длина строки индикатора с точками
			{
				str_len=10;
			}

			for(i=0;i<str_len;i++)//
			{
				if((str[i]>=0x30)&&(str[i]<=0x39))//
				{
					uint8_t sym=Sym_table[1][(str[i]-0x30)];

					tab.buses[ind->bus].bus_buf[ind->number_in_bus][buf_count]|=(/*Sym_table[1][(str[i]-0x30)]*/sym)|(0x100*((buf_count-5)+1));
					buf_count++;

					continue;
				}

				if(str[i]=='.')
				{
					if(i<(str_len-1))
					{
						if(str[i+1]=='.')
						{
							tab.buses[ind->bus].bus_buf[ind->number_in_bus][buf_count]|=0x80;
							tab.buses[ind->bus].bus_buf[ind->number_in_bus][buf_count-1]|=0x80;
							buf_count++;
						}
						else
						{
							tab.buses[ind->bus].bus_buf[ind->number_in_bus][buf_count-1]|=0x80;
						}
					}
					else
					{
						tab.buses[ind->bus].bus_buf[ind->number_in_bus][buf_count-1]|=0x80;
					}
					continue;
				}

				for(j=10;j<SYM_TAB_LEN;j++)//
				{
				   if(str[i]==Sym_table[0][j])//
				   {
						uint8_t sym=Sym_table[1][j];//коррекция ошибки индикатора

						tab.buses[ind->bus].bus_buf[ind->number_in_bus][buf_count]|=(/*Sym_table[1][(str[i]-0x30)]*/sym)|(0x100*((buf_count-5)+1));
						buf_count++;

						break;
				   }
				}

				if(buf_count>(ind->character_num+5))//
				{
					break;
				}
			}
			xSemaphoreGive( xSPI_Buf_Mutex );
		}
        return buf_count;

}
