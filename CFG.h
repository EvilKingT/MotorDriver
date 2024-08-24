#ifndef CFG_H
#define CFG_H
#include "stm32f1xx_hal.h"
#include "init.h"
#include "bldc.h"
#include "motor_ctrl.h"
#include "measure.h"
#include "comm.h"

#define TIMPSC 71
#define TIMARR 499
#define TIMPSC2 35
#define RADIUS 5
#define PI 3.141592654

#define GPIO_HALLSENSOR  GPIOA
#define GPIO_LBridge GPIOF

#define UL_ON  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)
#define VL_ON  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_SET)
#define WL_ON  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET)
#define UL_OFF  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)
#define VL_OFF  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET)
#define WL_OFF  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET)

#define SHUTOFF HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_RESET)
#define SHUTON   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_SET)

/*********************************************************************
电机启停状况和转动方向参数宏定义
**********************************************************************/
#define START 1
#define STOP 0

#define FORWARD 1
#define BACKWARD 0

#define ADC_MEASURE ADC1
#define ADC_CH_NUM 5
#define ADC_MEM_NUM 50
#define ADC_COLL 3

/**********************************************************************************
电机电学参数
**********************************************************************************/
#define V_SOURCE 24  //驱动板最大供电
#define V_MAX 18 //电机最大耐受电压
#define POLE_NUM 8 //maxon极对数

typedef void(*pctr) (void);

#endif
