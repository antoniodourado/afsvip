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
 * VoiceFeedback.h
 *
 *  Created on: 8 de ago de 2016
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
