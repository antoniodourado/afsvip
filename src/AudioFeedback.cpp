/*
 * AudioFeedback.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "AudioFeedback.h"

//

AudioFeedback::AudioFeedback() {

}

AudioFeedback::AudioFeedback(Configuration* c) {
	config = c;
	openAudioDevice(0);
}

void AudioFeedback::openAudioDevice(float pos=0){
	BOOST_LOG_TRIVIAL(info) << "(AudioFeedback) Start audio interface...";
	try{
		if(!device)
			device = alcOpenDevice(alcGetString(NULL,ALC_DEVICE_SPECIFIER));
		//
		if(!device){
			BOOST_LOG_TRIVIAL(error) << "(AudioFeedback) There was a problem while audio interface was being opened.";
			return;
		}
		context = alcCreateContext(device,NULL);
		alcMakeContextCurrent(context);
		alListener3f(AL_POSITION, pos, 0, 0);
		alListener3f(AL_VELOCITY, 0, 0, 0);
		alGenSources(MAXSOURCES,sources);
		alGenBuffers(MAXBUFFERS,buffers);
		BOOST_LOG_TRIVIAL(info) << "(AudioFeedback) Done!";
	}
	catch(exception err){
		err.what();
	}
}

void AudioFeedback::closeAudioDevice(){
	BOOST_LOG_TRIVIAL(info) << "(AudioFeedback) Closing audio interface... ";
	try{
		alDeleteSources(MAXSOURCES, sources);
		alDeleteBuffers(MAXBUFFERS, buffers);
		device = alcGetContextsDevice(context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		if(device)
			alcCloseDevice(device);
		freq = 0;
		size = 0;
		format = 0;
		BOOST_LOG_TRIVIAL(info) << "(AudioFeedback) Done!";
	}
	catch(exception err){
		err.what();
	}

}

void AudioFeedback::shutAudio() {
	closeAudioDevice();
}

std::thread AudioFeedback::_doToneFeedback(float pos) {
	return std::thread([=] {doToneFeedback(pos);});
}

std::thread AudioFeedback::_doPathFeedback(float pos) {
	return std::thread([=] {doPathFeedback(pos);});
}

void AudioFeedback::loadTTSBuffer(float pos=0){
	config->reloadConfig();
	volume = config->getSystemAPIVolume();
	//alGenSources(1, &source);
	//cout << "(AudioFeedback) CHKLDTTSBF1" << endl;
	alSourcef(sources[VOICESOURCE], AL_PITCH,1);
	alSourcef(sources[VOICESOURCE], AL_GAIN, volume);
	alSource3f(sources[VOICESOURCE], AL_POSITION, pos, 0, 0);
	alSource3f(sources[VOICESOURCE], AL_VELOCITY, 0, 0, 0);
	alSourcei(sources[VOICESOURCE], AL_LOOPING, AL_FALSE);
	//cout << "(AudioFeedback) CHKLDTTSBF2" << endl;
	//alGenBuffers(1,&buffer);
	//cout << "(AudioFeedback) Size: " << size << "(AudioFeedback)  | Freq: " << freq << "(AudioFeedback)  | Format: " << format << endl;
	loadWavFile2(fname,buffers, &size, &freq, &format,VOICESOURCE);
	//cout << "(AudioFeedback) CHKLDTTSBF3" << endl;
	alSourcei(sources[VOICESOURCE],AL_BUFFER,buffers[VOICESOURCE]);
	//cout << "(AudioFeedback) CHKLDTTSBF4" << endl;
}

void AudioFeedback::loadBeepBuffer(float pos=0) {
	config->reloadConfig();
	volume = config->getSystemAPIVolume();
	//alGenSources(1, &source);
	//cout << "(AudioFeedback) CHKLDOBSBF1" << endl;
	alSourcef(sources[OBSTACLESOURCE], AL_PITCH,1);
	alSourcef(sources[OBSTACLESOURCE], AL_GAIN, volume);
	alSource3f(sources[OBSTACLESOURCE], AL_POSITION, pos, 0, 0);
	alSource3f(sources[OBSTACLESOURCE], AL_VELOCITY, 0, 0, 0);
	alSourcei(sources[OBSTACLESOURCE], AL_LOOPING, AL_FALSE);
	//cout << "(AudioFeedback) CHKLDOBSBF2" << endl;
	//alGenBuffers(1,&buffer);
	loadWavFile2("Assets/beep.wav",buffers, &size, &freq, &format,OBSTACLESOURCE);
	//cout << "(AudioFeedback) CHKLDOBSBF3" << endl;
	alSourcei(sources[OBSTACLESOURCE],AL_BUFFER,buffers[OBSTACLESOURCE]);
	//cout << "(AudioFeedback) CHKLDOBSBF4" << endl;
}

void AudioFeedback::loadPathBeepBuffer(float pos=0) {
	config->reloadConfig();
	volume = config->getSystemAPIVolume();
	//alGenSources(1, &source);
	if(pos==-2){
		//alSourcef(source, AL_PITCH,0.1);
		alSourcef(sources[PATHSOURCE], AL_PITCH,0.1);
		pos=0;
	}
	else{
		alSourcef(sources[PATHSOURCE], AL_PITCH,0.5);
	}
	alSourcef(sources[PATHSOURCE], AL_GAIN, volume);
	alSource3f(sources[PATHSOURCE], AL_POSITION, pos, 0, 0);
	alSource3f(sources[PATHSOURCE], AL_VELOCITY, 0, 0, 0);
	alSourcei(sources[PATHSOURCE], AL_LOOPING, AL_FALSE);
	//alGenBuffers(1,&buffer);
	//loadWavFile("Assets/beep.wav",&buffer, &size, &freq, &format);
	ALsizei size,freq;
	ALenum format;
	loadWavFile2("Assets/beep.wav",buffers, &size, &freq, &format,PATHSOURCE);
	//alSourcei(source,AL_BUFFER,buffer);
	alSourcei(sources[PATHSOURCE],AL_BUFFER,buffers[PATHSOURCE]);
}

void AudioFeedback::listAudioDevices(){
	const ALCchar *dev = alcGetString(NULL,ALC_DEVICE_SPECIFIER);
	const ALCchar *next = dev+1;
	size_t len = 0;

	cout << "(AudioFeedback) Audio devices found: " << endl;
	while(dev &&  *dev != '\0' && next && *next != '\0'){
		cout << dev << endl;
		len = strlen(dev);
		dev += len+1;
		next += len+2;
	}
}

void AudioFeedback::doToneFeedback(){
	try{
		/*if(!device)
			openAudioDevice();*/
		loadBeepBuffer();
		for(int i=0; i < 3; i++){
			alSourcePlay(source);
			alGetSourcei(source,AL_SOURCE_STATE,&source_state);
			while(source_state == AL_PLAYING)
				alGetSourcei(source,AL_SOURCE_STATE,&source_state);
			usleep(75000);
		}
		/*alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
		closeAudioDevice();*/
	}
	catch(exception err){
		err.what();
	}
}

