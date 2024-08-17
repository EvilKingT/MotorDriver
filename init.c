#include "stm32f1xx_hal.h"
#include "cfg.h"

TIM_HandleTypeDef TIM1_Handler, TIM2_Handler;
TIM_OC_InitTypeDef TIM1_CH1Handler;
TIM_OC_InitTypeDef TIM1_CH2Handler;
TIM_OC_InitTypeDef TIM1_CH3Handler;
ADC_HandleTypeDef    AdcHandle;
extern motor_ctrl motor;

void h_bridge_init(void)//����pwmΪ2kHZ����ʼռ�ձ�Ϊ50%
{
	uint16_t psc = TIMPSC;
	uint16_t arr = TIMARR;
	TIM1_Handler.Instance=TIM1;          	//��ʱ��1
	TIM1_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
	TIM1_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
	TIM1_Handler.Init.Period=arr;          //�Զ���װ��ֵ
	TIM1_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM1_Handler);       //��ʼ��PWM
    
	TIM1_CH1Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
	TIM1_CH1Handler.Pulse=0;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
	TIM1_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ�� 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH1Handler,TIM_CHANNEL_1);//����TIM1ͨ��1
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_1);//����PWMͨ��1
	
	TIM1_CH2Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
	TIM1_CH2Handler.Pulse=0;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
	TIM1_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ�� 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH2Handler,TIM_CHANNEL_2);//����TIM1ͨ��2
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_2);//����PWMͨ��2
	
	
	TIM1_CH3Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
	TIM1_CH3Handler.Pulse=0;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
	TIM1_CH3Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ�� 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH3Handler,TIM_CHANNEL_3);//����TIM1ͨ��3
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_3);//����PWMͨ��3
	
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

void H_sensor_init(void)				//���û����źŶ�ȡ����
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
	gpio_init_struct.Mode = GPIO_MODE_INPUT;
	gpio_init_struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_HALLSENSOR, &gpio_init_struct);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)   //PWM���ûص�����
{
	if(htim->Instance == TIM1)
	{
		GPIO_InitTypeDef gpio_init_struct;
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_TIM1_CLK_ENABLE();
		
		gpio_init_struct.Pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_13;//������ӳ�䵽PE9��11��13
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
	TIM2_Handler.Init.Prescaler = (TIMPSC+1)*2-1;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	TIM2_Handler.Init.Period = 1999; //2ms
	
	HAL_TIM_Base_Init(&TIM2_Handler); 
	HAL_TIM_Base_Start_IT(&TIM2_Handler); 
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(motor.run_flag == START)    //�����������״̬�Ż��ȡ
		{
			motor.step_last = motor.step_now;
			motor.step_now = hallsensor();
			if(motor.step_now >= 1 && motor.step_now <= 6)
			{
				if(motor.dir == FORWARD) //�����ת
				{
					switch(motor.step_now)
					{
						case 0x01:
							uhwl();
							break;
						case 0x02:
							vhul();
							break;
						case 0x03:
							vhwl();
							break;
						case 0x04:
							whvl();
							break;
						case 0x05:
							uhvl();
							break;
						case 0x06:
							whul();
							break;
					}				
				}
				else
				{
					switch(motor.step_now)
					{
						case 0x01:
							whul();
							break;
						case 0x02:
							uhvl();
							break;
						case 0x03:
							whvl();
							break;
						case 0x04:
							vhwl();
							break;
						case 0x05:
							vhul();
							break;
						case 0x06:
							uhwl();
							break;
					}	
				
				}
			}
		}
	}
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM2_Handler);
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
