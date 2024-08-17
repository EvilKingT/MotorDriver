#include "main.h"
#include "cfg.h"
#include "init.h"
#include "bldc.h"

extern motor_ctrl motor;

int main(void)
{
	sys_init();
	motor.pulse = (TIMARR+1)/2;		//50%Õ¼¿Õ±È
	uhvl();
	while(1)
	{
	}
	
}
