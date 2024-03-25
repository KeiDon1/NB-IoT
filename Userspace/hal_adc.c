#include "hal_adc.h"

/*
* ADC��ʼ��
*/
void halAdcInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	  //ָ��PA2��PA3ΪADC����ͨ��
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
    ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_55_5Cycles);//��ӦPA2
    ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_55_5Cycles);//��ӦPA3

    ADC_GetCalibrationFactor(ADC1);
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));

    ADC_StartOfConversion(ADC1);
}

/*
 *��ȡADCֵ
 *
 *@param ch ADC����ͨ����֧�� ADC_Channel_2 / ADC_Channel_3
 *
 *@return ADCֵ
 */
unsigned int halAdcRead(uint32_t ch)
{
    ADC1->CHSELR = 0;
    ADC1->CHSELR = ch;

    ADC_StartOfConversion(ADC1);

    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);

    return ADC_GetConversionValue(ADC1);
}