void AudioFeedback::doToneFeedback(float pos) {
	/*while(gvars.soundInUse)
		continue;
	gvars.soundInUse=true;*/
	try{
		if(!device)
			openAudioDevice();
		loadBeepBuffer(pos);
		for(int i=0; i < 3; i++){
			alSourcePlay(sources[OBSTACLESOURCE]);
			alGetSourcei(sources[OBSTACLESOURCE],AL_SOURCE_STATE,&source_state);
			while(source_state == AL_PLAYING)
				alGetSourcei(sources[OBSTACLESOURCE],AL_SOURCE_STATE,&source_state);
			usleep(75000);
		}
		//alDeleteSources(1, &sources[OBSTACLESOURCE]);
		//alDeleteBuffers(1, &buffers[OBSTACLESOURCE]);
		closeAudioDevice();
	}
	catch(exception err){
		err.what();
	}
	gvars.soundInUse=false;
}

void AudioFeedback::doToneFeedback(int times){
	try{
		/*if(!device)
			openAudioDevice();*/
		loadBeepBuffer();
		for(int i=0; i < times; i++){
			alSourcePlay(source);
			alGetSourcei(source,AL_SOURCE_STATE,&source_state);
			while(source_state == AL_PLAYING)
				alGetSourcei(source,AL_SOURCE_STATE,&source_state);
			usleep(75000);
		}
		/*alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
		closeAudioDevice();*/
	}
	catch(exception err){
		err.what();
	}
}

void AudioFeedback::doFeedbackFromMic(short *buff1, int SIZE, uint FREQ){
	//openAudioDevice();
	alGenSources(1,&source);
	ALuint buff2;
	alBufferData(buff2,AL_FORMAT_MONO16,buff1,SIZE*sizeof(short),FREQ);
	alSourceQueueBuffers(source,1,&buff2);
	alGetSourcei(source,AL_SOURCE_STATE,&source_state);
	while(source_state == AL_PLAYING)
		alGetSourcei(source,AL_SOURCE_STATE,&source_state);

}

ALenum AudioFeedback::toALFormat(short channels, short samples){

	bool stereo = (channels > 1);

	switch (samples) {
	case 16:
		if (stereo)
			return AL_FORMAT_STEREO16;
		else
			return AL_FORMAT_MONO16;
	case 8:
		if (stereo)
			return AL_FORMAT_STEREO8;
		else
			return AL_FORMAT_MONO8;
	default:
		return -1;
	}
}


void AudioFeedback::doTTSFeedback(){
	isFname = false;
	strcpy(fname,"Assets/VoiceFeedback.wav");
	try{
		/*if(!device)
			openAudioDevice();
		loadTTSBuffer();
		alSourcePlay(source);
		alGetSourcei(source,AL_SOURCE_STATE,&source_state);
		while(source_state == AL_PLAYING)
			alGetSourcei(source,AL_SOURCE_STATE,&source_state);
		if(!remove("Assets/VoiceFeedback.wav")){
			cout << "(AudioFeedback) VoiceFeedback.wav was successfully deleted!" << endl;
		}
		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);*/
		//closeAudioDevice();

	}
	catch(exception err){
		err.what();
	}
}

