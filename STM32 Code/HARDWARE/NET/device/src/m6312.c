/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	m6312.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-10-20
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		M6312����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "m6312.h"

//Ӳ������
#include "delay.h"
#include "uart.h"

//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define M6312_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"


unsigned char m6312_buf[256];
unsigned short m6312_cnt = 0, m6312_cntPre = 0;


//==========================================================
//	�������ƣ�	M6312_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void M6312_Clear(void)
{

	memset(m6312_buf, 0, sizeof(m6312_buf));
	m6312_cnt = 0;

}

//==========================================================
//	�������ƣ�	M6312_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool M6312_WaitRecive(void)
{

	if(m6312_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(m6312_cnt == m6312_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		m6312_cnt = 0;							//��0���ռ���
			
		return REV_OK;							//���ؽ�����ɱ�־
	}
		
	m6312_cntPre = m6312_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;							//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	M6312_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool M6312_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART1, (unsigned char *)cmd, strlen((const char *)cmd));

	while(timeOut--)
	{
		if(M6312_WaitRecive() == REV_OK)							//����յ�����
		{
				printf("%s",m6312_buf);
			if(strstr((const char *)m6312_buf, res) != NULL)		//����������ؼ���
			{
				M6312_Clear();										//��ջ���
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	M6312_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void M6312_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	M6312_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);	//��������
	if(!M6312_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART1, data, len);	//�����豸������������
	}

}

//==========================================================
//	�������ƣ�	M6312_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	timeOut�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//==========================================================
unsigned char *M6312_GetIPD(unsigned short timeOut)
{

	char *ptrIPD;
	
	do
	{
		if(M6312_WaitRecive() == REV_OK)								//����������
		{
				//ptrIPD = strstr((char *)m6312_buf, "+IPD,");				//������IPDATA��ͷ
			ptrIPD = strstr((char *)m6312_buf, "IPD,");				//������IPDATA��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDATAͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				//ptrIPD = strchr(ptrIPD, '\n');							//�ҵ�'\n'
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
		
		delay_ms(10);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	M6312_Init
//
//	�������ܣ�	��ʼ��M6312
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void M6312_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

	//M6312��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_15;				//GPIOC15-��λ
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	//M6312��Դ����
	GPIO_Initure.GPIO_Pin = GPIO_Pin_4;					//GPIOC4-PowerKey
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	//M6312״̬����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_5;					//GPIOC5-status
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET);			//�ϵ�
	
//	while(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5))
//		delay_ms(10);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_SET);			//��λ
	delay_ms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_RESET);
	delay_ms(500);
	
	M6312_Clear();

	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(M6312_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r\n");
	while(M6312_SendCmd("AT+CPIN?\r\n", "+CPIN: READY"))		//ȷ��SIM��PIN�����������READY����ʾ�����ɹ�
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. AT+CREG?\r\n");
	while(M6312_SendCmd("AT+CREG?\r\n", "0,1"))  //0,1
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. AT+CGREG?\r\n");
//	while(M6312_SendCmd("AT+CGREG?\r\n","OK"))					//�������ע��״̬
	while(M6312_SendCmd("AT+CGREG?\r\n","0,1"))					//�������ע��״̬
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. AT+CIPSHUT\r\n");
	while(M6312_SendCmd("AT+CIPSHUT\r\n","SHUT OK"))				//�ر��ƶ����� 
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "6. AT+CGCLASS=\"B\"\r\n");
	while(M6312_SendCmd("AT+CGCLASS=\"B\"\r\n","OK"))				//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ���
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "7. AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
	while(M6312_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK"))				//����PDP������,��������Э��,��������Ϣ
		delay_ms(500);	
	
	
	UsartPrintf(USART_DEBUG, "8. AT+CGATT=1\r\n");
	while(M6312_SendCmd("AT+CGATT=1\r\n","OK"))					//����GPRSҵ��
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "9. AT+CIPMUX=0\r\n");
	while(M6312_SendCmd("AT+CIPMUX=0\r\n","OK"))					//����Ϊ�����ӣ���Ȼƽ̨IP��������
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "10. AT+CIPHEAD=1\r\n");
	while(M6312_SendCmd("AT+CIPHEAD=1\r\n","OK"))				//��ʾIPͷ
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "11. AT+CIPSTART\r\n");
	while(M6312_SendCmd(M6312_ONENET_INFO,"CONNECT"))				//
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "12. M6312 Init OK\r\n");

}

//==========================================================
//	�������ƣ�	USART1_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(m6312_cnt >= sizeof(m6312_buf))	m6312_cnt = 0; //��ֹ���ڱ�ˢ��
		m6312_buf[m6312_cnt++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}

}

