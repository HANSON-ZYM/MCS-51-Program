#include <REG51.H>
#include<stdio.h>
#define uchar unsigned char 
#define uint unsigned int
#define THC0 0xf8
#define TLC0 0x50   //2ms,0x30,含中断处理时间时，0x50

sbit lcden = P2 ^ 7; //液晶使能端
sbit lcdrs = P2 ^ 6; //液晶数0据命令选择端
sbit lcdrw = P2 ^ 5;
sbit IN1=P1 ^ 0;
sbit IN2=P1 ^ 1;
sbit IN3=P1 ^ 6;
sbit IN4=P1 ^ 7;
sbit AddSpeed=P3^0;		//转速加键
sbit SubSpeed=P3^1;		//转速减键
sbit PWML=P1^3;		//转速PWM调节端 1轮子
sbit PWMR=P1^4;		//转速PWM调节端 2轮子

unsigned char i=0;
int e ,e1 ,e2 ;//pid 偏差
float uk ,uk1 ,duk ;//pid输出值

int d ,d1 ,d2 ;//pid 偏差
float ck ,ck1 ,cuk ;//pid输出值

float Kp=2.2,Ki=0.1,Kd=0.0;//pid控制系数	10,12,1.5

int out=0,put=0;
uint SpeedSet=500;	   //转速定值
uint cnt=0;
uint RIMPULSE=0,LIMPULSE=0,LSPEED=0,RSPEED=0,zs;//脉冲计数
uint PWMLO=500,PWMRO=500;//定义初始PWM占空比50%

void delay1(uint z)
{
  uint i, j;
  for (i = z; i > 0; i--)
    for (j = 110; j > 0; j--);
}

void write_com(uchar com) //写命令
{
  lcdrs = 0; //选择写命令模式
  P0 = com; //将要写的命令字送到数据总线上
  delay1(5);//稍作延时以待数据稳定
  lcden = 1; //使能端给一高电平脉冲，因为初始化函数中已将lcden置零
  delay1(5);//稍作延时
  lcden = 0; //将使能端置零完成高脉冲
}

void write_data(uchar date)  //写数据
{
  lcdrs = 1; //选择写数据操作
  P0 = date; //将要写的数据送到数据总线上
  delay1(5);//稍作延时
  lcden = 1; //使能端给一高电平脉冲
  delay1(5);
  lcden = 0;
}

void init()   //1602初始化
{
  lcdrw = 0;
  lcden = 0;
  write_com(0x38);//设置16*2显示，5*7点阵，8位数据接口
  write_com(0x0c);//设置开显示，不显示光标
  write_com(0x06);//写一个字节后地址指针加1
  write_com(0x01);//显示清零，数据指针清零
  write_com(0x80);//设置数据指针起点
	
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


void PIDControlL()        //pid控制算法
{
	e=LSPEED-SpeedSet;		   //偏差
	duk=-(Kp*(e-e1)+Ki*e+Kd*(e-2*e1+e2));  
	uk=uk1+duk;
	out=(int)uk;
	if(out>1000)
	{
		out=990;	 //PWM占空比最大为99%
	}
	else if(out<0)
	{
		out=10;		 //PWM占空比最小为1%
	}
	uk1=uk;
	e2=e1;
	e1=e;
	PWMLO=out;
}

void PIDControlR()        //pid控制算法
{
	d=RSPEED-SpeedSet;		   //偏差
	cuk=-(Kp*(d-d1)+Ki*d+Kd*(d-2*d1+d2));  
	ck=ck1+cuk;
	put=(int)ck;
	if(put>1000)
	{
		put=990;	 //PWM占空比最大为99%
	}
	else if(put<0)
	{
		put=10;		 //PWM占空比最小为1%
	}
	ck1=ck;
	d2=d1;
	d1=d;
	PWMRO=put;
}

void PWMOUT()			 //输出PWM，进行电机调速
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
	TMOD=0X21;    //t0用来调频率 t2占空比
	TH0=THC0;
	TL0=TLC0;
	TH1=0xC0;
	TL1=0XC0;
	ET1=1;
	ET0=1;
	TR0=1;
	TR1=1;
	EX0=1;     //中断0用来测量转速
	IT0=1;
	EX1=1;     //中断0用来测量转速
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
void SetSpeed()			//按键处理程序
{
	if(AddSpeed==0)
	{
		delay1(10);//按键消抖
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
		delay1(10);//按键消抖
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
		  zs=0;//接显示
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
	LIMPULSE++;				//外部中断转速数据采样
}

void t0() interrupt 1
{
	static unsigned int time=0;

	TH0=THC0;
	TL0=TLC0;
	time++;  //转速测量周期
	if(time>250)			  //500毫秒计算一次转速
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
	RIMPULSE++;				//外部中断转速数据采样
}

void timer_1()  interrupt 3
{
   	cnt++;	//cnt越大占空比越高2.5Khz
}
