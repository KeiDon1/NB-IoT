#ifndef __SVC_PLUGINS_H__
#define __SVC_PLUGINS_H__

void svcPluginsInit(void);

int svcPluginsGetTempHumi(int *pTemp, int *pHumi);

/*
* ��ȡ��������ADCͨ��2����ֵ
*/
unsigned int svcPluginsGetADC2();

/*
* ��ȡ��������ADCͨ��3����ֵ
*/
unsigned int svcPluginsGetADC3();

#endif /* __SVC_PLUGINS_H__ */