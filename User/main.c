#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "Delay.h"

int main(){
	LED_Init();
	OLED_Init();
	OLED_ShowString(1,1,"WTF");

	while(1){
		LED_ON();
		Delay_ms(500);
		LED_OFF();
		Delay_ms(500);
		
	}
}

