#include "bldc.h"
#include "stm32f1xx_hal.h"
#include "cfg.h"

motor_ctrl motor = {0};
extern TIM_HandleTypeDef TIM1_Handler;

//��ȡ��������״̬
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

//����������
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
