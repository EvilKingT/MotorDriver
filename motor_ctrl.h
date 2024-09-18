#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H
#include "stm32f1xx_hal.h"

#define Kp 0.095
#define Ki  0.007
#define Kd 0.001

//这是一个PID控制器
typedef struct
{
	int16_t ka;
	int16_t kb;
	int16_t kc;
	
	int16_t error;
	int16_t error_t;
	int16_t error_t_prim;
	
	uint16_t limit_h;//输出限幅，PWM占空比非负
	uint16_t limit_l;
	
	uint16_t pid_out;
}pid;

#endif
