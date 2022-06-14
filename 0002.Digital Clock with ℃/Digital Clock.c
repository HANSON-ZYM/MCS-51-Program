#include <reg51.h>
#include <stdio.h>
#include <DS1302.h>
sbit DSPORT = P3 ^ 7;
unsigned char DATA[20];
long temp;
sbit lcden = P2 ^ 2; //液晶使能端
sbit lcdrs = P2 ^ 0; //液晶数0据命令选择端
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

void write_com(unsigned char com) //写命令
{
  lcdrs = 0; //选择写命令模式
  P0 = com; //将要写的命令字送到数据总线上
  delay1(5);//稍作延时以待数据稳定
  lcden = 1; //使能端给一高电平脉冲，因为初始化函数中已将lcden置零
  delay1(5);//稍作延时
  lcden = 0; //将使能端置零完成高脉冲
}

void write_data(unsigned char date)  //写数据
{
  lcdrs = 1; //选择写数据操作
  P0 = date; //将要写的数据送到数据总线上
  delay1(5);//稍作延时
  lcden = 1; //使能端给一高电平脉冲
  delay1(5);
  lcden = 0;
}


void LCDinit()   //1602初始化
{
  lcdrw = 0;
  lcden = 0;
  write_com(0x38);//设置16*2显示，5*7点阵，8位数据接口
  write_com(0x0c);//设置开显示，不显示光标
  write_com(0x06);//写一个字节后地址指针加1
  write_com(0x01);//显示清零，数据指针清零
  write_com(0x80);//设置数据指针起点
}
unsigned char Ds18b20Init()
{
  unsigned int i;
  DSPORT = 0;    //将总线拉低480us~960us
  i = 70;
  while (i--); //延时642us
  DSPORT = 1;   //然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低
  i = 0;
  while (DSPORT) //等待DS18B20拉低总线
  {
    i++;
    if (i > 5000) //等待>5MS
      return 0;//初始化失败
  }
  return 1;//初始化成功
}

void Ds18b20WriteByte(unsigned char dat)
{
  unsigned int i, j;
  for (j = 0; j < 8; j++)
  {
    DSPORT = 0;   //每写入一位数据之前先把总线拉低1us
    i++;
    DSPORT = dat & 0x01; //然后写入一个数据，从最低位开始
    i = 6;
    while (i--); //延时68us，持续时间最少60us
    DSPORT = 1; //然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
    dat >>= 1;
  }
}

unsigned char Ds18b20ReadByte()
{
  unsigned char byte, bi;
  unsigned int i, j;
  for (j = 8; j > 0; j--)
  {
    DSPORT = 0; //先将总线拉低1us
    i++;
    DSPORT = 1; //然后释放总线
    i++;
    i++;//延时6us等待数据稳定
    bi = DSPORT; //读取数据，从最低位开始读取
    /*将byte左移一位，然后与上右移7位后的bi，注意移动之后移掉那位补0。*/
    byte = (byte >> 1) | (bi << 7);
    i = 4;  //读取完之后等待48us再接着读取下一个数
    while (i--);
  }
  return byte;
}

void  Ds18b20ChangTemp()
{
  int i = 50;
  Ds18b20Init();
  Delay1ms(1);
  Ds18b20WriteByte(0xcc);   //跳过ROM操作命令
  Ds18b20WriteByte(0x44);   //温度转换命令
}

void  Ds18b20ReadTempCom()
{
  Ds18b20Init();
  Delay1ms(1);
  Ds18b20WriteByte(0xcc);  //跳过ROM操作命令
  Ds18b20WriteByte(0xbe);  //发送读取温度命令
}

int Ds18b20ReadTemp()
{
  int temp = 0;
  unsigned char tmh, tml;
  Ds18b20ChangTemp();       //先写入转换命令
  Ds18b20ReadTempCom();     //然后等待转换完后发送读取温度命令
  tml = Ds18b20ReadByte();  //读取温度值共16位，先读低字节
  tmh = Ds18b20ReadByte();  //再读高字节
  temp = tmh;
  temp <<= 8;
  temp |= tml;
  return temp;
}

void showT()
{
  write_com(0xD0);//设置数据指针起点
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
  write_com(0x80);//设置数据指针起点
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
	
	write_com(0xC0);//设置数据指针起点
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