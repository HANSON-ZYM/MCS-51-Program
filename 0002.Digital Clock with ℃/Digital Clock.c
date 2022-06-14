#include <reg51.h>
#include <stdio.h>
#include <DS1302.h>
sbit DSPORT = P3 ^ 7;
unsigned char DATA[20];
long temp;
sbit lcden = P2 ^ 2; //Һ��ʹ�ܶ�
sbit lcdrs = P2 ^ 0; //Һ����0������ѡ���
sbit lcdrw = P2 ^ 1;
sbit Buzz = P1 ^ 7;
unsigned int i,j,k;
unsigned int time1,time2,time,oldtime;
void Delay1ms(unsigned int y)
{
  unsigned int x;
  for (y; y > 0; y--)
    for (x = 110; x > 0; x--);
}

void delay1(unsigned int z)
{
  unsigned int i, j;
  for (i = z; i > 0; i--)
    for (j = 110; j > 0; j--);
}

void write_com(unsigned char com) //д����
{
  lcdrs = 0; //ѡ��д����ģʽ
  P0 = com; //��Ҫд���������͵�����������
  delay1(5);//������ʱ�Դ������ȶ�
  lcden = 1; //ʹ�ܶ˸�һ�ߵ�ƽ���壬��Ϊ��ʼ���������ѽ�lcden����
  delay1(5);//������ʱ
  lcden = 0; //��ʹ�ܶ�������ɸ�����
}

void write_data(unsigned char date)  //д����
{
  lcdrs = 1; //ѡ��д���ݲ���
  P0 = date; //��Ҫд�������͵�����������
  delay1(5);//������ʱ
  lcden = 1; //ʹ�ܶ˸�һ�ߵ�ƽ����
  delay1(5);
  lcden = 0;
}


void LCDinit()   //1602��ʼ��
{
  lcdrw = 0;
  lcden = 0;
  write_com(0x38);//����16*2��ʾ��5*7����8λ���ݽӿ�
  write_com(0x0c);//���ÿ���ʾ������ʾ���
  write_com(0x06);//дһ���ֽں��ַָ���1
  write_com(0x01);//��ʾ���㣬����ָ������
  write_com(0x80);//��������ָ�����
}
unsigned char Ds18b20Init()
{
  unsigned int i;
  DSPORT = 0;    //����������480us~960us
  i = 70;
  while (i--); //��ʱ642us
  DSPORT = 1;   //Ȼ���������ߣ����DS18B20������Ӧ�Ὣ��15us~60us����������
  i = 0;
  while (DSPORT) //�ȴ�DS18B20��������
  {
    i++;
    if (i > 5000) //�ȴ�>5MS
      return 0;//��ʼ��ʧ��
  }
  return 1;//��ʼ���ɹ�
}

void Ds18b20WriteByte(unsigned char dat)
{
  unsigned int i, j;
  for (j = 0; j < 8; j++)
  {
    DSPORT = 0;   //ÿд��һλ����֮ǰ�Ȱ���������1us
    i++;
    DSPORT = dat & 0x01; //Ȼ��д��һ�����ݣ������λ��ʼ
    i = 6;
    while (i--); //��ʱ68us������ʱ������60us
    DSPORT = 1; //Ȼ���ͷ����ߣ�����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
    dat >>= 1;
  }
}

unsigned char Ds18b20ReadByte()
{
  unsigned char byte, bi;
  unsigned int i, j;
  for (j = 8; j > 0; j--)
  {
    DSPORT = 0; //�Ƚ���������1us
    i++;
    DSPORT = 1; //Ȼ���ͷ�����
    i++;
    i++;//��ʱ6us�ȴ������ȶ�
    bi = DSPORT; //��ȡ���ݣ������λ��ʼ��ȡ
    /*��byte����һλ��Ȼ����������7λ���bi��ע���ƶ�֮���Ƶ���λ��0��*/
    byte = (byte >> 1) | (bi << 7);
    i = 4;  //��ȡ��֮��ȴ�48us�ٽ��Ŷ�ȡ��һ����
    while (i--);
  }
  return byte;
}

