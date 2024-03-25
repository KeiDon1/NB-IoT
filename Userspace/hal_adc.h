#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include "stm32f0xx_adc.h"

/*
 *初始化ADC
 */
void halAdcInit(void);

/*
 *读取ADC值
 *
 *@param ch ADC采样通道，支持 ADC_Channel_1 / ADC_Channel_6
 *
 *@return ADC值
 */
unsigned int halAdcRead(uint32_t ch);

#endif /* __HAL_ADC_H__ */
