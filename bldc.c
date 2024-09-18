#include "stm32f1xx_hal.h"
#include "cfg.h"

motor_ctrl motor = {0};
int32_t cnt_all = 0, last_cnt = 0;
extern TIM_HandleTypeDef TIM1_Handler, htim3;
extern uint16_t counter;
extern uint8_t start_cnt; //涓婂崌娌胯幏鍙栨爣蹇椾綅
extern long long encoder_num;
extern uint32_t normal_cnt; //鑾峰彇杈撳叆鎹曡幏璁℃暟鍊�

pctr basic_ctrl[6] =
{
	&uhwl, &vhul, &vhwl,
	&whvl, &uhvl, &whul
};


uint8_t forward[6] = {5, 1, 3, 2, 6, 4};
uint8_t backward[6] = {4, 6, 2, 3, 1, 5};

uint8_t idx_f[6] = {1, 3, 2, 5, 0, 4};

//闇嶅皵缂栫爜鍣ㄤ俊鍙烽噰闆�
uint8_t hallsensor(void) 
{
	uint8_t state = 0x00;
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_14) != RESET)
	{
		state |= 0x01;
	}
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_13) != RESET)
	{
		state |= 0x02;
	}
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_12) != RESET)
	{
		state |= 0x04;
	}
	
	return state;
}

uint8_t Is_Forward(void)
{
	uint16_t idx;
	idx = (idx_f[motor.step_now]+5)%6;

	if(forward[idx] == motor.step_last)
	{
		return 1;
	}
	else
		return 0;

}
//鍏鎹㈠悜
void uhvl(void)
{
	TIM1_Handler.Instance->CCR4= motor.pulse;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_ON;
	WL_OFF;
}

void uhwl(void)
{
	TIM1_Handler.Instance->CCR4 = motor.pulse;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void vhul(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulse;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void vhwl(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulse;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void whul(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulse;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void whvl(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulse;

	UL_OFF;
	VL_ON;
	WL_OFF;
}

void Stop_motor(void)   //通过将EN_GATE从高置低关断桥臂
{
	SHUTOFF;
	
	TIM1_Handler.Instance->CCR4 = 0;
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
		
		TIM1_Handler.Instance->CCR4 = 0;
		TIM1_Handler.Instance->CCR2 = 0;
		TIM1_Handler.Instance->CCR3 = 0;
		
		UL_OFF;
		VL_OFF;
		WL_OFF;
		
		motor.run_flag = START;
}

/*******************************************************************************
定时器中断回调函数
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t i;
	if(htim->Instance == TIM2)
	{
		if(motor.run_flag == START)    //电机处于运行状态才会读取
		{
			motor.step_last = motor.step_now;
			motor.step_now = hallsensor();
			if(motor.step_now >= 1 && motor.step_now <= 6)
			{
				if(motor.dir == FORWARD) //电机正转
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
			counter++;
	}
	if(htim->Instance == TIM4)
	{
		getEncode(i);
		counter = 0;
	}
}

void getEncode(uint8_t i)
{
	cnt_all = __HAL_TIM_GET_COUNTER(&htim3);
	cnt_all += counter*65536;
	cnt_all -= last_cnt;
	last_cnt = __HAL_TIM_GET_COUNTER(&htim3);
	cnt_all = cnt_all*3000/52;
}

void gtimRestart(void) 
{ 
	 __HAL_TIM_DISABLE(&htim3); /* 关闭定时器 TIMX */ 
	 counter = 0; /* 累加器清零 */ 
	 __HAL_TIM_SET_COUNTER(&htim3, 0); /* 计数器清零 */ 
	 __HAL_TIM_ENABLE(&htim3); /* 使能定时器 TIMX */ 

} 
