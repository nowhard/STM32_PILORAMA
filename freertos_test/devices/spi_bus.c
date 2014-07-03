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

#include "watchdog.h"
#include "indicator.h"

extern struct tablo tab;//
extern struct task_watch task_watches[];

static void spi1_task(void *pvParameters);//
static void spi2_task(void *pvParameters);
static void spi3_task(void *pvParameters);

xSemaphoreHandle xSPI_Buf_Mutex;

#define IGREEN  0xAA
#define IRED	0x55
#define IORANGE	0xFF

#define COLOR	IORANGE
//
//uint16_t ind_test_buf3[13][8]=   {{0xAFF,0xAFF,0xAFF,0xAFF,0xAFF,0xAFF},
//								 {0x900,0x900,0x900,0x900,0x900,0x900},
//								 {0xB07,0xB07,0xB02,0xB02,0xB02,0xB02},
//								 {0xC01,0xC01,0xC01,0xC01,0xC01,0xC01},
//								 {0xF00,0xF00,0xF00,0xF00,0xF00,0xF00},
//								 {0x1*0x100+COLOR,0x1*0x100+COLOR,0x1FF,0x1FF,0x1FF,0x1FF},
//								 {0x2*0x100+COLOR,0x2*0x100+COLOR,0x2FF,0x2FF,0x2FF,0x2FF},
//								 {0x3*0x100+COLOR,0x3*0x100+COLOR,0x3FF,0x3FF,0x3FF,0x3FF},
//								 {0x4*0x100+COLOR,0x4*0x100+COLOR,0x4FF,0x4FF,0x4FF,0x4FF},
//								 {0x5*0x100+COLOR,0x5*0x100+COLOR,0x5FF,0x5FF,0x5FF,0x5FF},
//								 {0x6*0x100+COLOR,0x6*0x100+COLOR,0x6FF,0x6FF,0x6FF,0x6FF},
//								 {0x7*0x100+COLOR,0x7*0x100+COLOR,0x7FF,0x7FF,0x7FF,0x7FF},
//								 {0x8*0x100+COLOR,0x8*0x100+COLOR,0x8FF,0x8FF,0x8FF,0x8FF},
//								 };
//
//uint16_t ind_test_buf2[13][8]=   {{0xAFF,0xAFF,0xAFF,0xAFF,0xAFF,0xAFF},
//								 {0x900,0x900,0x900,0x900,0x900,0x900},
//								 {0xB02,0xB03,0xB07,0xB07,0xB02,0xB02},
//								 {0xC01,0xC01,0xC01,0xC01,0xC01,0xC01},
//								 {0xF00,0xF00,0xF00,0xF00,0xF00,0xF00},
//								 {0x1FF,0x1FF,0x1*0x100+COLOR,0x1*0x100+COLOR,0x1FF,0x1FF},
//								 {0x2FF,0x2FF,0x2*0x100+COLOR,0x2*0x100+COLOR,0x2FF,0x2FF},
//								 {0x3FF,0x3FF,0x3*0x100+COLOR,0x3*0x100+COLOR,0x3FF,0x3FF},
//								 {0x4FF,0x4FF,0x4*0x100+COLOR,0x4*0x100+COLOR,0x4FF,0x4FF},
//								 {0x5FF,0x5FF,0x5*0x100+COLOR,0x5*0x100+COLOR,0x5FF,0x5FF},
//								 {0x6FF,0x6FF,0x6*0x100+COLOR,0x6*0x100+COLOR,0x6FF,0x6FF},
//								 {0x7FF,0x7FF,0x7*0x100+COLOR,0x7*0x100+COLOR,0x7FF,0x7FF},
//								 {0x8FF,0x8FF,0x8*0x100+COLOR,0x8*0x100+COLOR,0x8FF,0x8FF},
//								 };
//
//uint16_t ind_test_buf1[13][8]=   {{0xAFF,0xAFF,0xAFF,0xAFF,0xAFF,0xAFF},
//								 {0x900,0x900,0x900,0x900,0x900,0x900},
//								 {0xB07,0xB02,0xB03,0xB04,0xB02,0xB07},
//								 {0xC01,0xC01,0xC01,0xC01,0xC01,0xC01},
//								 {0xF00,0xF00,0xF00,0xF00,0xF00,0xF00},
//								 {0x1*0x100+COLOR,0x1FF,0x1FF,0x1FF,0x1FF,0x1*0x100+COLOR},
//								 {0x2*0x100+COLOR,0x2FF,0x2FF,0x2FF,0x2FF,0x2*0x100+COLOR},
//								 {0x3*0x100+COLOR,0x3FF,0x3FF,0x3FF,0x3FF,0x3*0x100+COLOR},
//								 {0x4*0x100+COLOR,0x4FF,0x4FF,0x4FF,0x4FF,0x4*0x100+COLOR},
//								 {0x5*0x100+COLOR,0x5FF,0x5FF,0x5FF,0x5FF,0x5*0x100+COLOR},
//								 {0x6*0x100+COLOR,0x6FF,0x6FF,0x6FF,0x6FF,0x6*0x100+COLOR},
//								 {0x7*0x100+COLOR,0x7FF,0x7FF,0x7FF,0x7FF,0x7*0x100+COLOR},
//								 {0x8*0x100+COLOR,0x8FF,0x8FF,0x8FF,0x8FF,0x8*0x100+COLOR},
//								 };

