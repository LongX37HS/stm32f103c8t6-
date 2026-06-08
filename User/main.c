#include "stm32f10x.h"                  // Device header
#include "OLED.h"


int main(){
	OLED_Init();
	OLED_ShowString(1,1,"WTF");
	while(1){
		
	}
}

