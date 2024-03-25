#ifndef __SVC_PLUGINS_H__
#define __SVC_PLUGINS_H__

void svcPluginsInit(void);

int svcPluginsGetTempHumi(int *pTemp, int *pHumi);

/*
* 获取传感器（ADC通道2）的值
*/
unsigned int svcPluginsGetADC2();

/*
* 获取传感器（ADC通道3）的值
*/
unsigned int svcPluginsGetADC3();

#endif /* __SVC_PLUGINS_H__ */