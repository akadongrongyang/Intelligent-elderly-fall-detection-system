//ͷ�ļ�
#include "all.h"

unsigned short timeCount = 0;	//���ͼ������
unsigned short pingCount = 0;	//���ͼ������
unsigned char *dataPtr = NULL;

void OLED_Show(void);

char temp_data[16];
u8 fall_flag=0;    //�������ͱ�־λ
float pitch,roll,yaw; 	//ŷ����
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
char Buf[30] = {0};
u8 mpu_flag = 0;      				//�����������־λ ��־λ1��2����һ���쳣 �ñ�־λΪ1 	��1��������0��������
_Bool mpu_1_flag = 0;      		//�����������־λ1	�Ƕ��쳣��־		��1���쳣��0��������
_Bool mpu_2_flag = 0;      		//�����������־λ2	���ٶ��쳣��־	��1���쳣��0��������
int SVM;
u8 t=0,i=10,t1=10;

u32 HeartRatetime=0;
u32 gpstime=0;
float temp=0;
signed short zitai; 
signed short HeartRate_val; //��������
u8 SPO2_val;							//Ѫ��Ũ��
void GPRS_Init()
{
	
}
 int main(void)
{	
	SystemInit();	// ����ϵͳʱ��Ϊ72M 
  delay_init();    //��ʱ��ʼ��	
	GPS_USART2_Init(9600);
	UART1_Init();;
  MPU_Init();					//��ʼ��MPU6050
	delay_ms(300);			//�ȴ���ʼ���ȶ�
	while(mpu_dmp_init())
 	{
		printf("error");
 		delay_ms(300);
	}	
	TIM3_Int_Init(100-1,720-1);
	time2_config();
	OLED_Init();
	OLED_Show();
  //IIC_Init();					//����Ѫ��Ũ��ģ��IIC-----SCLK�ӵ���B8���� SDIN�ӵ���B9����
//	SPO2_Init();
	Ds18b20_Init();//��ʼ��DS18B20
		OLED_ShowString(32,0,(u8*)":");
	printf("Hardware init OK\r\n");
	M6312_Init();					//��ʼ��SIM800C
	while(OneNet_DevLink())			//����OneNET
	delay_ms(500);
 	while(1)  
	{
		
		  POupdate();	
	  if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
			{ 
				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������		
				SVM = sqrt(pow(aacx,2)+  pow(aacy,2) + pow(aacz,2));	
			//	printf2("%.1f",roll);
			//	printf2("%d",SVM);
				//����x��y��z�Ƕȵ��쳣�ж�
				if( fabs(pitch)>50 || fabs(roll)>50 || fabs(yaw)>50 )//��б�Ƕȵ� ������ֵ�� ����40��SVM�����趨����ֵʱ������Ϊˤ��
					mpu_1_flag = 1;	
				else 
					mpu_1_flag = 0;
				
				//�������ٶ�SVM���쳣�ж�
				if( SVM>23000 || SVM<12000 )i = 0;
				i++;
				
				if( i<=10 )mpu_2_flag = 1;
				else 
				{
					i = 10;
					mpu_2_flag = 0;
				}
		
				//�ۺ�ŷ���ǡ����ٶ�SVM�쳣�ж��쳣	
				if( mpu_2_flag || mpu_1_flag )mpu_flag = 1;
				else mpu_flag = 0;
				
			if(mpu_flag==1){
					t1=0;
					
				  zitai=1;
					GPIO_ResetBits(GPIOC,GPIO_Pin_0);	
				}
				else{
					zitai=0;
					
					GPIO_SetBits(GPIOC,GPIO_Pin_0);
				}
			t=0;
		}
	 
		t++;
		temp=(Ds18b20ReadTemp()*0.0625);   //��ȡ�����¶�
		if(gpstime>=5)
		{
		  OLED_ShowNum(40,0,HeartRate_val,2,16);
      OLED_ShowNum(104,0,SPO2_val,2,16);			
			
			
	 if(zitai==1)
	 {
		// printf("ˤ��");
          OLED_ShowString(72,6,(u8*)"     ");
					OLED_ShowCHinese(72,6,3);
	        OLED_ShowCHinese(72+16,6,4);
	 }
	 else
	 {
	//	 printf("����");
					OLED_ShowString(72,6,(u8*)"     ");
					OLED_ShowCHinese(72,6,5);
	        OLED_ShowCHinese(72+16,6,6);
	 }
			memset(temp_data,0x00,16);
			sprintf(temp_data,"%.2f",temp);
			
			OLED_ShowString(40,3,(u8*)temp_data);
			printf("%.2f",temp);
			gpstime=0;
			OneNet_SendData();									//��������
			M6312_Clear();
	  	dataPtr = M6312_GetIPD(0);
		  if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);	
		}
 }
}

void OLED_Show()
{
	OLED_ShowCHinese(0,0,12);
	OLED_ShowCHinese(16,0,13);
	OLED_ShowString(32,0,(u8*)":");
  OLED_ShowCHinese(64,0,14);
	OLED_ShowCHinese(64+16,0,15);
	OLED_ShowString(80+16,0,(u8*)":");
	
	OLED_ShowCHinese(0,3,7);
	OLED_ShowCHinese(16,3,8);
	OLED_ShowCHinese(80,3,11);
	 OLED_ShowString(32,3,(u8*)":");
	
  OLED_ShowCHinese(0,6,3);
	OLED_ShowCHinese(16,6,4);
	OLED_ShowCHinese(32,6,9);
	OLED_ShowCHinese(48,6,10);
  OLED_ShowString(64,6,(u8*)":");
}





