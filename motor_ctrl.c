#include "cfg.h"
#include "net_pid.h"
/************************************************************************
这个文件用于电机闭环控制
日期：2024.8.17，负责人wyq

*************************************************************************/
extern motor_ctrl motor;
uint16_t counter = 0;
uint8_t start_cnt = 0;//上升沿捕获标志位，捕获到上升沿置1，下降沿置0
uint32_t normal_cnt = 0;
uint32_t speed_actual, target_speed;

long long encoder_num = 0;//用于记录霍尔编码器码数
uint32_t rpm, target_rpm;


pid Pid;

//编码速度转换为转速
void Enc_Rpm(void)
{
	rpm = encoder_num / POLE_NUM;
}

//转速转换为实际速度，单位m/s
void Rpm_Speed(void)
{
	speed_actual = 2*rpm*PI*RADIUS;
}

//实际速度转换为转速，用于目标转速计算
void Speed_Rpm(void)
{
	target_rpm = target_speed / (2*rpm*PI);
}

//PID控制器初始化函数
void Pid_Conf(pid*k)
{
	k->ka = Kp+Ki+Kd;
	k->kb = -Kp-2*Kd;
	k->kc = Kd;
	
	k->limit_h = TIMARR*V_MAX/V_SOURCE;
	k->limit_l = 0;
	
	k->error = 0;
	k->error_t = 0;
	k->error_t_prim = 0;
	
	k->pid_out = 0;
}

//PID控制器输出计算
void Pid_Cal(pid*k, int16_t e)
{
	k->error_t_prim = k->error_t;
	k->error_t = k->error;
	k->error = e;
	
	int16_t pid_out;
	pid_out = k->ka*k->error+k->kb*k->error_t+k->kc*k->error_t_prim;
	
	k->pid_out += pid_out;
	k->pid_out = k->pid_out>k->limit_h?k->limit_h:(k->pid_out<k->limit_l?k->limit_l:k->pid_out);
}

void cal_motor(pid*k)
{
	Speed_Rpm();
	Enc_Rpm();
	int16_t err = target_rpm - rpm;
	Pid_Cal(&Pid, err);
	motor.pulse = k->pid_out;
}
