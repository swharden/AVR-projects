#include "NumberSpeakerAudio.h"

const uint8_t* AUDIO_P1;
const uint8_t* AUDIO_P2;

uint8_t GetNextAudioLevel(){
	return pgm_read_byte(AUDIO_P1++);
}

uint8_t IsPlaying(){
	return AUDIO_P1 < AUDIO_P2;
}

void speak_digit(uint8_t digit) {
	switch (digit) {
		
		case 0:
		AUDIO_P1 = &AUDIO_SAMPLES_0;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_0);
		break;
		
		case 1:
		AUDIO_P1 = &AUDIO_SAMPLES_1;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_1);
		break;
		
		case 2:
		AUDIO_P1 = &AUDIO_SAMPLES_2;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_2);
		break;
		
		case 3:
		AUDIO_P1 = &AUDIO_SAMPLES_3;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_3);
		break;
		
		case 4:
		AUDIO_P1 = &AUDIO_SAMPLES_4;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_4);
		break;
		
		case 5:
		AUDIO_P1 = &AUDIO_SAMPLES_5;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_5);
		break;
		
		case 6:
		AUDIO_P1 = &AUDIO_SAMPLES_6;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_6);
		break;
		
		case 7:
		AUDIO_P1 = &AUDIO_SAMPLES_7;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_7);
		break;
		
		case 8:
		AUDIO_P1 = &AUDIO_SAMPLES_8;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_8);
		break;
		
		case 9:
		AUDIO_P1 = &AUDIO_SAMPLES_9;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_9);
		break;
		
		default:
		AUDIO_P1 = &AUDIO_SAMPLES_POINT;
		AUDIO_P2 = AUDIO_P1 + sizeof(AUDIO_SAMPLES_POINT);
		break;
	}
}
