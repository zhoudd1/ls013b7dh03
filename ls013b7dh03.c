#include <stdbool.h>
#include <stdint.h>
#include <string.h> 
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "ls013b7dh03.h"
#include "oled_font.h"

#define delay_us(x) nrf_delay_us(x)
#define delay_ms(x) nrf_delay_ms(x)

// Commands
#define MLCD_WR 0x80 //MLCD write line command
#define MLCD_CM 0x20 //MLCD clear memory command
#define MLCD_NO 0x00 //MLCD NOP command (used to switch VCOM)

//defines the VCOM bit in the command word that goes to the LCD
#define VCOM_HI 0x40
#define VCOM_LO 0x00


#define DIS_1 nrf_gpio_pin_set(8);
#define DIS_0 nrf_gpio_pin_clear(8);

#define EXT_1 
#define EXT_0 

#define SCS_1 nrf_gpio_pin_set(7);
#define SCS_0 nrf_gpio_pin_clear(7);

#define SI_1 nrf_gpio_pin_set(6);
#define SI_0 nrf_gpio_pin_clear(6);

#define SCLK_1 nrf_gpio_pin_set(5);
#define SCLK_0 nrf_gpio_pin_clear(5);

#define LCD_ON nrf_gpio_pin_set(4);
#define LCD_OFF nrf_gpio_pin_clear(4);

#define LINE (128)
#define LINE_SIZE (16)
static char LCD_GRAM[LINE][LINE_SIZE];

void lcd_init(void)
{
		nrf_gpio_cfg_output(5);
		nrf_gpio_cfg_output(6);
		nrf_gpio_cfg_output(7);
		nrf_gpio_cfg_output(8);	
	
		DIS_0;
		SCS_0;
		lcd_clear();
		DIS_1;
		delay_us(30);
		EXT_1;
		delay_us(30);
}

void write_byte(uint8_t Dbyte)
{
	uint8_t i;
	SCLK_0;
	for(i=0;i<8;i++)
	{
		if(Dbyte&0x80)
		{
		 SI_1;
		}
		else
		{
		 SI_0;
		}

		SCLK_1;
		SCLK_0;
		Dbyte=Dbyte<<1;
		delay_us(5);
	}

}

void write_byte1(uint8_t Dbyte)
{
	uint8_t i;
	SCLK_0;
	for(i=0;i<8;i++)
	{
		 if(Dbyte&0x01)
		 {
			 SI_1;
		 }
		 else
		 {
			 SI_0;
		 }

		 SCLK_1;
		 SCLK_0;
		 Dbyte=Dbyte>>1;
		 delay_us(5);
	}
}

void lcd_clear(void)
{
	SCS_1;
	delay_us(5);
	write_byte(0x20);
	write_byte(0);
	delay_us(5);
	SCS_0;
}

void lcd_refresh(void){
	uint16_t i=0,ln=0;
	SCS_1;
	delay_us(5);
	write_byte(0x80);
	
	for(ln=0;ln<128;ln++)
	{
		write_byte1(ln);
		for(i=0;i<16;i++)
		{
			write_byte(LCD_GRAM[ln][i]);
		}
		write_byte(0);
	}

	write_byte(0);write_byte(0);

	delay_us(5);
	SCS_0;
	
}

void lcd_drawpoint(uint16_t x,uint16_t y,bool bDraw){
	
 uint16_t pos,bx,tmp;

  if(x>OLED_MAX_X-1||y>OLED_MAX_Y-1)
    return;
  pos=15-y/8;
  bx=y%8;
	tmp=1<<(bx);
  if(bDraw)
     LCD_GRAM[x][pos]|= tmp;
    else
     LCD_GRAM[x][pos]&= ~tmp; 
    
}

void lcd_fillRect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,bool bDraw)
{
  uint16_t wi,hi;
  for(hi=0;hi<h;hi++){
   for(wi=0;wi<w;wi++)
      lcd_drawpoint(x+wi,y+hi,bDraw);
  }
}

void lcd_fillRectByXY(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,bool bDraw)
{
   uint16_t xStart=0,yStart=0;
   uint16_t w,h;
   
   if(x0<x1){
     xStart=x0;
     w=x1-x0+1;
   }else{
     xStart=x1;
     w=x0-x1+1;
   }
   
   if(y0<y1){
     yStart=y0;
     h=y1-y0+1;
   }else{
      yStart=y1;
      h=y0-y1+1;
   }
   lcd_fillRect(xStart,yStart,w,h,bDraw);
   
}

uint32_t lcd_getpoint(uint16_t x,uint16_t y)
{
}

void lcd_print_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{                   
    uint8_t temp,t,t1;
    uint8_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);      
    chr=chr-' ';       
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206[chr][t];         
        else if(size==16)temp=asc2_1608[chr][t];    
        else if(size==24)temp=asc2_2412[chr][t];    
        else return;                                
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)lcd_drawpoint(x,y,mode);
            else lcd_drawpoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }    
    }          
}

uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;    
    while(n--)result*=m;    
    return result;
}                 
         
void lcd_print_num(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{           
    uint8_t t,temp;
    uint8_t enshow=0;                           
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                lcd_print_char(x+(size/2)*t,y,' ',size,1);
                continue;
            }else enshow=1; 
              
        }
        lcd_print_char(x+(size/2)*t,y,temp+'0',size,1); 
    }
} 

void lcd_print_string(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{   
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(128-size)){y=x=0;lcd_clear();}
        lcd_print_char(x,y,*p,size,1);    
        x+=size/2;
        p++;
    }  
     
}
