#define MA_NO_DECODING
#define MA_NO_ENCODING

#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  44100

#define NOTES_MAX 255
#define SHEETS_MAX 1024

typedef struct Note {
	float vol;
	float freq;
} Note;

typedef struct Sheet {
	int ncount;
	Note notes[NOTES_MAX];
} Sheet;

typedef struct PbState {
	int frame;
	float bpm;
	Sheet sheet[SHEETS_MAX];
} PbState;

void PbInit(PbState* pbstate) {
	pbstate->bpm = 120;
	pbstate->frame = 0;
}

void data_callback(
	ma_device* pDevice,
	void* pOutput,
	const void* pInput,
	ma_uint32 frameCount)
{
	PbState *ps = (PbState*)pDevice->pUserData;

	float *output = (float*)pOutput;

	for (ma_uint32 i = 0; i < frameCount; i++)
	{
		ps->frame++;
		float sec = (float)ps->frame / DEVICE_SAMPLE_RATE;
		int beat = (int)(fmod(sec, 60)*ps->bpm/60);
		float value = (float)rand()/RAND_MAX/5;
		//value += sin(2 * M_PI * ps->sheet[beat%16].freq * sec) * ps->sheet[beat%16].vol;
		output[i*2+0] = value;
		output[i*2+1] = value;
	}
}

void MAInit(ma_device* device, PbState* pbstate)
{
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);

    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &pbstate;

    if (ma_device_init(NULL, &deviceConfig, device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        exit(-4);
    }

    //printf("Device Name: %s\n", device.playback.name);

    if (ma_device_start(device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(device);
        exit(-5);
    }
}

int main(int argc, char** argv) {
	PbState pbstate;
	PbInit(&pbstate);

    ma_device device;
	MAInit(&device, &pbstate);

    while (1)
	{
	}

    ma_device_uninit(&device);
    
    return 0;
}
