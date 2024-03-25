#include "svc_plugins.h"
#include "hal_adc.h"
#include "hal_dht20.h"
#include "hal_adc.h"


/*
* ��������ʼ��
*/
void svcPluginsInit()
{
}

/*
* ��ȡ���ص�DHT20��ʪ������
* @param pTemp ����¶�ֵ
* @param pHumi ���ʪ��ֵ
* 
* @return 
*/
int svcPluginsGetTempHumi(int *pTemp, int *pHumi)
{
    AHT20Data_t data;

    data = AHT20GetData();

    if (!(data.ok)){
			return -1;
		}

    if (pTemp) *pTemp = data.temp;

    if (pHumi) *pHumi = data.humi;

    return 0;
}

/*
* ��ȡ��������ADCͨ��2����ֵ
*/
unsigned int svcPluginsGetADC2(){
	return halAdcRead(ADC_Channel_2);
}

/*
* ��ȡ��������ADCͨ��3����ֵ
*/
unsigned int svcPluginsGetADC3(){
	return halAdcRead(ADC_Channel_3);
}



