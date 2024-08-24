#include "stm32f1xx_hal.h"
#include "cfg.h"

/******************************************************************************
2024.8.17 负责人：wyq
这个文件完成系统初始化
******************************************************************************/

TIM_HandleTypeDef TIM1_Handler, TIM2_Handler;
TIM_OC_InitTypeDef TIM1_CH1Handler;
TIM_OC_InitTypeDef TIM1_CH2Handler;
TIM_OC_InitTypeDef TIM1_CH3Handler;
TIM_HandleTypeDef htim3;
ADC_HandleTypeDef    AdcHandle;
extern motor_ctrl motor;
extern uint16_t g_adc_raw[ADC_CH_NUM*ADC_MEM_NUM];


void h_bridge_init(void)//上桥臂初始化，控制方式采用h_pwm_l_on
{
	uint16_t psc = TIMPSC;
	uint16_t arr = TIMARR;
	TIM1_Handler.Instance=TIM1;          	
	TIM1_Handler.Init.Prescaler=psc;       
	TIM1_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
	TIM1_Handler.Init.Period=arr;          
	TIM1_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM1_Handler);      
    
	TIM1_CH1Handler.OCMode=TIM_OCMODE_PWM1; 
	TIM1_CH1Handler.Pulse=0;
	TIM1_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH1Handler,TIM_CHANNEL_1);
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_1);
	
	TIM1_CH2Handler.OCMode=TIM_OCMODE_PWM1;
	TIM1_CH2Handler.Pulse=0;
	TIM1_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH2Handler,TIM_CHANNEL_2);
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_2);
	
	
	TIM1_CH3Handler.OCMode=TIM_OCMODE_PWM1; 
	TIM1_CH3Handler.Pulse=0;            
	TIM1_CH3Handler.OCPolarity=TIM_OCPOLARITY_HIGH; 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH3Handler,TIM_CHANNEL_3);
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_3);
	
}

void l_bridge_init(void)
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_LBridge, &gpio_init_struct);
}

void H_sensor_init(void)				//霍尔信号采集引脚初始化
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
	gpio_init_struct.Mode = GPIO_MODE_INPUT;
	gpio_init_struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_HALLSENSOR, &gpio_init_struct);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)   //PWM回调函数，用于IO复用配置
{
	if(htim->Instance == TIM1)
	{
		GPIO_InitTypeDef gpio_init_struct;
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_TIM1_CLK_ENABLE();
		
		gpio_init_struct.Pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_13;
		gpio_init_struct.Mode = GPIO_MODE_AF_PP;
		gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
		gpio_init_struct.Pull = GPIO_PULLUP;
		
		HAL_GPIO_Init(GPIOE, &gpio_init_struct);
		__HAL_AFIO_REMAP_TIM1_ENABLE();

	}
}

void HAL_TIM_Base_MapInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();
	
		HAL_NVIC_SetPriority(TIM2_IRQn, 1, 3);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}

}

void Stop_Start_Init(void)
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_5;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_LBridge, &gpio_init_struct);
}

void TIM_Init(void)
{
	TIM2_Handler.Instance = TIM2;
	TIM2_Handler.Init.Prescaler = (TIMPSC2+1)*2-1;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	TIM2_Handler.Init.Period = 1999; //2ms
	
	HAL_TIM_Base_Init(&TIM2_Handler); 
	HAL_TIM_Base_Start_IT(&TIM2_Handler); 
}

/**************************************************************************************

adc初始化函数
日期：2024.8.19

**************************************************************************************/
void adc_init(void)
{
	ADC_ChannelConfTypeDef Sconfig = {0};
	AdcHandle.Instance = ADC_MEASURE;
	AdcHandle.Init.ContinuousConvMode = ENABLE;
	AdcHandle.Init.NbrOfConversion = ADC_CH_NUM;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	
	AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ScanConvMode = ADC_SCAN_ENABLE;
	HAL_ADC_Init(&AdcHandle);
	
	Sconfig.Channel = ADC_CHANNEL_0;
	Sconfig.Rank = ADC_REGULAR_RANK_1;
	Sconfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_3;
	Sconfig.Rank = ADC_REGULAR_RANK_2;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_5;
	Sconfig.Rank = ADC_REGULAR_RANK_3;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_7;
	Sconfig.Rank = ADC_REGULAR_RANK_4;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_9;
	Sconfig.Rank = ADC_REGULAR_RANK_5;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef gpio_init_struct;
	DMA_HandleTypeDef DMAHandle;
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
	gpio_init_struct.Mode = GPIO_MODE_ANALOG;
	gpio_init_struct.Pull = GPIO_NOPULL;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	//HAL_ADC_Start(hadc);
	
	__HAL_RCC_DMA1_CLK_ENABLE();
	DMAHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
	DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DMAHandle.Init.Mode = DMA_CIRCULAR;
	DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&DMAHandle);
	
	__HAL_LINKDMA(&AdcHandle,DMA_Handle,DMAHandle);
	
	HAL_NVIC_SetPriority(ADC1_2_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)g_adc_raw, ADC_CH_NUM*ADC_MEM_NUM);
}

void capture_init(void)
{
	Cap_Tim_Init();

}

void Cap_Tim_Init(void)
{
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = TIMPSC2;
	htim3.Init.Period = 0xFFFFFFFF;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;

	HAL_TIM_IC_Init(&htim3);
	
	TIM_IC_InitTypeDef sConfig;
	sConfig.ICPolarity = TIM_ICPOLARITY_RISING;
	sConfig.ICPrescaler = TIM_ICPSC_DIV1;
	sConfig.ICFilter = 0;
	sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	
	HAL_TIM_IC_ConfigChannel(&htim3, &sConfig, TIM_CHANNEL_1);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM5_CLK_ENABLE(); 
	
	GPIO_InitTypeDef gpio_init_struct;
	
	gpio_init_struct.Pin = GPIO_PIN_0;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	
	__HAL_AFIO_REMAP_TIM3_ENABLE();
	
	__HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
	HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
	
	HAL_NVIC_SetPriority(TIM3_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void sys_init(void)
{
	HAL_Init();
	h_bridge_init();
	l_bridge_init();
	H_sensor_init();
	Stop_Start_Init();
	TIM_Init();
}