uint8_t spi_buses_init(void)//
{
	uint8_t error=0;

	if(tab.buses[BUS_SPI_1].error==BUS_ERROR_NONE)
	{
		xTaskCreate(spi1_task,(signed char*)"SPI_1_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
		task_watches[SPI_TASK_1].task_status=TASK_ACTIVE;
	}

	if(tab.buses[BUS_SPI_2].error==BUS_ERROR_NONE)
	{
		xTaskCreate(spi2_task,(signed char*)"SPI_2_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
		task_watches[SPI_TASK_2].task_status=TASK_ACTIVE;
	}

	if(tab.buses[BUS_SPI_3].error==BUS_ERROR_NONE)
	{
		xTaskCreate(spi3_task,(signed char*)"SPI_3_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
		task_watches[SPI_TASK_3].task_status=TASK_ACTIVE;
	}

	 xSPI_Buf_Mutex=xSemaphoreCreateMutex();

	 if( xSPI_Buf_Mutex != NULL )
	 {		//
	 }

	return error;
}


void	spi1_config(void)//
{
	    if(tab.buses[BUS_SPI_1].error)//îøèáêà øèíû
	    {
	    	return;
	    }

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOC, ENABLE);
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

	    GPIO_InitTypeDef GPIO_InitStructure;
	    SPI_InitTypeDef SPI_InitStructure;

	    /* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 |GPIO_Pin_6|GPIO_Pin_7;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	    GPIO_Init(GPIOA, &GPIO_InitStructure);


	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

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

	    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
	//--------------------------------------------------------------------------
		DMA_InitTypeDef DMA_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
//---------------------------
		DMA_StructInit(&DMA_InitStructure);//mosi

		DMA_DeInit(DMA2_Stream3);

		DMA_InitStructure.DMA_Channel = DMA_Channel_3;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPI1->DR)) ;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
		DMA_Init(DMA2_Stream3,&DMA_InitStructure);
//-----------------------------
		// Enable DMA request
		DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);

		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
//		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
}

void	spi2_config(void)//
{
    if(tab.buses[BUS_SPI_2].error)//îøèáêà øèíû
    {
    	return;
    }

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    /* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    //SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);


    /* Enable SPI2 */
    SPI_CalculateCRC(SPI2, DISABLE);
    SPI_Cmd(SPI2, ENABLE);

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
//--------------------------------------------------------------------------
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_StructInit(&DMA_InitStructure);
	DMA_DeInit(DMA1_Stream4);

	//-----------------------------
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPI2->DR)) ;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
	DMA_Init(DMA1_Stream4,&DMA_InitStructure);


	DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
//	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
}

void 	spi3_config(void)//
{
    if(tab.buses[BUS_SPI_3].error)//îøèáêà øèíû
    {
    	return;
    }

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    /* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    //SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI3, &SPI_InitStructure);


    /* Enable SPI2 */
    SPI_CalculateCRC(SPI3, DISABLE);
    SPI_Cmd(SPI3, ENABLE);

    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
//--------------------------------------------------------------------------
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_StructInit(&DMA_InitStructure);
	DMA_DeInit(DMA1_Stream5);

	//-----------------------------
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPI3->DR)) ;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
	DMA_Init(DMA1_Stream5,&DMA_InitStructure);


	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
//	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
}

void spi1_write_buf(uint16_t* pBuffer, uint16_t len)//Ð¿ÐµÑ€ÐµÐºÐ¸Ð½ÑƒÑ‚ÑŒ Ð±ÑƒÑ„ÐµÑ€ Ñ‡ÐµÑ€ÐµÐ· dma
{
	  DMA2_Stream3->M0AR= (uint32_t)pBuffer;
	  DMA2_Stream3->NDTR= len;

	  DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);

	  DMA_Cmd(DMA2_Stream3, ENABLE);

}

//void spi1_read_buf(uint16_t* pBuffer, uint16_t len)//Ð¿ÐµÑ€ÐµÐºÐ¸Ð½ÑƒÑ‚ÑŒ Ð±ÑƒÑ„ÐµÑ€ Ñ‡ÐµÑ€ÐµÐ· dma
//{
//	  DMA2_Stream3->M0AR= (uint32_t)pBuffer;
//	  DMA2_Stream3->NDTR= len;
//
//	//  DMA_ClearFlag(DMA1_FLAG_TC2);
//
//	  DMA_Cmd(DMA2_Stream3, ENABLE);
//}

