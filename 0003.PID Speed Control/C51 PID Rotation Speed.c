#include <REG51.H>
#include<stdio.h>
#define uchar unsigned char 
#define uint unsigned int
#define THC0 0xf8
#define TLC0 0x50   //2ms,0x30,���жϴ���ʱ��ʱ��0x50

sbit lcden = P2 ^ 7; //Һ��ʹ�ܶ�
sbit lcdrs = P2 ^ 6; //Һ����0������ѡ���
sbit lcdrw = P2 ^ 5;
sbit IN1=P1 ^ 0;
sbit IN2=P1 ^ 1;
sbit IN3=P1 ^ 6;
sbit IN4=P1 ^ 7;
sbit AddSpeed=P3^0;		//ת�ټӼ�
sbit SubSpeed=P3^1;		//ת�ټ���
sbit PWML=P1^3;		//ת��PWM���ڶ� 1����
sbit PWMR=P1^4;		//ת��PWM���ڶ� 2����

unsigned char i=0;
int e ,e1 ,e2 ;//pid ƫ��
float uk ,uk1 ,duk ;//pid���ֵ

int d ,d1 ,d2 ;//pid ƫ��
float ck ,ck1 ,cuk ;//pid���ֵ

float Kp=2.2,Ki=0.1,Kd=0.0;//pid����ϵ��	10,12,1.5

int out=0,put=0;
uint SpeedSet=500;	   //ת�ٶ�ֵ
uint cnt=0;
uint RIMPULSE=0,LIMPULSE=0,LSPEED=0,RSPEED=0,zs;//�������
uint PWMLO=500,PWMRO=500;//�����ʼPWMռ�ձ�50%

void delay1(uint z)
{
  uint i, j;
  for (i = z; i > 0; i--)
    for (j = 110; j > 0; j--);
}

void write_com(uchar com) //д����
{
  lcdrs = 0; //ѡ��д����ģʽ
  P0 = com; //��Ҫд���������͵�����������
  delay1(5);//������ʱ�Դ������ȶ�
  lcden = 1; //ʹ�ܶ˸�һ�ߵ�ƽ���壬��Ϊ��ʼ���������ѽ�lcden����
  delay1(5);//������ʱ
  lcden = 0; //��ʹ�ܶ�������ɸ�����
}

void write_data(uchar date)  //д����
{
  lcdrs = 1; //ѡ��д���ݲ���
  P0 = date; //��Ҫд�������͵�����������
  delay1(5);//������ʱ
  lcden = 1; //ʹ�ܶ˸�һ�ߵ�ƽ����
  delay1(5);
  lcden = 0;
}

void init()   //1602��ʼ��
{
  lcdrw = 0;
  lcden = 0;
  write_com(0x38);//����16*2��ʾ��5*7����8λ���ݽӿ�
  write_com(0x0c);//���ÿ���ʾ������ʾ���
  write_com(0x06);//дһ���ֽں��ַָ���1
  write_com(0x01);//��ʾ���㣬����ָ������
  write_com(0x80);//��������ָ�����
	
	 write_com(0x80);
	 write_data('R');
	 write_data('E');
	 write_data('A');
	 write_data('L');
	 write_data(' ');
	 write_data('S');
	 write_data('E');
	 write_data('T');
	 write_data(' ');
	 write_data('R');
	 write_data('E');
	 write_data('A');
	 write_data('L');
}


void PIDControlL()        //pid�����㷨
{
	e=LSPEED-SpeedSet;		   //ƫ��
	duk=-(Kp*(e-e1)+Ki*e+Kd*(e-2*e1+e2));  
	uk=uk1+duk;
	out=(int)uk;
	if(out>1000)
	{
		out=990;	 //PWMռ�ձ����Ϊ99%
	}
	else if(out<0)
	{
		out=10;		 //PWMռ�ձ���СΪ1%
	}
	uk1=uk;
	e2=e1;
	e1=e;
	PWMLO=out;
}

void PIDControlR()        //pid�����㷨
{
	d=RSPEED-SpeedSet;		   //ƫ��
	cuk=-(Kp*(d-d1)+Ki*d+Kd*(d-2*d1+d2));  
	ck=ck1+cuk;
	put=(int)ck;
	if(put>1000)
	{
		put=990;	 //PWMռ�ձ����Ϊ99%
	}
	else if(put<0)
	{
		put=10;		 //PWMռ�ձ���СΪ1%
	}
	ck1=ck;
	d2=d1;
	d1=d;
	PWMRO=put;
}

void PWMOUT()			 //���PWM�����е������
{
	  if(cnt<PWMLO)
	  {
		  PWML=1;
	  }
	  if(cnt<PWMRO)
	  {
		  PWMR=1;	
	  }
	  if(cnt>=PWMLO)
	  {
		  PWML=0;
	  }
	  if(cnt>=PWMRO)
	  {
		  PWMR=0;
	  }
	if(cnt>1000) cnt=0;
}


void SystemInit()
{
	TMOD=0X21;    //t0������Ƶ�� t2ռ�ձ�
	TH0=THC0;
	TL0=TLC0;
	TH1=0xC0;
	TL1=0XC0;
	ET1=1;
	ET0=1;
	TR0=1;
	TR1=1;
	EX0=1;     //�ж�0��������ת��
	IT0=1;
	EX1=1;     //�ж�0��������ת��
	IT1=1;
	EA=1;
	e =0;
	e1=0;
	e2=0;
	d =0;
	d1=0;
	d2=0;
	IN1=1;
	IN2=0;
	IN3=1;
	IN4=0;
}
void SetSpeed()			//�����������
{
	if(AddSpeed==0)
	{
		delay1(10);//��������
		if(AddSpeed==0)
		{
			SpeedSet+=10;
			if(SpeedSet>1000)
			{
				SpeedSet=1000;
			}
		}
	}
	if(SubSpeed==0)
	{
		delay1(10);//��������
		if(SubSpeed==0)
		{
			SpeedSet-=10;
			if(SpeedSet<0) SpeedSet=0;
		}
	}	
}

void main()
{
	SystemInit();
	init();
	zs=1;
	while(1)
	{
		SetSpeed();
		if(zs==1)
		{
		  zs=0;//����ʾ
	    write_com(0xc0);
      write_data('0'+LSPEED%1000/100);	
      write_data('0'+LSPEED%100/10);	
      write_data('0'+LSPEED%10);
      write_data(' ');
      write_data(' ');	    
      write_data('0'+SpeedSet%1000/100);	
      write_data('0'+SpeedSet%100/10);	
      write_data('0'+SpeedSet%10);
			write_data(' ');
			write_data(' ');
			write_data('0'+RSPEED%1000/100);	
      write_data('0'+RSPEED%100/10);	
      write_data('0'+RSPEED%10);
	  }
  }
}

void int0() interrupt 0
{
	LIMPULSE++;				//�ⲿ�ж�ת�����ݲ���
}

void t0() interrupt 1
{
	static unsigned int time=0;

	TH0=THC0;
	TL0=TLC0;
	time++;  //ת�ٲ�������
	if(time>250)			  //500�������һ��ת��
	{
		zs=1;
		time=0;
		LSPEED=LIMPULSE*120/11;
		RSPEED=RIMPULSE*120/11;
		LIMPULSE=0;
		RIMPULSE=0;
		PIDControlL();
		PIDControlR();
	}
	PWMOUT();
}

void int1() interrupt 2
{
	RIMPULSE++;				//�ⲿ�ж�ת�����ݲ���
}

void timer_1()  interrupt 3
{
   	cnt++;	//cntԽ��ռ�ձ�Խ��2.5Khz
}
