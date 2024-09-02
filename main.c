#include "main.h"
#include "cfg.h"
#include "init.h"
#include "bldc.h"
#include "adc.h"
#include "spi.h"

extern motor_ctrl motor;

// add by ty, maybe there was some error
extern ADC_HandleTypeDef hadc;
extern void ADC_Config(void);
extern uint16_t Read_ADC(void);

int main(void)
{
	sys_init();
	motor.pulse = (TIMARR+1)/2;		//50%ռ�ձ�
	uhvl();

	// add by ty, maybe there was some error
	HAL_Init(); // 初始化HAL库
    ADC_Config(); // 配置ADC
	SPI_Init(); // 初始化SPI

    while (1) {
        SPI_Transmit(0xAA); // 发送数据
        uint8_t receivedData = SPI_Receive(); // 接收数据
        // 处理接收到的数据
    }
	
    while (1) {
        uint16_t adcValue = Read_ADC();
        // 处理 adcValue
    }
}


