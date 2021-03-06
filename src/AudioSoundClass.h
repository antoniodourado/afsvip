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
 * AudioSoundClass.h
 *
 *  Created on: 13 de mar de 2019
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
