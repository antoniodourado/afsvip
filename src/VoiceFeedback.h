/*
 * VoiceFeedback.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_VOICEFEEDBACK_H_
#define SRC_VOICEFEEDBACK_H_
#include <string.h>
#include <espeak/speak_lib.h>
#include <thread>
#include <iostream>
#include <vector>
#include "CodeUtil.h"
#include "AudioFeedback.h"
#include "RawToWavConverter.h"
#include "__afsvipGLOBALS.h"

using namespace std;


class VoiceFeedback {
public:
	VoiceFeedback();
	VoiceFeedback(AudioFeedback,Configuration*);
	virtual ~VoiceFeedback();

	void doVoiceFeedback(const char*);
	void doVoiceFeedback(const char*,const char*);
	void doVoiceFeedback(const char*,const char*,ALint*);

	void doVoiceFeedback(const char*,const char*, const char*);
	void doVoiceFeedback(const char*,const char*,float);
	std::thread _doVoiceFeedback(const char*, const char*, const char*);
	std::thread _doVoiceFeedback(const char*, const char*);
	std::thread _doVoiceFeedback(const char*);
	std::thread _doVoiceFeedback(const char*,const char*,ALint*);
	bool updateLanguage(const char*);
	static void updateVoiceSpeed(int);
	static void updateVoiceGap(int);

private:
	CodeUtil cutil;
	AudioFeedback audio;
	Configuration* config;
	const char* lang = "pt";
	espeak_AUDIO_OUTPUT output;
	int bufflen = 20000;
	int opt = 1;
	void* udata;
	espeak_PARAMETER param;
	unsigned int size, pos=0,epos;
	unsigned int flags = espeakCHARS_AUTO;
	//static int voicegap;
	//static int voicespeed;
	static int SynthCallback(short int*, int, espeak_EVENT*);

};

#endif /* SRC_VOICEFEEDBACK_H_ */