void  Ds18b20ChangTemp()
{
  int i = 50;
  Ds18b20Init();
  Delay1ms(1);
  Ds18b20WriteByte(0xcc);   //����ROM��������
  Ds18b20WriteByte(0x44);   //�¶�ת������
}

void  Ds18b20ReadTempCom()
{
  Ds18b20Init();
  Delay1ms(1);
  Ds18b20WriteByte(0xcc);  //����ROM��������
  Ds18b20WriteByte(0xbe);  //���Ͷ�ȡ�¶�����
}

int Ds18b20ReadTemp()
{
  int temp = 0;
  unsigned char tmh, tml;
  Ds18b20ChangTemp();       //��д��ת������
  Ds18b20ReadTempCom();     //Ȼ��ȴ�ת������Ͷ�ȡ�¶�����
  tml = Ds18b20ReadByte();  //��ȡ�¶�ֵ��16λ���ȶ����ֽ�
  tmh = Ds18b20ReadByte();  //�ٶ����ֽ�
  temp = tmh;
  temp <<= 8;
  temp |= tml;
  return temp;
}

void showT()
{
  write_com(0xD0);//��������ָ�����
  write_data(DATA[0]);
  write_data(DATA[1]);
  write_data(DATA[2]);
	write_data('.');
  write_data(DATA[3]);
	write_data(DATA[4]);
	write_data(0xDF);
	write_data('C');
	write_com(0xDC);
	write_data('T');
	write_data('E');
	write_data('M');
	write_data('P');
}

void Showtime()
{
  write_com(0x80);//��������ָ�����
  write_data('0'+time_buf1[0]/10);
  write_data('0'+time_buf1[0]%10);
  write_data('0'+time_buf1[1]/10);
  write_data('0'+time_buf1[1]%10);
	write_data('.');
  write_data('0'+time_buf1[2]/10);
  write_data('0'+time_buf1[2]%10);
	write_data('.');
  write_data('0'+time_buf1[3]/10);
  write_data('0'+time_buf1[3]%10);
	write_com(0x8C);
	write_data('D');
	write_data('A');
	write_data('T');
	write_data('E');	
	
	write_com(0xC0);//��������ָ�����
  write_data('0'+time_buf1[4]/10);
  write_data('0'+time_buf1[4]%10);
	write_data(':');
  write_data('0'+time_buf1[5]/10);
  write_data('0'+time_buf1[5]%10);
	write_data(':');
  write_data('0'+time_buf1[6]/10);
  write_data('0'+time_buf1[6]%10);
	write_com(0xCC);
	write_data('T');
	write_data('I');
	write_data('M');
	write_data('E');	
	
	write_com(0x90);
	switch(time_buf1[7])
	{
		case 1:
		{
			write_data('S');
	    write_data('U');
	    write_data('N');
			break;
		}
		case 2:
		{
			write_data('M');
	    write_data('O');
	    write_data('N');
			break;
		}
		case 3:
		{
			write_data('T');
	    write_data('U');
	    write_data('E');
			break;
		}
		case 4:
		{
			write_data('W');
	    write_data('E');
	    write_data('D');
			break;
		}
		case 5:
		{
			write_data('T');
	    write_data('H');
	    write_data('U');
			break;
		}
		case 6:
		{
			write_data('F');
	    write_data('R');
	    write_data('I');
			break;
		}
		case 7:
		{
			write_data('S');
	    write_data('A');
	    write_data('T');
			break;
		}
	}
	write_com(0x9C);
	write_data('W');
	write_data('E');
	write_data('E');
	write_data('K');
}


void main(void)
{
	  LCDinit();
	  Ds1302_Read_Time();
	  oldtime=time_buf1[4];
    while(1)
    {
			temp = Ds18b20ReadTemp() * 0.0625 * 100 + 0.5;  
			DATA[0]='0'+temp / 10000;
			DATA[1]='0'+temp % 10000 / 1000;
			DATA[2]='0'+temp % 1000 / 100;
			DATA[3]='0'+temp % 100 / 10;
			DATA[4]='0'+temp  % 10;
			Ds1302_Read_Time();
			Showtime();
			showT();
			if(time_buf1[4]!=oldtime)
			{
				Buzz=1;
			}
			else
			{
				Buzz=0;
			}
			oldtime=time_buf1[4];
    }
}