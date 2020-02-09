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
 * VoiceHypotesisProcessor.h
 *
 *  Created on: 13 de out de 2016
 *      Author: antonio
 */

#ifndef SRC_VOICEHYPOTESISPROCESSOR_H_
#define SRC_VOICEHYPOTESISPROCESSOR_H_
#include <iostream>
#include "CodeUtil.h"
#include <string.h>
#include "FeedbackLayer.h"
#include <thread>
#include "__afsvipGLOBALS.h"
#include "Configuration.h"
using namespace std;

class VoiceHypotesisProcessor {
public:
	VoiceHypotesisProcessor();
	VoiceHypotesisProcessor(FeedbackLayer*);
	VoiceHypotesisProcessor(FeedbackLayer*,Configuration*);
	virtual ~VoiceHypotesisProcessor();
	void doProcess(const char*);
	std::thread doProcessString(const char*);
	std::thread doProcessString(string);
	vector<string> getCommands();
private:

	FeedbackLayer* feedback = NULL;
	Configuration* config = NULL;
	const char* hyp = NULL;
	string shyp;
	const unsigned int maxfreecommand = 11;
	enum VoiceCommands{
		SYSTEM_CLASSIFY,
		SYSTEM_SETUP_ENABLE,
		SYSTEM_SETUP_DISABLE,
		SYSTEM_OBSTACLE_DISTANCE,
		SYSTEM_SHOW_PATH,
		SYSTEM_FEEDBACK_STOP,
		SYSTEM_FEEDBACK_START,
		SYSTEM_TURN_OFF,
		SYSTEM_HELP_COMMANDS,
		SYSTEM_HELP_STOP,
		DEBUG_SNAPSHOT,
		SYSTEM_DISTANCE_INCREASE,
		SYSTEM_DISTANCE_DECREASE,
		SYSTEM_FEEDBACK_TONE,
		SYSTEM_FEEDBACK_VOICE,
		SYSTEM_VOLUME_HIGHER,
		SYSTEM_VOLUME_LOWER,
		SYSTEM_VOLUME_MINIMUM,
		SYSTEM_VOLUME_LOUD,
		SYSTEM_VOLUME_DEFAULT,
		SYSTEM_NAVIGATION_TYPE_PATH,
		SYSTEM_NAVIGATION_TYPE_OBSTACLE,
		SYSTEM_NAVIGATION_DISTANCE_MINIMUM,
		SYSTEM_NAVIGATION_DISTANCE_DEFAULT,
		SYSTEM_NAVIGATION_DISTANCE_MAXIMUM,
		SYSTEM_FEEDBACK_TYPE_POSITIONAL,
		SYSTEM_FEEDBACK_TYPE_STEREO,
		SYSTEM_DISTANCE_TYPE_METERS,
		SYSTEM_DISTANCE_TYPE_PACES,
		SYSTEM_FEEDBACK_CONTINUOUS,
		SYSTEM_FEEDBACK_SCENE,
		SYSTEM_FEEDBACK_SPEED_INCREASE,
		SYSTEM_FEEDBACK_SPEED_DECREASE,
		SYSTEM_INCREASE_MAXIMUM_DISTANCE,
		SYSTEM_DECREASE_MAXIMUM_DISTANCE,
		SYSTEM_INCREASE_MINIMUM_DISTANCE,
		SYSTEM_DECREASE_MINIMUM_DISTANCE
	};
	std::vector<string> commands = {
			"SYSTEM CLASSIFY",
			"SYSTEM SETUP ENABLE",
			"SYSTEM SETUP DISABLE",
			"SYSTEM OBSTACLE DISTANCE",
			"SYSTEM SHOW PATH",
			"SYSTEM FEEDBACK STOP",
			"SYSTEM FEEDBACK START",
			"SYSTEM TURN OFF",
			"SYSTEM HELP COMMANDS",
			"SYSTEM HELP STOP",
			"DEBUG SNAPSHOT",
			"SYSTEM DISTANCE INCREASE",
			"SYSTEM DISTANCE DECREASE",
			"SYSTEM FEEDBACK TONE",
			"SYSTEM FEEDBACK VOICE",
			"SYSTEM VOLUME HIGHER",
			"SYSTEM VOLUME LOWER",
			"SYSTEM VOLUME MINIMUM",
			"SYSTEM VOLUME LOUD",
			"SYSTEM VOLUME DEFAULT",
			"SYSTEM NAVIGATION TYPE PATH",
			"SYSTEM NAVIGATION TYPE OBSTACLE",
			"SYSTEM NAVIGATION DISTANCE MINIMUM",
			"SYSTEM NAVIGATION DISTANCE DEFAULT",
			"SYSTEM NAVIGATION DISTANCE MAXIMUM",
			"SYSTEM FEEDBACK TYPE POSITIONAL",
			"SYSTEM FEEDBACK TYPE STEREO",
			"SYSTEM DISTANCE TYPE METERS",
			"SYSTEM DISTANCE TYPE PACES",
			"SYSTEM FEEDBACK CONTINUOUS",
			"SYSTEM FEEDBACK SCENE",
			"SYSTEM FEEDBACK SPEED INCREASE",
			"SYSTEM FEEDBACK SPEED DECREASE",
			"SYSTEM INCREASE MAXIMUM DISTANCE",
			"SYSTEM DECREASE MAXIMUM DISTANCE",
			"SYSTEM INCREASE MINIMUM DISTANCE",
			"SYSTEM DECREASE MINIMUM DISTANCE"
	};

};

#endif /* SRC_VOICEHYPOTESISPROCESSOR_H_ */
