#define MA_NO_DECODING
#define MA_NO_ENCODING

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  44100
#define BPM                 60.0

typedef struct Note
{
	char  note;
	float beg;
	float end;
} Note;

typedef struct RD_PlaybackState
{
	unsigned long frame;
	size_t ncount;
	Note notes[10];
	
} RD_PlaybackState;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	RD_PlaybackState *ps = (RD_PlaybackState*)pDevice->pUserData;

	float *output = (float*)pOutput;

	for (ma_uint32 i = 0; i < frameCount; i++)
	{
		ps->frame++;
		float sec = (float)ps->frame / DEVICE_SAMPLE_RATE;
		float place = fmod(sec * (BPM / 60.0), 1.0);
		float value = sin(2 * M_PI * 440.0 * sec);
		float value2 = sin(2 * M_PI * 435.0 * sec);
		if (place >= ps->notes[0].beg && place < ps->notes[0].end)
		{
			output[i * 2 + 0] = value;
			output[i * 2 + 1] = value;
			output[i * 2 + 0] += value2/2;
			output[i * 2 + 1] += value2/2;
			output[i * 2 + 0] /= 1.5;
			output[i * 2 + 1] /= 1.5;
		}
	}
}

int main(int argc, char** argv)
{
	RD_PlaybackState playbackState;
	playbackState.frame = 0;
	playbackState.ncount = 1;
	playbackState.notes[0] = (Note){
		.note = 60,
		.beg = 0.0,
		.end = 0.75
	};

    ma_device device;
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);

    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &playbackState;

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
