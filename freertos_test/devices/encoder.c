#include "encoder.h"

//uint32_t counter=0x80008000;
uint32_t counter2=0x80008000;
uint32_t tim1_cnt=0;

//void TIM3_IRQHandler(void)
//{
//  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//  {
//    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//
//    if(TIM3->CR1 & TIM_CR1_DIR)
//    	counter-=0x10000 ;
//    else
//    	counter+=0x10000;
//  }
//}

void TIM1_UP_TIM10_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    if(TIM1->CR1 & TIM_CR1_DIR)
    	counter2-- ;
    else
    	counter2++;

    tim1_cnt=TIM1->CNT;
  }
}

void Encoder_Init(void)//инициализаци€ таймера дола
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//тактируем портј
//	RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3EN, ENABLE);//тактируем таймер 3
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1EN, ENABLE);//тактируем таймер 1

	 	//настройка таймера дола
		TIM_TimeBaseInitTypeDef timer_base;
	    TIM_TimeBaseStructInit(&timer_base);
	    timer_base.TIM_Period = 1;//65535;
	    timer_base.TIM_Prescaler=0;
	    timer_base.TIM_ClockDivision=0;
	    timer_base.TIM_CounterMode = TIM_CounterMode_Down | TIM_CounterMode_Up;
//	    TIM_TimeBaseInit(TIM3, &timer_base);
	    TIM_TimeBaseInit(TIM1, &timer_base);

//	    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);
//	    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//	    TIM_Cmd(TIM3, ENABLE);

	    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	    TIM_Cmd(TIM1, ENABLE);

	    //настройка прерывани€ дола
	    NVIC_InitTypeDef NVIC_InitStructure;
	    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 14;
	    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	    NVIC_Init(&NVIC_InitStructure);

	    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	    NVIC_Init(&NVIC_InitStructure);

	    //настройка пинов микроконтроллера
	    GPIO_InitTypeDef  GPIO_InitStructure;

//	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
//	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;;
//	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	    GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
//	    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);

	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
	    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);
	    TIM1->CNT=0;
}
