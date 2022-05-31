#include "tim2.h"

void NVIC_config(void)
{
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//���ȼ�����
     
     NVIC_InitTypeDef nvic;
     nvic.NVIC_IRQChannel=TIM2_IRQn;
     nvic.NVIC_IRQChannelCmd=ENABLE;
      nvic.NVIC_IRQChannelPreemptionPriority=0;
     nvic.NVIC_IRQChannelSubPriority=0;
     NVIC_Init(&nvic);
}

void time2_config(void)
{
     TIM_TimeBaseInitTypeDef tim;//�ṹ��
     NVIC_config();//���ȼ���������
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//����ʱ��
     TIM_DeInit(TIM2);//����������
     tim.TIM_ClockDivision=TIM_CKD_DIV1;//������Ƶ
    tim.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���
     tim.TIM_Period=4999;//�Զ���װ�ؼĴ�����ֵ
     tim.TIM_Prescaler=7199;//ʱ��Ԥ��Ƶ
     //tim.TIM_RepetitionCounter=
     TIM_TimeBaseInit(TIM2,&tim);//��ʼ���ṹ��
     TIM_ClearFlag(TIM2,TIM_FLAG_Update);//�������жϱ�־
     TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
     TIM_Cmd(TIM2,ENABLE);//����ʱ��
 }

void TIM2_IRQHandler(void)
{
     if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//�ж��жϱ�־�Ƿ���
     {
			  gpstime++;
			  HeartRatetime++;
     }
     TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);    
}
