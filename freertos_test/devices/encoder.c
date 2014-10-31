#include "encoder.h"
#include "drive.h"
#include "buzzer.h"

extern struct drive drv;

void TIM1_UP_TIM10_IRQHandler(void)
{
//  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
	  TIM1->SR = (uint16_t)~TIM_IT_Update;
//    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    if(TIM1->CR1 & TIM_CR1_DIR)
    {
    	drv.current_position--;
    }
    else
    {
    	drv.current_position++;
    }

    if((drv.error_flag==DRIVE_OK)&&(drv.stop_type==STOP_NONE)&&(drv.move_type_flag!=MOVE_TYPE_NONE))
    {
    	if(drv.current_position==drv.min_speed_position)
    	{
    		Drive_Set_Speed(DRIVE_SPEED_LOW);
    	}

    	if(drv.current_position==drv.stop_position)
    	{
    		Drive_Stop(STOP_END_OF_OPERATION,FROM_ISR);
    	}
    }
  }
}

void Encoder_Init(void)//инициализаци€ таймера дола
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//тактируем портј
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1EN, ENABLE);//тактируем таймер 1

	 	//настройка таймера дола
		TIM_TimeBaseInitTypeDef timer_base;
	    TIM_TimeBaseStructInit(&timer_base);
	    timer_base.TIM_Period = 1;//65535;
	    timer_base.TIM_Prescaler=0;
	    timer_base.TIM_ClockDivision=0;
	    timer_base.TIM_CounterMode = TIM_CounterMode_Down | TIM_CounterMode_Up;
	    TIM_TimeBaseInit(TIM1, &timer_base);

	    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);
	    TIM1->CCER = TIM_CCER_CC1P | TIM_CCER_CC2P;
	    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	    TIM_Cmd(TIM1, ENABLE);

	//    TIM1->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
	    //TIM1->CCER = TIM_CCER_CC1P | TIM_CCER_CC2P;
	  //  TIM1->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;

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

	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
	    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);
	    TIM1->CNT=0;
}
