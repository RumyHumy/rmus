#define MA_NO_DECODING
#define MA_NO_ENCODING

#include "miniaudio.h"

#include <stdio.h>
#include <math.h>

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  44100

typedef struct State {
	int frame;
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
		value += sin(2 * M_PI * 220.0 * sec);
		output[i*2+0] = value;
		output[i*2+1] = value;
;
	}
}

int main(int argc, char** argv)
{
	State state;

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
    
    while (1)
	{
		//printf("Frame: %lu\n", playbackState.frame);
	}

    ma_device_uninit(&device);
    
    return 0;
}
