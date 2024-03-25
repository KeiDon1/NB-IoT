#include "hal_adc.h"

/*
* ADC初始化
*/
void halAdcInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	  //指定PA2、PA3为ADC采样通道
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3;
	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_DeInit(ADC1);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);

    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

    ADC_DMACmd(ADC1, DISABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_8b;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Upward;

    ADC_Init(ADC1, &ADC_InitStruct);

    /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */
    ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_55_5Cycles);//对应PA2
    ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_55_5Cycles);//对应PA3

    ADC_GetCalibrationFactor(ADC1);
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));

    ADC_StartOfConversion(ADC1);
}

/*
 *读取ADC值
 *
 *@param ch ADC采样通道，支持 ADC_Channel_2 / ADC_Channel_3
 *
 *@return ADC值
 */
unsigned int halAdcRead(uint32_t ch)
{
    ADC1->CHSELR = 0;
    ADC1->CHSELR = ch;

    ADC_StartOfConversion(ADC1);

    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);

    return ADC_GetConversionValue(ADC1);
}
