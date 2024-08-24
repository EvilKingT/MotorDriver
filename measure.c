#include "cfg.h"

/*********************************************************************
2024.8.17 �����ˣ�wyq
���ļ�����ģ���źŲɼ��Ͳ��������Ƶ�������ѹ���¶�
*********************************************************************/
uint16_t g_adc_raw[ADC_CH_NUM*ADC_MEM_NUM];
uint16_t g_adc_ave[ADC_CH_NUM];
uint16_t phase_offset[ADC_COLL];

extern motor_ctrl motor;
extern ADC_HandleTypeDef    AdcHandle;

void adc_ave_filter(void)
{
	uint8_t i = 0, j = 0;
	uint32_t tmp[ADC_CH_NUM]={0};
	for(i = 0;i<ADC_CH_NUM;i++)
	{
		for(j = 0; j<ADC_MEM_NUM;j++)
		{
			tmp[i] += g_adc_raw[i+j*ADC_CH_NUM];
		}
		g_adc_ave[i] = tmp[i]/ADC_MEM_NUM;
	}
}

//���ֹͣ״̬�µ��ã�������ѹ��Ưֵ
void adc_offset(void)
{
	uint8_t i = 0, j = 0;
	uint32_t tmp[ADC_COLL]={0};
	for(i = ADC_COLL-1;i<ADC_CH_NUM;i++)
	{
		for(j = 0; j<ADC_MEM_NUM;j++)
		{
			tmp[i] += g_adc_raw[i+j*ADC_CH_NUM];
		}
		phase_offset[i] = tmp[i]/ADC_MEM_NUM;
	}
}

/**************************************************************************************************
ADC�жϻص����������ڲɼ�ģ���ź�������������ѹ���¶ȣ�
**************************************************************************************************/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		HAL_ADC_Stop_DMA(&AdcHandle);
		if(motor.run_flag == STOP)
			adc_offset();
		else
			adc_ave_filter();
		HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)g_adc_raw, ADC_CH_NUM*ADC_MEM_NUM);
	}
}
