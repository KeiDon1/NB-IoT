#include "task_data_upload.h"

#include "svc_plugins.h"
#include "svc_log.h"
#include "svc_task.h"
#include "svc_msg.h"

#include <stdio.h>
#include <string.h>

/*��Ѷ��MQTT��������Ϣ*/
//���� 
static const char *DOMAIN = "106.55.124.154";
//�˿�
static const char *PORT="1883";
//�ͻ���ID
static const char *CLIENT_ID = "9QH3I85XF5";
//�û���
static const char *USERNAME = "9QH3I85XF5MySensor;12010126;7LQKV;2001295204";
//����
static const char *PASSWORD = "90761d4047810e04d2cf3d8c369604a2f5b8aff3fcddc8539aee4396a6f7a911;hmacsha256";
//����A
static const char *TOPIC_A = "$thing/up/property/9QH3I85XF5/MySensor";


/* NB-IoTģ�鹤�������б� */
enum {
    TASK_NBIOT_AT_TEST = 0,
    TASK_NBIOT_AT_TEST_RSP,

    TASK_NBIOT_DISABLE_PSM,
    TASK_NBIOT_DISABLE_PSM_RSP,

    TASK_NBIOT_DISABLE_ECHO,
    TASK_NBIOT_DISABLE_ECHO_RSP,

    TASK_NBIOT_SET_BAND,
    TASK_NBIOT_SET_BAND_RSP,

    TASK_NBIOT_DHCP,
    TASK_NBIOT_DHCP_RSP,

    TASK_NBIOT_MQTT_OPEN,
    TASK_NBIOT_MQTT_OPEN_RSP,

    TASK_NBIOT_MQTT_CONNECT,
    TASK_NBIOT_MQTT_CONNECT_RSP,

    TASK_NBIOT_GET_DATA,

    TASK_NBIOT_MQTT_PRE_SEND,
    TASK_NBIOT_MQTT_SEND,

    TASK_NBIOT_MQTT_CLOSE,

    TASK_NBIOT_FINISH,

    TASK_NBIOT_ERROR,
    TASK_NBIOT_ERROR_RSP,
};

/* NB-IoTģ�鹤������ */
static int nbStep;

/* ��ʱ�� */
static int timeCounter;

/* ��Ļ��ʾ�ַ��� */
static char lcdString[16];
	
/* ATָ���ַ��� */
static char atString[256];
	
/* �����ַ��� */
static char dataString[256];

/* ATָ��ͱ�� */
static uint8_t isSendAT;
	
/* data���ݲɼ� */
//static int valueA;//�¶�
//static int valueB;//ʪ��
static int valueC;//ADCֵ
//static int valueD;//ADCֵ

static void dataUploadRunner(uint8_t, void *);
static int atDefaultCheck(void);
static int atDHCPCheck(void);
static int atMqttOpenCheck(void);
static int atMqttConnectCheck(void);



/*
* �����ʼ��
*/
void taskDataUploadInit()
{
    nbStep=TASK_NBIOT_AT_TEST;
	  
    svcTaskAdd(
		  1000,
			1, 
			dataUploadRunner,
			0);
}


