/******************** *************************

 * ˵���UUART2
 * Ӳ������: TX->PA2;RX->PA3

**********************************************/	

#include "stdio.h"	
#include "sys.h" 
#include "stdarg.h"	 	 	 
#include "string.h"	


/****************                       ******   */


/************************************/
//�������鳤��
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2 
#define false 0
#define true 1
#define USART_REC_LEN  			200  	//�����������ֽ��� 200	

#define CMD_BUFFER_LEN2 255
/*******************************/

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		//�Ƿ��ȡ��GPS����
	char isParseData;	//�Ƿ�������
	char UTCTime[UTCTime_Length];		//UTCʱ��
	char latitude[latitude_Length];		//γ��
	char N_S[N_S_Length];		//N/S
	char longitude[longitude_Length];		//����
	char E_W[E_W_Length];		//E/W
	char isUsefull;		//��λ��Ϣ�Ƿ���Ч
} _SaveData;
/********************************/

extern char rxdatabufer;
extern u16 point1;
extern char USART2_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern _SaveData Save_Data;



/****************������ʼ��**********************/
u8 Hand(char *a);
void clrStruct(void);
void CLR_Buf(void);
void GPS_USART2_Init(u32 My_BaudRate);
void UART2_Send_Str(unsigned char *s);
void errorLog(int num);
void parseGpsBuffer(void);
void printGpsBuffer(void);
void printf2(char *fmt, ...) ;
