#include <reg51.h>       
#include <intrins.h> 
#define uchar unsigned char  
#define uint unsigned int  
unsigned int t=0;

sbit lcden=P2^2;//Һ��ʹ�ܶ�
sbit lcdrs=P2^0;//Һ����0������ѡ���
sbit lcdrw=P2^1;
sbit IRIG=P2^3; //ʱ���ź����

unsigned char m=0,q,deter,receive[17];  //receive--20220123A120523  20220514A123654           
unsigned char start,num=0;  
unsigned char i,j,k,days,hyper=0; 
unsigned int count;
unsigned char code month[]={0,31,28,31,30,31,30,31,31,30,31,30,31,
                            0,31,29,31,30,31,30,31,31,30,31,30,31};
bit send;
unsigned char time[]={8,//P0
	                    8,//Pr
	
                      0,0,0,0,//��1248  time[2-5]
	                    2,// empty
	                    0,0,0,//�� 10 20 40 time[7-9]      
	                    8,//P1
	
	                    0,0,0,0,//��1248     time[11-14]
	                    2,// empty
	                    0,0,0,//�� 10 20 40  time[16-18]
	                    2,// empty
	                    8,//P2

	                    0,0,0,0,//ʱ1248  time[21-24]
	                    2,// empty
	                    0,0,//ʱ�� 10 20  time[26-27]
	                    2,2,// empty
	                    8,//P3

	                    0,0,0,0,//��1248        time[31-34]
	                    2,// empty
	                    0,0,0,0,//�� 10 20 40 80 time[36-39]
	                    8,//P4

	                    0,0,//��100 200  time[41-42]
	                    2,2,2,2,2,2,2,// empty
	                    8,//P5

	                    0,0,0,0,//��1248        time[51-54]
	                    2,// empty
	                    0,0,0,0,//�� 10 20 40 80 time[56-59]
	                    8,//P6
                     };

void breakint()
{
  PCON &= 0x7F;   //        ���ò����ʲ��ӱ�
  TMOD = 0x21;  //���ö�ʱ��1������ʽ 2 ��ʱ��0 ��ʽ1
  TH1 = 0xfd;
  TL1 = 0xfd;         //�趨��ʱ��1������ʽ2�ĳ�ֵ
  TR1 = 1;     //����ʱ��1,���������ʷ�����
  TH0 = 0xFC;//1ms��ʱ
  TL0 = 0x67;
  ET0 = 1;
  TR0 = 0;
  SM0 = 0;
  SM1 = 1;  //�趨����ͨ�Ź�����ʽ1
  REN = 1;
  ES = 1;
  EA = 1;           //�����ж�
  q = 0;
  deter = 0;
}
//************************//
//*******1602��ʱ*********//
//************************// 
void delay1(uint z)
{
	uint i,j;
	for(i=z;i>0;i--)
	for(j=110;j>0;j--);
}
//************************//
//******1602�ӳ�**********//
//************************// 
void write_com(uchar com) //д���� 
{
	lcdrs=0;//ѡ��д����ģʽ
	P0=com;//��Ҫд���������͵�����������
	delay1(5);//������ʱ�Դ������ȶ�
	lcden=1;//ʹ�ܶ˸�һ�ߵ�ƽ���壬��Ϊ��ʼ���������ѽ�lcden����
	delay1(5);//������ʱ
	lcden=0;//��ʹ�ܶ�������ɸ�����
}

void write_data(uchar date)  //д���� 
{
	lcdrs=1;//ѡ��д���ݲ���
	P0=date;//��Ҫд�������͵�����������
	delay1(5);//������ʱ
	lcden=1;//ʹ�ܶ˸�һ�ߵ�ƽ����
	delay1(5);
	lcden=0;
}

void init()   //1602��ʼ�� 
{
	lcdrw=0;
	lcden=0;
	write_com(0x38);//����16*2��ʾ��5*7����8λ���ݽӿ�
	write_com(0x0c);//���ÿ���ʾ������ʾ���
	write_com(0x06);//дһ���ֽں��ַָ���1
	write_com(0x01);//��ʾ���㣬����ָ������
}