std::thread AudioFeedback::_doTTSFeedback(const char *name, ALint *s){
	return thread([=] {_doTTSFeedback(name,s);});
}

void AudioFeedback::doTTSFeedback(const char *name,ALint *s){
	/*isFname = true;
	strcpy(fname,name);
	try{
		if(!device)
			openAudioDevice();
		loadTTSBuffer();
		alSourcePlay(source);
		alGetSourcei(source,AL_SOURCE_STATE,s);
		while(*s == AL_PLAYING)
			alGetSourcei(source,AL_SOURCE_STATE,s);
		if(!remove(fname)){
			cout << fname << "(AudioFeedback)  was successfully deleted!" << endl;
		}

		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
		closeAudioDevice();

	}
	catch(exception err){
		err.what();
	}*/
}

void AudioFeedback::doTTSFeedback(const char *name){
	/*while(gvars.soundInUse)
		continue;
	gvars.soundInUse=true;*/
	isFname = true;
	strcpy(fname,name);
	try{
		/*if(!device)
			openAudioDevice();*/
		/*loadTTSBuffer();
		alSourcePlay(sources[VOICESOURCE]);
		ALint state;
		alGetSourcei(sources[VOICESOURCE],AL_SOURCE_STATE,&state);
		while(state == AL_PLAYING)
			alGetSourcei(sources[VOICESOURCE],AL_SOURCE_STATE,&state);
		usleep(75000);
		if(!remove(fname)){
			cout << fname << "(AudioFeedback)  was successfully deleted!" << endl;
		}
		alDeleteSources(1, &sources[VOICESOURCE]);
		alDeleteBuffers(1, &buffers[VOICESOURCE]);*/
		//closeAudioDevice();

	}
	catch(exception err){
		err.what();
	}
	gvars.soundInUse=false;
}

void AudioFeedback::doTTSFeedbackPos(const char* name, float pos) {
	/*while(gvars.soundInUse)
		continue;
	gvars.soundInUse=true;*/
	isFname = true;
	strcpy(fname,name);
	try{
		/*if(!device)
			openAudioDevice();*/
		/*loadTTSBuffer(pos);
		alSourcePlay(sources[VOICESOURCE]);
		ALint state;
		alGetSourcei(sources[VOICESOURCE],AL_SOURCE_STATE,&state);
		while(state == AL_PLAYING)
			alGetSourcei(sources[VOICESOURCE],AL_SOURCE_STATE,&state);
		if(!remove(fname)){
			cout << fname << "(AudioFeedback)  was successfully deleted!" << endl;
		}
		alDeleteSources(1, &sources[VOICESOURCE]);
		alDeleteBuffers(1, &buffers[VOICESOURCE]);*/
		//closeAudioDevice();

	}
	catch(exception err){
		err.what();
	}
	gvars.soundInUse=false;
}

void AudioFeedback::doPathFeedback(float pos) {
	/*while(gvars.soundInUse)
		continue;
	gvars.soundInUse=true;*/
	try{
		if(!device)
			openAudioDevice();
		loadPathBeepBuffer(pos);
		alSourcePlay(sources[PATHSOURCE]);
		ALint state;
		alGetSourcei(sources[PATHSOURCE],AL_SOURCE_STATE,&state);
		while(state == AL_PLAYING)
			alGetSourcei(sources[PATHSOURCE],AL_SOURCE_STATE,&state);
		//alDeleteSources(1, &sources[PATHSOURCE]);
		//alDeleteBuffers(1, &buffers[PATHSOURCE]);
		closeAudioDevice();
	}
	catch(exception err){
		err.what();
	}
	gvars.soundInUse=false;
}


bool AudioFeedback::loadWavFile(const std::string filename, ALuint* buffer,
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

		//cout << "(AudioFeedback) Chunksize: " << wave_data.subChunk2Size << endl;

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

		std::cerr << error << "(AudioFeedback)  : trying to load "
				<< filename << std::endl;

		if (soundFile != NULL)
			fclose(soundFile);
		return false;
	}
}

bool AudioFeedback::loadWavFile2(const std::string filename, ALuint* buffer,
		ALsizei* size, ALsizei* frequency,
		ALenum* format, int SOURCETYPE) {
	FILE* soundFile = NULL;
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	unsigned char* data;

	//std::cout << "(AudioFeedback) \r\n\r\n\r ARQUIVO: " << filename << "(AudioFeedback) \r\n\r\n\r" << std::endl;

	try {
		soundFile = fopen(filename.c_str(), "rb+");
		while(!soundFile)
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

		//cout << "(AudioFeedback) Chunksize: " << wave_data.subChunk2Size << endl;

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

	        //alGenBuffers(1, &buffer[SOURCETYPE]);
		alBufferData(buffer[SOURCETYPE], *format, (void*)data,
				*size, *frequency);
		fclose(soundFile);
		return true;
	} catch(std::string error) {

		std::cerr << error << "(AudioFeedback)  : trying to load "
				<< filename << std::endl;

		if (soundFile != NULL)
			fclose(soundFile);
		return false;
	}
}

AudioFeedback::~AudioFeedback() {
	closeAudioDevice();
}

