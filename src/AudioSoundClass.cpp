/*
 * AudioSoundClass.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "AudioSoundClass.h"

AudioSoundClass::AudioSoundClass(ALint Xi=0, ALint Yi=0, ALint Zi=0,const char* fname="", float vol=1,float pos=0) {
	X=Xi;
	Y=Yi;
	Z=Zi;
	alGenSources(1, &source);
	if(pos==-2){
		alSourcef(source, AL_PITCH,0.1);
		pos=0;
	}
	else
		alSourcef(source, AL_PITCH,0.5);
	alSourcef(source, AL_GAIN, vol);
	alSource3f(source, AL_POSITION, pos, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);
	alGenBuffers(1,&buffer);
	loadWavFile("Assets/beep.wav",&buffer, &size, &freq, &format);
	alSourcei(source,AL_BUFFER,buffer);
}

AudioSoundClass::~AudioSoundClass() {
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	freq = 0;
	size = 0;
	format = 0;
}

bool AudioSoundClass::loadWavFile(const std::string filename, ALuint* buffer,
		ALsizei* size, ALsizei* frequency,
		ALenum* format) {

	FILE* soundFile = NULL;
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	unsigned char* data;

	try {
		soundFile = fopen(filename.c_str(), "rb");
		if (!soundFile)
			throw (filename);

		fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

		if ((riff_header.chunkID[0] != 'R' ||
				riff_header.chunkID[1] != 'I' ||
				riff_header.chunkID[2] != 'F' ||
				riff_header.chunkID[3] != 'F') ||
				(riff_header.format[0] != 'W' ||
						riff_header.format[1] != 'A' ||
						riff_header.format[2] != 'V' ||
						riff_header.format[3] != 'E')){
			throw ("Invalid RIFF or WAVE Header");
		}

		fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);

		if (wave_format.subChunkID[0] != 'f' ||
				wave_format.subChunkID[1] != 'm' ||
				wave_format.subChunkID[2] != 't' ||
				wave_format.subChunkID[3] != ' ')
			throw ("Invalid Wave Format");

		if (wave_format.subChunkSize > 16)
			fseek(soundFile, sizeof(short), SEEK_CUR);

		fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);

		if (wave_data.subChunkID[0] != 'd' ||
				wave_data.subChunkID[1] != 'a' ||
				wave_data.subChunkID[2] != 't' ||
				wave_data.subChunkID[3] != 'a')
			throw ("Invalid data header");

		data = new unsigned char[wave_data.subChunk2Size];

		//cout << "Chunksize: " << wave_data.subChunk2Size << endl;

		if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
			throw ("error loading WAVE data into struct!");

		*size = wave_data.subChunk2Size;
		*frequency = wave_format.sampleRate;

		if (wave_format.numChannels == 1) {
			if (wave_format.bitsPerSample == 8 )
				*format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_MONO16;
		} else if (wave_format.numChannels == 2) {
			if (wave_format.bitsPerSample == 8 )
				*format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_STEREO16;
		}

		alGenBuffers(1, buffer);
		alBufferData(*buffer, *format, (void*)data,
				*size, *frequency);
		fclose(soundFile);
		return true;
	} catch(std::string error) {

		std::cerr << error << " : trying to load "
				<< filename << std::endl;

		if (soundFile != NULL)
			fclose(soundFile);
		return false;
	}
}
