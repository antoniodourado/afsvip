/*Multimodal, Customizable and Interactive Navigation System for Visually Impaired People 
Copyright (C) 2020 Antonio Miguel Batista Dourado

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/
/*
 * AudioFeedback.h
 *
 *  Created on: 6 de ago de 2016
 *      Author: dourado
 */

#ifndef SRC_AUDIOFEEDBACK_H_
#define SRC_AUDIOFEEDBACK_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <thread>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#define BACKEND "alut"
#include <iostream>
#include "Configuration.h"

#define MAXSOURCES 2
#define MAXBUFFERS 2
#define PATHSOURCE 0
#define OBSTACLESOURCE 1
#define VOICESOURCE 2

using namespace std;


#define TEST_ERROR(_msg)		\
	error = alGetError();		\
	if (error != AL_NO_ERROR) {	\
		fprintf(stderr, _msg "\n");	\
		return;		\
}

struct RIFF_Header {
	char chunkID[4];
	int chunkSize;
	char format[4];
};
struct WAVE_Format {
	char subChunkID[4];
	int subChunkSize;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;
};
struct WAVE_Data {
	char subChunkID[4];
	int subChunk2Size;
};

class AudioFeedback {
public:
	AudioFeedback();
	AudioFeedback(Configuration*);
	virtual ~AudioFeedback();
	void listAudioDevices();
	void doFeedbackFromMic(short*, int, uint);
	//Classe VoiceFeedback utiliza os métodos TTS abaixo
	void doTTSFeedback();
	void doTTSFeedback(const char*);
	void doTTSFeedback(const char*,ALint*);
	std::thread _doTTSFeedback(const char*,ALint*);
	void doTTSFeedbackPos(const char*, float);
	//----
	void doToneFeedback();
	void doToneFeedback(float);
	std::thread _doToneFeedback(float);
	void doToneFeedback(int);
	void doPathFeedback(float);
	std::thread _doPathFeedback(float);
	void shutAudio();
private:
	Configuration* config;
	const ALCchar *devices;
	const ALCchar *defaultDeviceName;
	char *bufferData;
	ALCdevice *device;
	ALvoid *data;
	ALCcontext *context;
	ALsizei size, freq;
	ALenum format;
	ALuint buffer, source;
	ALuint sources[MAXSOURCES];
	ALuint buffers[MAXBUFFERS];
	ALfloat listenerOri[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	ALboolean loop = AL_FALSE;
	ALCenum error;
	ALint source_state;
	ALint source_states[MAXSOURCES];
	bool isFname = false;
	char fname[30];
	float volume;

	ALenum toALFormat(short, short);
	void openAudioDevice(float);
	void closeAudioDevice();
	bool loadWavFile(const string, ALuint*, ALsizei*, ALsizei*,ALenum*);
	bool loadWavFile2(const string, ALuint*, ALsizei*, ALsizei*,ALenum*,int);
	void loadBeepBuffer(float);
	void loadPathBeepBuffer(float);
	void loadTTSBuffer(float);
};

#endif /* SRC_AUDIOFEEDBACK_H_ */
