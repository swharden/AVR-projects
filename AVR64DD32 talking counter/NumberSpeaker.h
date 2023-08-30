#include "NumberSpeakerAudio.h"

const uint8_t* AUDIO_P1; // address currently being played
const uint8_t* AUDIO_P2; // last address in the array being played

#define IsPlaying() AUDIO_P1 < AUDIO_P2

uint8_t GetNextAudioLevel() {
	if (AUDIO_P1 < AUDIO_P2){
		return pgm_read_byte(AUDIO_P1++);
		} else {
		return 128;
	}
}

void speak_digit(uint8_t digit) {
	switch (digit) {
		
		case 0:
		AUDIO_P1 = AUDIO_SAMPLES_0;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_0);
		break;
		
		case 1:
		AUDIO_P1 = AUDIO_SAMPLES_1;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_1);
		break;
		
		case 2:
		AUDIO_P1 = AUDIO_SAMPLES_2;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_2);
		break;
		
		case 3:
		AUDIO_P1 = AUDIO_SAMPLES_3;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_3);
		break;
		
		case 4:
		AUDIO_P1 = AUDIO_SAMPLES_4;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_4);
		break;
		
		case 5:
		AUDIO_P1 = AUDIO_SAMPLES_5;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_5);
		break;
		
		case 6:
		AUDIO_P1 = AUDIO_SAMPLES_6;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_6);
		break;
		
		case 7:
		AUDIO_P1 = AUDIO_SAMPLES_7;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_7);
		break;
		
		case 8:
		AUDIO_P1 = AUDIO_SAMPLES_8;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_8);
		break;
		
		case 9:
		AUDIO_P1 = AUDIO_SAMPLES_9;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_9);
		break;
		
		default:
		AUDIO_P1 = AUDIO_SAMPLES_POINT;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_POINT);
		break;
	}
	
	while(IsPlaying()){}
}

void speak_point(){
	speak_digit(11);
}

void speak_digits(uint32_t value){
	
	if (value == 0){
		printf("0 ");
		speak_digit(0);
		while(IsPlaying()){}
		return;
	}
	
	uint32_t divisor = 1000000000;
	uint8_t real_numbers_seen = 0;
	while(divisor > 0){
		uint8_t digit = value/divisor;
		
		if (digit > 0){
			real_numbers_seen = 1;
		}
		
		if (real_numbers_seen){
			printf("%d ", digit);
			speak_digit(digit);
			while(IsPlaying()){}
		}

		value = value - (digit*divisor);
		divisor /= 10;
	}
	
	printf("\r\n");
}

void speak_mhz(uint32_t frequency, uint8_t decimals){
	
	if (frequency == 0){
		printf("0");
		speak_digit(0);
		return;
	}
	
	uint32_t divisor = 1000000000;
	uint8_t real_numbers_seen = 0;
	
	uint32_t exit_after_divisor = 100000;
	while (decimals--){
		exit_after_divisor/=10;
	}
	
	while(divisor > 0){
		uint8_t digit = frequency/divisor;
		
		if (digit > 0){
			real_numbers_seen = 1;
		}
		
		if (real_numbers_seen){
			printf("%d ", digit);
			speak_digit(digit);
			if (divisor==1000000){
				printf(". ");
				speak_point();
			}
		}
		
		frequency = frequency - (digit*divisor);
		divisor /= 10;
		
		if (divisor == exit_after_divisor){
			break;
		}
	}
	printf("\r\n");
}