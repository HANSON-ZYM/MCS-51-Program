#include <reg51.h>       
#include <intrins.h> 
#define uchar unsigned char  
#define uint unsigned int  
unsigned int t=0;

sbit lcden=P2^2;//Һ��ʹ�ܶ�
sbit lcdrs=P2^0;//Һ����0������ѡ���
sbit lcdrw=P2^1;
      
unsigned char start,num=0;  
unsigned char point,disp; 
unsigned int days,count;
unsigned char hyper,monthinv=0;
unsigned char code month[]={31,28,31,30,31,30,31,31,30,31,30,31,
                            31,29,31,30,31,30,31,31,30,31,30,31};
unsigned char timeinv[12]={0,0,0,0,0,0,0,0,0,0,0};
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
  TMOD = 0x09;
  TH0 = 0xFC;//1ms��ʱ
  TL0 = 0x22;
  ET0 = 1;
  TR0 = 1;
	IT0=1;
	EX0=1;
  EA = 1;           //�����ж�
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

void timeinvtrans()
{
	/*second*/
	timeinv[11]=time[5]*8+time[4]*4+time[3]*2+time[2];
	timeinv[10]=time[9]*4+time[8]*2+time[7];
	/*minute*/
	timeinv[9]=time[14]*8+time[13]*4+time[12]*2+time[11];
	timeinv[8]=time[18]*4+time[17]*2+time[16];
	/*hour*/
	timeinv[7]=time[24]*8+time[23]*4+time[22]*2+time[21];
	timeinv[6]=time[27]*2+time[26];
	/*day*/
	timeinv[5]=time[34]*8+time[33]*4+time[32]*2+time[31];
	timeinv[4]=time[39]*8+time[38]*4+time[37]*2+time[36];
	timeinv[3]=time[42]*2+time[41];
	/*year*/
	timeinv[1]=time[54]*8+time[53]*4+time[52]*2+time[51];
	timeinv[0]=time[59]*8+time[58]*4+time[57]*2+time[56];
}

void monthtrans()
{
	hyper=0;
	monthinv=0;
	if((2000+timeinv[0]*10+timeinv[1])%4==0)
	{
		hyper=1;
	}
	days=timeinv[3]*100+timeinv[4]*10+timeinv[5];
	while(days>month[monthinv+hyper*12])
	{
		days-=month[monthinv+hyper*12];
		monthinv++;
	}
	timeinv[2]=(1+monthinv)/10;
	timeinv[3]=(1+monthinv)%10;
	timeinv[4]=days/10;
	timeinv[5]=days%10;
}

void main()                 
{     	
	breakint();//��ʼ���ж�  
	init();
	delay1(200);
	point=0;
	while(1)  
	{  	
	  if(point==60)
		{
			timeinvtrans();
			monthtrans();
			write_com(0x80);
			write_data('2');
			write_data('0');
			write_com(0x82);
			for(disp=0;disp<6;disp++)
			{
				write_data(timeinv[disp]+48);
			}
			
			write_com(0xC0);
			write_data(timeinv[6]+48);
			write_data(timeinv[7]+48);
			write_data(':');
			write_data(timeinv[8]+48);
			write_data(timeinv[9]+48);
			write_data(':');
			write_data(timeinv[10]+48);
			write_data(timeinv[11]+48);
			point=0;
		}			
	}
}

void exint0() interrupt 0
{
  TH0 = 0xFC;//1ms��ʱ
  TL0 = 0x22;	
	if(count>4) time[point]=1;
	if(count<3) time[point]=0;
	point++;
	count=0;
}

void timer0() interrupt 1
{
  TH0 = 0xFC;//1ms��ʱ
  TL0 = 0x22;
  count++;
}