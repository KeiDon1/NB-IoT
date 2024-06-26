#include "svc_plugins.h"
#include "hal_adc.h"
#include "hal_dht20.h"
#include "hal_adc.h"


/*
* 插件服务初始化
*/
void svcPluginsInit()
{
}

/*
* 获取板载的DHT20温湿度数据
* @param pTemp 存放温度值
* @param pHumi 存放湿度值
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
* 获取传感器（ADC通道2）的值
*/
unsigned int svcPluginsGetADC2(){
	return halAdcRead(ADC_Channel_2);
}

/*
* 获取传感器（ADC通道3）的值
*/
unsigned int svcPluginsGetADC3(){
	return halAdcRead(ADC_Channel_3);
}



