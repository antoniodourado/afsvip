/*
 * VoiceFeedback.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "VoiceFeedback.h"

vector<short> sdata;
int samp,sampr;
int tsize;
char fname[30];
bool isFname = false;
//bool *lock = false;
unsigned int uniqueid;

VoiceFeedback::VoiceFeedback() {
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Starting voice feedback..";
	output = AUDIO_OUTPUT_SYNCHRONOUS;
	sampr = espeak_Initialize(output, bufflen, NULL, 0);
	espeak_SetSynthCallback(&SynthCallback);
	espeak_SetVoiceByName(lang);
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
}

VoiceFeedback::VoiceFeedback(AudioFeedback a,Configuration* c) {
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Starting voice feedback..";
	output = AUDIO_OUTPUT_PLAYBACK;
	sampr = espeak_Initialize(output, bufflen, NULL, 0);
	espeak_SetSynthCallback(&SynthCallback);
	espeak_SetVoiceByName(lang);
	audio = a;
	config=c;
	//voicespeed = config->getVoiceSpeed();
	//voicegap = config->getVoiceGap();
	espeak_SetParameter(espeakRATE, config->getVoiceSpeed(),0);
	espeak_SetParameter(espeakWORDGAP, config->getVoiceGap(),0);
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
}

void VoiceFeedback::doVoiceFeedback(const char *text, const char *name, ALint *s){
	/*int spd = config->getVoiceSpeed();
	if(spd != voicespeed)
		updateVoiceSpeed(spd);*/
	size = strlen(text)+1;
	tsize=size;
	isFname = true;
	strcpy(fname,"");

	strcat(fname,"Assets/");
	strcat(fname,name);
	strcat(fname,".wav");


	//cout << fname << endl;
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Saying: " << text;
	try{
		espeak_Synth(text, size, pos, POS_SENTENCE,0,flags,&uniqueid,this);
		espeak_Synchronize();

		BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
		//audio.doTTSFeedback(fname,s);

	}
	catch(exception err){
		err.what();
	}
}

thread VoiceFeedback::_doVoiceFeedback(const char *text, const char *name, ALint *s){
	return std::thread([=] {doVoiceFeedback(text,name,s);});
}

std::thread VoiceFeedback::_doVoiceFeedback(const char* x, const char* y, const char* z) {
	return std::thread([=] {doVoiceFeedback(x,y,z);});
}

std::thread VoiceFeedback::_doVoiceFeedback(const char* x, const char* y) {
	return std::thread([=] {doVoiceFeedback(x,y);});
}

std::thread VoiceFeedback::_doVoiceFeedback(const char* x) {
	return std::thread([=] {doVoiceFeedback(x);});
}

void VoiceFeedback::doVoiceFeedback(const char* text, const char* name, float pos) {
	/*int spd = config->getVoiceSpeed();
	if(spd != voicespeed)
		updateVoiceSpeed(spd);*/
	size = strlen(text)+1;
	tsize=size;
	isFname = true;
	strcpy(fname,"");

	strcat(fname,"Assets/");
	strcat(fname,name);
	strcat(fname,".wav");


	//cout << fname << endl;
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Saying: " << text;
	try{
		espeak_Synth(text, size, pos, POS_SENTENCE,0,flags,&uniqueid,this);
		espeak_Synchronize();

		BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
		//audio.doTTSFeedbackPos(fname,pos);
	}
	catch(exception err){
		err.what();
	}
}

int VoiceFeedback::SynthCallback(short int *wav, int nsamp, espeak_EVENT *events){
		if(!wav)
			return -1;


		sdata.insert(sdata.end(),wav,wav+nsamp);
		samp=nsamp;
		cout << "Event: " << events->text_position << endl;

		/*RawToWaveConverter conv;
		conv.convert(sdata,samp,sampr,events->type,isFname,fname);*/

}

void VoiceFeedback::doVoiceFeedback(const char* text){
	/*int spd = config->getVoiceSpeed();
	if(spd != voicespeed)
		updateVoiceSpeed(spd);*/
	std::cout << *text << std::endl;
	size = strlen(text)+1;
	tsize=size;
	const char* name = cutil.randomString(5);
	isFname = true;
	strcpy(fname,"");

	strcat(fname,"Assets/");
	strcat(fname,name);
	strcat(fname,".wav");
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Saying: " << text << "...";
	try{
		espeak_Synth(text, size, pos, POS_SENTENCE,0,flags,&uniqueid,this);
		espeak_Synchronize();
		BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
		//audio.doTTSFeedback();
	}
	catch(exception err){
		err.what();
	}
}

/*void VoiceFeedback::doVoiceFeedback(const char *text){
	doVoiceFeedback(text,randomString(5));
}*/
void VoiceFeedback::doVoiceFeedback(const char *text, const char *name){
	/*int spd = config->getVoiceSpeed();
	if(spd != voicespeed)
		updateVoiceSpeed(spd);*/
	size = strlen(text)+1;
	tsize=size;
	isFname = true;
	strcpy(fname,"");

	strcat(fname,"Assets/");
	strcat(fname,name);
	strcat(fname,".wav");
	//const char* filen = (const char*) malloc(sizeof(strlen(fname)));
	//strcpy(filen,fname);

	//cout << fname << endl;
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Saying: " << text;
	try{
		espeak_Synth(text, size, pos, POS_SENTENCE,0,flags,&uniqueid,this);
		espeak_Synchronize();

		BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
		//audio.doTTSFeedbackPos(fname,0);
	}
	catch(exception err){
		err.what();
	}
}


void VoiceFeedback::doVoiceFeedback(const char *text, const char *name, const char *nlang){
	/*int spd = config->getVoiceSpeed();
	updateVoiceSpeed(spd);*/
	size = strlen(text)+1;
	tsize=size;
	isFname = true;
	strcpy(fname,"");

	strcat(fname,"Assets/");
	strcat(fname,name);
	strcat(fname,".wav");


	//cout << fname << endl;
	BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Saying: " << text;
	try{

		espeak_Synth(text, size, pos, POS_SENTENCE,0,flags,&uniqueid,this);
		espeak_Synchronize();

		BOOST_LOG_TRIVIAL(info) << "(VoiceFeedback) Done!";
		//audio.doTTSFeedbackPos(fname,0);
	}
	catch(exception err){
		err.what();
	}
}


bool VoiceFeedback::updateLanguage(const char *newlang){
	lang = newlang;
	return espeak_SetVoiceByName(lang) > 0;
}

void VoiceFeedback::updateVoiceSpeed(int speed){
	espeak_SetParameter(espeakRATE, speed,0);
}

void VoiceFeedback::updateVoiceGap(int gap){
	espeak_SetParameter(espeakWORDGAP, gap,0);
}

VoiceFeedback::~VoiceFeedback() {
	/*espeak_Synchronize();
	while(espeak_IsPlaying() > 0)
		continue;
	espeak_Terminate();*/
}


