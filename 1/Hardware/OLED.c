#include "oled.h"
#include "OLED_Font.h"
#include "delay.h"


#include "oled.h"
//#include "oledfont.h"  
#include "delay.h"

//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 

	void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		//OLED_Set_Pos(i,0);
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
	}
} 
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}



//参数规则  汉字为16*16，所以x值为（前方字符数-1）*16的值，Y为（字符上方行数）*2
//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);
	//OLED_Set_Pos((x-1)*2,(y-1)*8);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);
	//OLED_Set_Pos((x-1)*2+1,(y-1)*8);	
		//OLED_Set_Pos(x,y+1);	//这起始点定义有问题
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;//这个有啥用？？？
      }					
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 


//初始化SSD1306					    
void OLED_Init(void)
{ 	

  GPIO_InitTypeDef GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟使能


 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13;	//PB8,PB9,PB12,PB13 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB4
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13);//PB8,PB9,PB12,PB13 输出高
	delay_ms(100);

	OLED_WR_Byte(0xAE,OLED_CMD);//--关闭oled面板
	OLED_WR_Byte(0x00,OLED_CMD);//---设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);//---设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);//--设置起始行地址设置映射RAM显示起始行（0x00 ~ 0x3F）
	OLED_WR_Byte(0x81,OLED_CMD);//--设置对比度控制寄存器
	OLED_WR_Byte(0xCF,OLED_CMD); // 设置SEG输出电流亮度
	OLED_WR_Byte(0xA1,OLED_CMD);//--设置SEG/列映射     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//设置COM/行扫描方向   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--设置正常显示
	OLED_WR_Byte(0xA8,OLED_CMD);//--设置多路复用比(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 责任
	OLED_WR_Byte(0xD3,OLED_CMD);//-设置显示偏移移位映射RAM计数器 (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-不偏移
	OLED_WR_Byte(0xd5,OLED_CMD);//--设置显示时钟分频比/振荡器频率
	OLED_WR_Byte(0xf0,OLED_CMD);//--设置分频比，将时钟设置为100帧/秒
	OLED_WR_Byte(0xD9,OLED_CMD);//--设定预充电期
	OLED_WR_Byte(0xF1,OLED_CMD);//将预充电设置为15个时钟，放电设置为1个时钟
	OLED_WR_Byte(0xDA,OLED_CMD);//--设置com引脚硬件配置
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--设置vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//设置VCOM取消选择级别
	OLED_WR_Byte(0x20,OLED_CMD);//-设置页面寻址模式 (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--设置充油泵启用/禁用
	OLED_WR_Byte(0x14,OLED_CMD);//--设置（0x10）禁用
	OLED_WR_Byte(0xA4,OLED_CMD);// 关闭整个显示器（0xa4/0xa5）
	OLED_WR_Byte(0xA6,OLED_CMD);// 禁用反向显示打开（0xa6/a7）
	OLED_WR_Byte(0xAF,OLED_CMD);//--打开oled面板
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*显示在*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  































