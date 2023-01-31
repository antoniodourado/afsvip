/*
 * AudioSoundClass.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC_AUDIOSOUNDCLASS_H_
#define SRC_AUDIOSOUNDCLASS_H_

#include <string.h>
#include <iostream>
#include <string.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#define BACKEND "alut"

#define MAXSOURCES 256
#define MAXBUFFERS 256

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

class AudioSoundClass {
public:
	AudioSoundClass(ALint, ALint, ALint, const char*, float,float);
	virtual ~AudioSoundClass();
	ALint X,Y,Z;
	ALuint buffer;
	ALuint source;
	int soundID;
	ALsizei size, freq;
	ALenum format;
private:
	bool loadWavFile(const std::string, ALuint*, ALsizei*, ALsizei*,ALenum*);
};

#endif /* SRC_AUDIOSOUNDCLASS_H_ */
