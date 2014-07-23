#include "spi_bus.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tablo.h"
#include "tablo_parser.h"

#include "watchdog.h"
#include "indicator.h"

extern struct tablo tab;//
extern struct task_watch task_watches[];

static void spi1_task(void *pvParameters);//
static void spi2_task(void *pvParameters);
static void spi3_task(void *pvParameters);

xSemaphoreHandle xSPI_Buf_Mutex;


uint8_t spi_buses_init(void)//
{
	uint8_t error=0;

	if(tab.buses[BUS_SPI_1].error==BUS_ERROR_NONE)
	{
		xTaskCreate(spi1_task,(signed char*)"SPI_1_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
		task_watches[SPI_TASK_1].task_status=TASK_ACTIVE;
	}

	 xSPI_Buf_Mutex=xSemaphoreCreateMutex();

	 if( xSPI_Buf_Mutex != NULL )
	 {		//
	 }

	return error;
}


void	spi1_config(void)//
{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB |SPI1_GPIO_BUS, ENABLE);
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

	    GPIO_InitTypeDef GPIO_InitStructure;
	    SPI_InitTypeDef SPI_InitStructure;

	    /* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 |GPIO_Pin_4|GPIO_Pin_5;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	    GPIO_Init(GPIOB, &GPIO_InitStructure);


	    GPIO_InitStructure.GPIO_Pin   = SPI1_CS1;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	    //SPI_InitStructure.SPI_CRCPolynomial = 7;
	    SPI_Init(SPI1, &SPI_InitStructure);

	    /* Enable SPI1 */
	    SPI_CalculateCRC(SPI1, DISABLE);
	   // SPI_SSOutputCmd(SPI1, ENABLE);
	    SPI_Cmd(SPI1, ENABLE);

	    //GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
	    SPI1_GPIO->BSRRH|=SPI1_CS1;//reset pin SPI1_CS1
}



static void spi1_task(void *pvParameters)//
{
	uint8_t i=0;

	while(1)
	{
//		Indicator_Blink_Handler(BUS_SPI_1);

//		str_to_ind(&tab.indicators[0],"1.23");
		if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
		{
			for(i=0;i<IND_COMMAND_LEN;i++)
			{
				 SPI1_GPIO->BSRRH|=SPI1_CS1;//reset pin SPI1_CS1


					 SPI_I2S_SendData(SPI1, tab.buses[BUS_SPI_1].bus_buf[0][i]);
					 while(SPI1->SR & SPI_SR_BSY)
					 {
						 taskYIELD();
					 }


				taskYIELD();
				SPI1_GPIO->BSRRL|=SPI1_CS1;//set pin SPI1_CS1
				taskYIELD();
				SPI1_GPIO->BSRRH|=SPI1_CS1;//reset pin SPI1_CS1
			}
		xSemaphoreGive( xSPI_Buf_Mutex );
	    }

		task_watches[SPI_TASK_1].counter++;
		vTaskDelay(50);
	}
}