void timetrans()
{
	/*second*/
	time[5]=2+3*((receive[15]-48)/8);
	time[4]=2+3*((receive[15]-48)%8/4);
	time[3]=2+3*((receive[15]-48)%4/2);
	time[2]=2+3*((receive[15]-48)%2);
	
	time[9]=2+3*((receive[14]-48)/4);
	time[8]=2+3*((receive[14]-48)%4/2);
	time[7]=2+3*((receive[14]-48)%2);
	
	/*minute*/
	time[14]=2+3*((receive[13]-48)/8);
	time[13]=2+3*((receive[13]-48)%8/4);
	time[12]=2+3*((receive[13]-48)%4/2);
	time[11]=2+3*((receive[13]-48)%2);
	
	time[18]=2+3*((receive[12]-48)/4);
	time[17]=2+3*((receive[12]-48)%4/2);
	time[16]=2+3*((receive[12]-48)%2);

	/*hour*/
	time[24]=2+3*((receive[11]-48)/8);
	time[23]=2+3*((receive[11]-48)%8/4);
	time[22]=2+3*((receive[11]-48)%4/2);
	time[21]=2+3*((receive[11]-48)%2);
	
	time[27]=2+3*((receive[10]-48)/2);
	time[26]=2+3*((receive[10]-48)%2);

	/*day*/
	time[34]=2+3*((receive[8]-48)/8);
	time[33]=2+3*((receive[8]-48)%8/4);
	time[32]=2+3*((receive[8]-48)%4/2);
	time[31]=2+3*((receive[8]-48)%2);
	
	time[39]=2+3*((receive[7]-48)/8);
	time[38]=2+3*((receive[7]-48)%8/4);
	time[37]=2+3*((receive[7]-48)%4/2);
	time[36]=2+3*((receive[7]-48)%2);
	
	time[42]=2+3*((receive[6]-48)/2);
	time[41]=2+3*((receive[6]-48)%2);
	
	/*year*/
	time[54]=2+3*((receive[4]-48)/8);
	time[53]=2+3*((receive[4]-48)%8/4);
	time[52]=2+3*((receive[4]-48)%4/2);
	time[51]=2+3*((receive[4]-48)%2);
	
	time[59]=2+3*((receive[3]-48)/8);
	time[58]=2+3*((receive[3]-48)%8/4);
	time[57]=2+3*((receive[3]-48)%4/2);
	time[56]=2+3*((receive[3]-48)%2);	
}

void main()                 
{     	
	breakint();//��ʼ���ж�  
	init();
	i=0;
	send=0;
	hyper=0;
	while(1)  
	{  	
		if(send==1)
		{
		  if(time[i]>count)
		  {
			  IRIG=1;
		  }
		  else
		  {
			  IRIG=0;
		  }
			if(i==60)
			{
				TR0=0;
				send=0;
				i=0;
			}
		}
		if (deter == 2) 
    {
      ES = 0;
			deter = 0;
      q = 0;
			days=0;
			send=1;
			if(((receive[1]-48)*1000+(receive[2]-48)*100+(receive[3]-48)*10+(receive[4]-48))%4==0)
			{
				hyper=1;
			}
			else hyper=0;
			for(k=0;k<((receive[5]-48)*10+receive[6]-48);k++)
      {
				days+=month[k+13*hyper];
      }
      days+=(receive[7]-48)*10+receive[8]-48;
			receive[5]=0+48;
			receive[6]=48+days/100;
			receive[7]=48+days%100/10;
			receive[8]=48+days%10;
			timetrans();
			///*
			write_com(0x80);
			for(j=1;j<16;j++)
			{
				write_data(receive[j]);
				//write_data(48+time[j]);
			}
			//*/
			start=0;
			ES=1;
			TR0=1;
		}
	}
}

void timer0() interrupt 1
{
  TH0 = 0xFC;//1ms��ʱ
  TL0 = 0x67;
  count++;
	if(count==10)
	{
		i++;
		count=0;
	}
}

void RX() interrupt 4
{
  if (RI == 1)
  {
    num = SBUF;
    if (num == 35)
    {
      deter++;
			start = 1;
    }
		if(start==1)
		{
      receive[q] = num;
			q++;
		}
		SBUF=0;
    RI = 0;
  }
}