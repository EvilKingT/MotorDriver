#ifndef CFG_H
#define CFG_H
#include "stm32f1xx_hal.h"
#include "init.h"
#include "bldc.h"
#include "motor_ctrl.h"

#define TIMPSC 71
#define TIMARR 499

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

/*
电机启停判断标志位定义

*/
#define START 1
#define STOP 0

#define FORWARD 1
#define BACKWARD 0

#endif
