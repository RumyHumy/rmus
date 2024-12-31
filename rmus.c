#define MA_NO_DECODING
#define MA_NO_ENCODING

#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define M_PI 3.14159265358979323846

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  44100

// Prerender
#define PR_FREQ    10
#define PR_SAMPLES (DEVICE_SAMPLE_RATE/PR_FREQ)

#define BEATS_MAX 1000000

typedef struct Song {
	float vol[32];
	float freq[32];
} Song;

typedef struct PbState {
	int frame;
	float bpm;
	Song song;
	// Prerender
	float phase; // [0, 1]
	float pre_sine[PR_SAMPLES];
} PbState;

void PbInit(PbState* pbstate) {
	pbstate->bpm = 120;
	pbstate->frame = 0;
	pbstate->phase = 0;
	for (int i = 0; i < PR_SAMPLES; i++) {
		float phase = (float)i/PR_SAMPLES;
		pbstate->pre_sine[i] = sin(2*M_PI*phase);
	}
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
		float value = 0.0;
		//value += sin(2 * M_PI * ps->sheet[beat%16].freq * sec) * ps->sheet[beat%16].vol;
		value += ps->pre_sine[(int)(ps->phase*PR_SAMPLES)]/5;
		output[i*2+0] = value;
		output[i*2+1] = value;
		ps->phase += 440.0*(1.0/DEVICE_SAMPLE_RATE);
		if (ps->phase >= 1.0)
			ps->phase = 0.0;
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
    deviceConfig.pUserData         = pbstate;

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

char* FileReadAlloc(FILE* fptr) {
	int reserve = 16;
	char* data = malloc(reserve);
	if (!data)
		exit(1);
	if (!fptr) {
		printf("Error while openeing a file.");
		exit(1);
	}
	char ch;
	int i = 0;
	while (1) {
		printf("%c\n", ch);
		ch = fgetc(fptr);
		if (ch == EOF)
			break;
		if (i == reserve-1) {
			reserve *= 2;
			data = realloc(data, reserve);
			if (!data)
				exit(1);
		}
		data[i] = ch;
		i++;
	}
	data[i] = '\0';
	return data;

}

char** DataTokenize(char* data) {
	int reserve = 16;
	char **toks = calloc(reserve, sizeof(*toks));
	if (!toks)
		exit(1);
	char* lptr = data;
	int t = 0;
	for (char* ptr = data; *ptr; ptr++) {
		printf("'%c'\n", *ptr);
		if (t == reserve-1) {
			reserve *= 2;
			toks = realloc(toks, reserve*sizeof(*toks));
			if (!toks)
				exit(1);
		}
		if (*ptr == ' ' || *ptr == '\n') {
			printf("%p\n", ptr);
			toks[t] = lptr;
			*ptr = '\0';
			lptr = ptr+1;
			t++;
		}
	}
	toks[t] = NULL;

	return toks;
}

void LoadSong(char* fname, Song* song) {
	memset(song->vol, 0, 32);
	memset(song->freq, 0, 32);

	FILE* fptr = fopen(fname, "r");
	char* data = FileReadAlloc(fptr);
	fclose(fptr);
	if (!data)
		exit(1);

	char **toks = DataTokenize(data);
	for (char** tok = toks; *tok; tok++) {
		printf("%d: '%s'\n", *tok, *tok);
	}

	free(data);

	exit(0);
}

int main(int argc, char** argv) {
	PbState pbstate;
	PbInit(&pbstate);
	LoadSong("track.rmu", &pbstate.song);

    ma_device device;
	MAInit(&device, &pbstate);

    while (1)
	{
	}

    ma_device_uninit(&device);
    
    return 0;
}
