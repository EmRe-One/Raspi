#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <stdint.h>
#include <stdbool.h>

#include <bcm2835.h>
#include <tm1638.h>



static void niceFooo(tm1638_p t);
static uint8_t readInput(tm1638_p t);
bool debug = false;	

static double log_2(double n){
	return log(n)/log(2);
} 

int main(int argc, char *argv[]){
	
	tm1638_p t;
	
	if (!bcm2835_init()){
		printf("Unable to initialize BCM library\n");
		return -1;
    }

	t = tm1638_alloc(17, 27, 22);
	if (!t){
		printf("Unable to allocate TM1638\n");
		return -2;
    }
    
	tm1638_send_cls(t);
	niceFooo(t);
	tm1638_send_cls(t);
	tm1638_set_7seg_text(t, "HELLO -)", 0x00);
	delay(2500);
	
	bool play = true;
	while(play){
		
		niceFooo(t);
		tm1638_send_cls(t);
		tm1638_set_7seg_text(t, "READY ?", 0x00);
		readInput(t);
 
		bool lost = false;
		int round = 0;
		char text[10];
		int com[255];
		srand(time(NULL));
		int r = rand() % 8;
	
		while(lost == 0){
			int r = rand() % 8;
			com[round] = 1 << r;
		
			tm1638_send_cls(t);
			snprintf(text, 9, "ROUND %d", ++round);
			tm1638_set_7seg_text(t, text, 0x00);
			delay(1000);
		
			for(int i=0; i < round; i++){
				tm1638_set_8leds(t, com[i], 0);
				delay(500);
				tm1638_set_8leds(t,0,0);
				delay(100);
			}
			
			tm1638_send_cls(t);
			if(debug){
				printf("Runde %d:\n", round);
				for(int i=0; i < round; i++){
					printf("%d\n", 8 - ((int) log_2(com[i])) );
				}
			}
			
			tm1638_set_7seg_text(t, "YOURTURN", 0x00);
		
			for(int i=0; i < round; i++){
				uint8_t input = readInput(t);
				tm1638_set_7seg_text(t, "", 0x00);
				tm1638_set_8leds(t,input,0);
				delay(300);
				tm1638_set_8leds(t,0,0);
				if(input != com[i]){
					lost = true;
					break;
				}
			}
		}
		
	
		tm1638_set_7seg_text(t, "GAMEOVER", 0x00);
		delay(2000);
		
		
		tm1638_set_7seg_text(t, "NEW GAME", 0x00);
		tm1638_set_8leds(t, (1 << 7)|1, 0);
		
		uint8_t btn;
		
		do{
			btn = readInput(t);
			
			if (btn == 1){
				play = false;
			}
			if (btn == 1 << 7){
				play = true;
			}
		}while(btn != 1 && btn != 1<<7);
		
	}
	
	
	tm1638_set_8leds(t, 0, 0);
	tm1638_set_7seg_text(t, "GOOD BYE", 0x00);
	tm1638_free(&t);

	return 0;
}


static void niceFooo(tm1638_p t){

	for(int j = 0; j < 8; j++){
		uint8_t m = 128 >> j;
		tm1638_set_8leds(t, m, 0);
		tm1638_set_7seg_text(t, "", m);
		delay(25);
	}

	for(int j = 0; j < 8; j++){
		uint8_t m = 1 << j;
		tm1638_set_8leds(t, m, 0);
		tm1638_set_7seg_text(t, "", m);
		delay(25);
	}
}


static uint8_t readInput(tm1638_p t){
	uint8_t lastBtn = 0, btn = 0;
	
	while(1){
		btn = tm1638_read_8buttons(t);
		if(btn != lastBtn){
			if(btn != 0){
				if(debug) printf("btn pressed\n");
				lastBtn = btn;
			}else{
				if(debug) printf("btn release\n");
				break;
			}
		}
		delay(10);
	}
	
	return lastBtn;
}

