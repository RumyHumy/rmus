#define MA_NO_DECODING
#define MA_NO_ENCODING

#include "miniaudio.h"
#include <ncurses.h>

#include <stdio.h>
#include <math.h>

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  44100

#define MAX_FCOUNT 255

typedef struct State {
	int frame;
	float freq;
} State;

void data_callback(
	ma_device* pDevice,
	void* pOutput,
	const void* pInput,
	ma_uint32 frameCount)
{
	State *ps = (State*)pDevice->pUserData;

	float *output = (float*)pOutput;

	for (ma_uint32 i = 0; i < frameCount; i++)
	{
		ps->frame++;
		float sec = (float)ps->frame / DEVICE_SAMPLE_RATE;
		float value = 0;
		value += sin(2 * M_PI * ps->freq * sec);
		output[i*2+0] = value;
		output[i*2+1] = value;
;
	}
}

int main(int argc, char** argv)
{
	State state;
	state.frame = 0;
	state.freq = 0;

    ma_device device;
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);

    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &state;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        return -4;
    }

    printf("Device Name: %s\n", device.playback.name);

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return -5;
    }

	initscr();
	timeout(-1);
	float nfreq[13];
	for (int i = 0; i < 13; i++)
		nfreq[i] = 220.0f*pow(2, (float)i/12);
    while (1)
	{
		int c = getch();
		switch (c) {
		case 's': state.freq = nfreq[0]; break;
		case 'e': state.freq = nfreq[1]; break;
		case 'd': state.freq = nfreq[2]; break;
		case 'r': state.freq = nfreq[3]; break;
		case 'f': state.freq = nfreq[4]; break;
		case 'g': state.freq = nfreq[5]; break;
		case 'y': state.freq = nfreq[6]; break;
		case 'h': state.freq = nfreq[7]; break;
		case 'u': state.freq = nfreq[8]; break;
		case 'j': state.freq = nfreq[9]; break;
		case 'i': state.freq = nfreq[10]; break;
		case 'k': state.freq = nfreq[11]; break;
		case 'l': state.freq = nfreq[12]; break;
		}
	}
	endwin();

    ma_device_uninit(&device);
    
    return 0;
}