void spi2_write_buf(uint16_t* pBuffer, uint16_t len)
{
	  DMA1_Stream4->M0AR= (uint32_t)pBuffer;
	  DMA1_Stream4->NDTR= len;

	  DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);

	  DMA_Cmd(DMA1_Stream4, ENABLE);
}

//void spi2_read_buf(uint16_t* pBuffer, uint16_t len)
//{
//	  DMA1_Stream4->M0AR= (uint32_t)pBuffer;
//	  DMA1_Stream4->NDTR= len;
//
//	//  DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
//
//	  DMA_Cmd(DMA1_Stream4, ENABLE);
//}

void spi3_write_buf(uint16_t* pBuffer, uint16_t len)
{
	  DMA1_Stream5->M0AR= (uint32_t)pBuffer;
	  DMA1_Stream5->NDTR= len;

	  DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);

	  DMA_Cmd(DMA1_Stream5, ENABLE);
}

static void spi1_task(void *pvParameters)//½
{
	uint8_t i=0;
	while(1)
	{
		Indicator_Blink_Handler(BUS_SPI_1);
		for(i=0;i<IND_COMMAND_LEN;i++)
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_4,0);

		//	if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
			{
				 spi1_write_buf(&tab.buses[BUS_SPI_1].bus_buf[i][0],IND_SPI_BUS_1_NUM);

				//spi1_write_buf(&ind_test_buf1[i][0],IND_SPI_BUS_1_NUM);
				 while(DMA_GetFlagStatus(DMA2_Stream3,DMA_FLAG_TCIF3)==RESET)
				 {
					 taskYIELD();
				 }
				 DMA_Cmd(DMA2_Stream3, DISABLE);
				 DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);

				 while(SPI1->SR & SPI_SR_BSY)
				 {
					 taskYIELD();
				 }

				 //xSemaphoreGive( xSPI_Buf_Mutex );
			}
			//vTaskDelay(1);
			taskYIELD();
			GPIO_WriteBit(GPIOA, GPIO_Pin_4,1);
			taskYIELD();
			//vTaskDelay(1);
			GPIO_WriteBit(GPIOA, GPIO_Pin_4,0);
		}
		task_watches[SPI_TASK_1].counter++;
		vTaskDelay(50);
	}
}


static void spi2_task(void *pvParameters)
{
	uint8_t i=0;
	while(1)
	{
		Indicator_Blink_Handler(BUS_SPI_2);
		for(i=0;i<IND_COMMAND_LEN;i++)
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_12,0);
			//if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
			{
				spi2_write_buf(&tab.buses[BUS_SPI_2].bus_buf[i][0],IND_SPI_BUS_2_NUM);
				//spi2_write_buf(&ind_test_buf2[i][0],IND_SPI_BUS_1_NUM);
				while(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)==RESET)
				{
					taskYIELD();
				}
				DMA_Cmd(DMA1_Stream4, DISABLE);
				DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);

				while(SPI2->SR & SPI_SR_BSY)
				{
					taskYIELD();
				}

		//		xSemaphoreGive( xSPI_Buf_Mutex );
			}
			taskYIELD();
			GPIO_WriteBit(GPIOB, GPIO_Pin_12,1);
			taskYIELD();
			//vTaskDelay(1);
			GPIO_WriteBit(GPIOB, GPIO_Pin_12,0);
		}
		task_watches[SPI_TASK_2].counter++;
		vTaskDelay(50);
	}
}


static void spi3_task(void *pvParameters)
{
	uint8_t i=0;

	while(1)
	{
		Indicator_Blink_Handler(BUS_SPI_3);
		for(i=0;i<IND_COMMAND_LEN;i++)
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_6,0);
		//	if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
			{
				spi3_write_buf(&tab.buses[BUS_SPI_3].bus_buf[i][0],IND_SPI_BUS_3_NUM);
				//spi3_write_buf(&ind_test_buf3[i][0],IND_SPI_BUS_1_NUM);
				while(DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5)==RESET)
				{
					taskYIELD();
				}
				DMA_Cmd(DMA1_Stream5, DISABLE);
				DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);

				while(SPI3->SR & SPI_SR_BSY)
				{
					taskYIELD();
				}

			//	xSemaphoreGive( xSPI_Buf_Mutex );
			}
			//vTaskDelay(1);
			taskYIELD();
			GPIO_WriteBit(GPIOB, GPIO_Pin_6,1);
			taskYIELD();
			//vTaskDelay(1);
			GPIO_WriteBit(GPIOB, GPIO_Pin_6,0);
		}
		task_watches[SPI_TASK_3].counter++;
		vTaskDelay(50);
	}
}

