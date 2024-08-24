#include "stm32f1xx_hal.h"
#include "cfg.h"

motor_ctrl motor = {0};
extern TIM_HandleTypeDef TIM1_Handler;
extern uint16_t counter;
extern uint8_t start_cnt; //上升沿获取标志位
extern long long encoder_num;
extern uint32_t normal_cnt; //获取输入捕获计数值

pctr basic_ctrl[6] =
{
	&uhwl, &vhul, &vhwl,
	&whvl, &uhvl, &whul
};


uint8_t forward[6] = {5, 1, 3, 2, 6, 4};
uint8_t backward[6] = {4, 6, 2, 3, 1, 5};

uint8_t idx_f[6] = {1, 3, 2, 5, 0, 4};

//霍尔编码器信号采集
uint8_t hallsensor(void) 
{
	uint8_t state = 0x00;
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_11) != RESET)
	{
		state |= 0x01;
	}
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_12) != RESET)
	{
		state |= 0x02;
	}
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_13) != RESET)
	{
		state |= 0x04;
	}
	
	return state;
}

void Is_Forward(void)
{
	uint16_t idx;
	idx = (idx_f[motor.step_now]+5)%6;

	if(forward[idx] == motor.step_last)
	{
		motor.dir = FORWARD;
	}
	else
		motor.dir = BACKWARD;

}
//六步换向
void uhvl(void)
{
	TIM1_Handler.Instance->CCR1 = motor.pulse;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_ON;
	WL_OFF;
}

void uhwl(void)
{
	TIM1_Handler.Instance->CCR1 = motor.pulse;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void vhul(void)
{
	TIM1_Handler.Instance->CCR1 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulse;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void vhwl(void)
{
	TIM1_Handler.Instance->CCR1 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulse;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void whul(void)
{
	TIM1_Handler.Instance->CCR1 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulse;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void whvl(void)
{
	TIM1_Handler.Instance->CCR1 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulse;

	UL_OFF;
	VL_ON;
	WL_OFF;
}

void Stop_motor(void)   //ͨ����EN_GATE�Ӹ��õ͹ض��ű�
{
	SHUTOFF;
	
	TIM1_Handler.Instance->CCR1 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;
	
	UL_OFF;
	VL_OFF;
	WL_OFF;
	
	motor.run_flag = STOP;
}

void Start_motor(void)
{
		SHUTON;
		
		TIM1_Handler.Instance->CCR1 = 0;
		TIM1_Handler.Instance->CCR2 = 0;
		TIM1_Handler.Instance->CCR3 = 0;
		
		UL_OFF;
		VL_OFF;
		WL_OFF;
		
		motor.run_flag = START;
}

/*******************************************************************************
��ʱ���жϻص�����
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(motor.run_flag == START)    //�����������״̬�Ż��ȡ
		{
			motor.step_last = motor.step_now;
			motor.step_now = hallsensor();
			Is_Forward();
			if(motor.step_now >= 1 && motor.step_now <= 6)
			{
				if(motor.dir == FORWARD) //�����ת
				{
					basic_ctrl[motor.step_now-1]();			
				}
				else
				{
					basic_ctrl[6 - motor.step_now]();	
				}
			}
		}
	}
	if(htim->Instance == TIM3)
	{
		if(start_cnt == 1) //�ڲ��������ص�ʱ��ʼ����
		{
			counter++;
		}
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance  == TIM3)
	{
		if(start_cnt == 0)//����������
		{
			start_cnt = 1;
			__HAL_TIM_DISABLE(htim);
			__HAL_TIM_SET_COUNTER(htim, 0);
			htim->Instance->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
			TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
			__HAL_TIM_ENABLE(htim);
		}
		else
		{
			start_cnt = 0;
			normal_cnt = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
			htim->Instance->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
			TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
			encoder_num = 1000000 / normal_cnt + counter*0xFFFFFFFF;
			counter = 0;
		}
	}
}
