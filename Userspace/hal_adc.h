#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include "stm32f0xx_adc.h"

/*
 *��ʼ��ADC
 */
void halAdcInit(void);

/*
 *��ȡADCֵ
 *
 *@param ch ADC����ͨ����֧�� ADC_Channel_1 / ADC_Channel_6
 *
 *@return ADCֵ
 */
unsigned int halAdcRead(uint32_t ch);

#endif /* __HAL_ADC_H__ */