void dataUploadRunner(uint8_t id, void *args)
{
	
	switch (nbStep) {
			/* AT Test */
			case TASK_NBIOT_AT_TEST:
					svcLogWriteLcd(0, (uint8_t *)"CMD: AT      ", 0, 0, 0);

					svcMsgWriteString("AT\r\n");

					nbStep++;
			break;

			/* Response: AT Test */
			case TASK_NBIOT_AT_TEST_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: AT      ", 0, 0, 0);

					if (atDefaultCheck() == 0) 
						nbStep++;
					else 
						nbStep--;
			break;

			/* Disable PSM */
			case TASK_NBIOT_DISABLE_PSM:
					svcLogWriteLcd(0, (uint8_t *)"CMD: QSCLK=0      ", 0, 0, 0);

					svcMsgWriteString("AT+QSCLK=0\r\n");

					nbStep++;
			break;

			/* Response: Disable PSM */
			case TASK_NBIOT_DISABLE_PSM_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: QSCLK=0      ", 0, 0, 0);

					if (atDefaultCheck() == 0) 
						nbStep++;
					else 
						nbStep--;
			break;

			/* Disable Echo */
			case TASK_NBIOT_DISABLE_ECHO:
					svcLogWriteLcd(0, (uint8_t *)"CMD: ATE0      ", 0, 0, 0);

					svcMsgWriteString("ATE0\r\n");

					nbStep++;
			break;

			/* Response: Disable Echo */
			case TASK_NBIOT_DISABLE_ECHO_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: ATE0      ", 0, 0, 0);

					if (atDefaultCheck() == 0) 
						nbStep++;
					else 
						nbStep--;
			break;

			/* Set Band */
			case TASK_NBIOT_SET_BAND:
					svcLogWriteLcd(0, (uint8_t *)"CMD: QBAND=8      ", 0, 0, 0);

					svcMsgWriteString("AT+QBAND=1,8\r\n");

					nbStep++;
			break;

			/* Response: Set Band */
			case TASK_NBIOT_SET_BAND_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: QBAND=8      ", 0, 0, 0);

					if (atDefaultCheck() == 0) 
						nbStep++;
					else 
						nbStep--;
			break;

			/* DHCP */
			case TASK_NBIOT_DHCP:
					svcLogWriteLcd(0, (uint8_t *)"CMD: DHCP      ", 0, 0, 0);

					svcMsgWriteString("AT+CGPADDR?\r\n");

					nbStep++;

					timeCounter = 0;
			break;

			/* Response: DHCP */
			case TASK_NBIOT_DHCP_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: DHCP      ", 0, 0, 0);
					
					timeCounter++;
					if(timeCounter>=2
						&& atDHCPCheck() == 0){
							nbStep++;
					}
					else if(timeCounter>=8){
						nbStep--;
					}
					
			break;

			/* MQTT Open */
			case TASK_NBIOT_MQTT_OPEN:
					svcLogWriteLcd(0, (uint8_t *)"CMD: QMTOPEN      ", 0, 0, 0);

					sprintf(atString, "AT+QMTOPEN=0,\"%s\",1883\r\n", DOMAIN);
					svcMsgWriteString(atString);

					nbStep++;

					timeCounter = 0;
			break;

			/* Response: MQTT Open */
			case TASK_NBIOT_MQTT_OPEN_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: QMTOPEN      ", 0, 0, 0);
					
					timeCounter++;
					if(timeCounter>=2
						&& atMqttOpenCheck() == 0){
							nbStep++;
						}
					else if(timeCounter>=8){
						nbStep = TASK_NBIOT_ERROR;
					}
					
			break;

			/* MQTT Connect */
			case TASK_NBIOT_MQTT_CONNECT:
					svcLogWriteLcd(0, (uint8_t *)"CMD: QMTCONN      ", 0, 0, 0);

					sprintf(atString, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n", CLIENT_ID, USERNAME, PASSWORD);
					svcMsgWriteString(atString);

					nbStep++;

					timeCounter = 0;
			break;

			/* Response: MQTT Connect */
			case TASK_NBIOT_MQTT_CONNECT_RSP:
					svcLogWriteLcd(0, (uint8_t *)"RSP: QMTCONN      ", 0, 0, 0);

					timeCounter++;
					
					if (timeCounter>=2&&atMqttConnectCheck() == 0){
						svcLogWriteLcd(0,0, (uint8_t *)"", 0, 0);
						nbStep++;
					}
					else if(timeCounter >= 8){
						svcMsgWriteString("AT+QMTDISC=0\r\n");
						nbStep = TASK_NBIOT_ERROR;
					}
					
			break;

			/* Get data */
			case TASK_NBIOT_GET_DATA:
					svcLogWriteLcd(0, (uint8_t *)"CMD: get data      ", 0, 0, 0);
			
          valueC=svcPluginsGetADC2();
			    //valueD=svcPluginsGetADC3();
	
	        sprintf(lcdString,"value:%d        ",valueC);
			    //sprintf(lcdString,"C:%d D:%d     ",valueC,valueD);
			    svcLogWriteLcd(0,0,0,(uint8_t*)"data:",(uint8_t*)lcdString);		

					nbStep++;

			break;

			/* MQTT Pre-Send */
			case TASK_NBIOT_MQTT_PRE_SEND:
					svcLogWriteLcd(0, (uint8_t *)"CMD: QMTPUB      ", 0, 0, 0);

					sprintf(dataString, "{\"method\":\"report\",\"clientToken\":\"123\",\"timestamp\":1628646783,\"params\":{\"value\":%d}}",valueC);
			
			    //sprintf(dataString, "{\"method\":\"report\",\"clientToken\":\"123\",\"timestamp\":1628646783,\"params\":{\"valueC\":%d,\"valueD\":%d}}",valueC,valueD);
			
					sprintf(atString, "AT+QMTPUB=0,0,0,0,\"%s\",%d\r\n", TOPIC_A, strlen(dataString));

					svcMsgWriteString(atString);

					nbStep++;
			
					timeCounter = 0;
			    isSendAT=0;
			break;

			/* MQTT Send */
			case TASK_NBIOT_MQTT_SEND:
					svcLogWriteLcd(0, (uint8_t *)"CMD: SEND       ", 0, 0, 0);

			    if(isSendAT==0){
						svcMsgWriteString(dataString);
						isSendAT=1;
					}
          
					timeCounter++;
					
					if (atDefaultCheck()==0){
						svcLogWriteLcd(0,0, (uint8_t *)"", 0, 0);
						nbStep = TASK_NBIOT_GET_DATA;
					}
					else if(timeCounter >= 8){
						nbStep = TASK_NBIOT_GET_DATA;
					}

			break;

			/* MQTT Close */
			case TASK_NBIOT_MQTT_CLOSE:
					svcLogWriteLcd(0, (uint8_t *)"CMD: CLOSE      ", 0, 0, 0);

					svcMsgWriteString("AT+QMTDISC=0\r\n");

					nbStep++;
			break;

			/* Finish */
			case TASK_NBIOT_FINISH:
					svcLogWriteLcd(0, (uint8_t *)"CMD: Finish      ", 0, 0, 0);
			break;

			/* RESET */
			case TASK_NBIOT_ERROR:
					svcLogWriteLcd(0, (uint8_t *)"CMD: RESET      ", 0, 0, 0);

					svcMsgWriteString("AT+QRST=1\r\n");

					nbStep++;

					timeCounter = 0;
			break;

			/* RSP: RESET */
			case TASK_NBIOT_ERROR_RSP:
					timeCounter++;
				  if(atDefaultCheck() == 0){
						nbStep = TASK_NBIOT_DHCP;
					}
					else if (timeCounter >= 3) {
						nbStep = TASK_NBIOT_DHCP;
					}
					else{
						nbStep--;
					}
			break;

			default: break;
	}
}

int atDefaultCheck()
{
    uint16_t len;

    uint8_t buf[SVC_MSG_BUF_MAX + 1];

    memset(buf, 0, sizeof(buf));

    len = svcMsgRead(buf, sizeof(buf));
    if (len == 0) return -1;

    if (strstr((char *)buf, "OK") != NULL) return 0;

    return -1;
}

/*
* ����ATָ�������
*/
int atDHCPCheck()
{
    uint16_t len;

    uint8_t buf[SVC_MSG_BUF_MAX + 1];

    memset(buf, 0, sizeof(buf));

    len = svcMsgRead(buf, sizeof(buf));
    if (len == 0) return -1;

    if (strstr((char *)buf, "OK") == NULL) return -1;

    char *start = strstr((char *)buf, "+CGPADDR: ");
    if (start == NULL) return -1;

    char *ip = start + 13;

    char *end = strchr(ip, '"');
    if (end == NULL) return -1;

    *end = 0;

    if (strlen(ip) < 7) return -1;

    return 0;
}

int atMqttOpenCheck()
{
    uint16_t len;

    uint8_t buf[SVC_MSG_BUF_MAX + 1];

    memset(buf, 0, sizeof(buf));

    len = svcMsgRead(buf, sizeof(buf));
    if (len == 0) return -1;

    if (strstr((char *)buf, "+QMTOPEN: ") != NULL) return 0;

    return -1;
}

int atMqttConnectCheck()
{
    uint16_t len;

    uint8_t buf[SVC_MSG_BUF_MAX + 1];

    memset(buf, 0, sizeof(buf));

    len = svcMsgRead(buf, sizeof(buf));
    if (len == 0) return -1;

    if (strstr((char *)buf, "+QMTCONN: ") != NULL) return 0;

    return -1;
}
