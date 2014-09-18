#include "spi_bus.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"
#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tablo.h"
#include "tablo_parser.h"

#include "watchdog.h"
#include "indicator.h"
#include "buzzer.h"

extern struct tablo tab;
extern struct task_watch task_watches[];

static void spi_task(void *pvParameters);//
//static void spi2_task(void *pvParameters);
static void	spi1_config(void);
static void	spi2_config(void);

xSemaphoreHandle xSPI_Buf_Mutex;


uint8_t spi_buses_init(void)//
{
	spi1_config();
	spi2_config();
	xTaskCreate(spi_task,(signed char*)"SPI_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
//	xTaskCreate(spi2_task,(signed char*)"SPI_2_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
	task_watches[SPI_TASK_1].task_status=TASK_ACTIVE;
//	task_watches[SPI_TASK_2].task_status=TASK_ACTIVE;

	 xSPI_Buf_Mutex=xSemaphoreCreateMutex();
//	 xSPI2_Buf_Mutex=xSemaphoreCreateMutex();

	return 0;
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
	    SPI_Cmd(SPI1, ENABLE);
	    SPI1_GPIO_CS->BSRRH|=SPI1_CS1;//reset pin SPI1_CS1
}

void	spi2_config(void)//
{
	RCC_AHB1PeriphClockCmd(SPI2_GPIO_BUS|SPI2_CS_GPIO_BUS, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    /* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin   = SPI2_CS1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI2_GPIO_CS, &GPIO_InitStructure);

	GPIO_PinAFConfig(SPI2_GPIO, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(SPI2_GPIO, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(SPI2_GPIO, GPIO_PinSource15, GPIO_AF_SPI2);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    //SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_CalculateCRC(SPI2, DISABLE);
    SPI_Cmd(SPI2, ENABLE);

    //GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    SPI2_GPIO_CS->BSRRH|=SPI2_CS1;
}

//----------------------------------------------------------------
static void spi_task(void *pvParameters)//
{
	uint8_t i=0;
	//Indicator_Blink_Set(IND_1,0xFF,5);
	//Indicator_Blink_Set(IND_2,0xFF,2);
	//buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON);
	while(1)
	{
//		Indicator_Blink_Handler(BUS_SPI_1);

		//str_to_ind(IND_1,"Err0r");
		//str_to_ind(IND_2,"A-54");
		Indicator_Blink_Handler();

		if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
		{
			for(i=0;i<IND_COMMAND_LEN;i++)
			{
				 //-------------------------------------------
				 SPI1_GPIO_CS->BSRRH|=SPI1_CS1;//reset pin SPI1_CS1
				 SPI2_GPIO_CS->BSRRH|=SPI2_CS1;//reset pin SPI2_CS1

				 if(i>=5)//алгоритм мигания
				 {
					 if((tab.indicators[0].blink_mask_flags>>(i-5))&0x1)
					 {
						 SPI1->DR=(tab.buses[BUS_SPI_1].bus_buf[0][i]&0xFF00);
					 }
					 else
					 {
						 SPI1->DR=tab.buses[BUS_SPI_1].bus_buf[0][i];
					 }

					 if((tab.indicators[1].blink_mask_flags>>(i-5))&0x1)
					 {
						 SPI2->DR=(tab.buses[BUS_SPI_2].bus_buf[0][i]&0xFF00);
					 }
					 else
					 {
						 SPI2->DR=tab.buses[BUS_SPI_2].bus_buf[0][i];
					 }
				 }
				 else
				 {
					 //SPI_I2S_SendData(SPI1, tab.buses[BUS_SPI_1].bus_buf[0][i]);
					 SPI1->DR=tab.buses[BUS_SPI_1].bus_buf[0][i];
					 //SPI_I2S_SendData(SPI2, tab.buses[BUS_SPI_2].bus_buf[0][i]);
					 SPI2->DR=tab.buses[BUS_SPI_2].bus_buf[0][i];
				 }


				 while((SPI1->SR & SPI_SR_BSY)||(SPI2->SR & SPI_SR_BSY))
				 {
					taskYIELD();
				 }

				 taskYIELD();
				 SPI1_GPIO_CS->BSRRL|=SPI1_CS1;//set pin SPI1_CS1
				 SPI2_GPIO_CS->BSRRL|=SPI2_CS1;//set pin SPI1_CS1
				 taskYIELD();
				 SPI1_GPIO_CS->BSRRH|=SPI1_CS1;//reset pin SPI1_CS1
				 SPI2_GPIO_CS->BSRRH|=SPI2_CS1;//reset pin SPI2_CS1
			}
			xSemaphoreGive( xSPI_Buf_Mutex );
	    }

		task_watches[SPI_TASK_1].counter++;
		vTaskDelay(100);
	}
}

//-------------------------------------------------


//static void spi2_task(void *pvParameters)
//{
//	uint8_t i=0;
//
//	while(1)
//	{
////		Indicator_Blink_Handler(BUS_SPI_2);
////		str_to_ind(&tab.indicators[1],"-123");
//	//	str_to_ind(IND_2,"A-54");
//
//		if( xSemaphoreTake( xSPI2_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
//		{
//			for(i=0;i<IND_COMMAND_LEN;i++)
//			{
//				 SPI2_GPIO_CS->BSRRH|=SPI2_CS1;//reset pin SPI2_CS1
//			     SPI_I2S_SendData(SPI2, tab.buses[BUS_SPI_2].bus_buf[0][i]);
//				 while(SPI2->SR & SPI_SR_BSY)
//				 {
//					 taskYIELD();
//				 }
//
//				taskYIELD();
//				SPI2_GPIO_CS->BSRRL|=SPI2_CS1;//set pin SPI1_CS1
//				taskYIELD();
//				SPI2_GPIO_CS->BSRRH|=SPI2_CS1;//reset pin SPI2_CS1
//			}
//			xSemaphoreGive( xSPI2_Buf_Mutex );
//	    }
//
//		task_watches[SPI_TASK_2].counter++;
//		vTaskDelay(100);
//	}
//}

