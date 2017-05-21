#include "ls013b7dh03.h"
#include "oled_font.h"

void lcd_main(void)
{
	lcd_init();	
	lcd_clear();
			
	lcd_print_char(16,16,'a',16,1);
	
	lcd_print_num(32,32,5,1,16);
	
	lcd_print_string(0,64,"hello,world",16);
		
	lcd_refresh();
}