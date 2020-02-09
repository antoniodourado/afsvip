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
 * VoiceHypotesisProcessor.cpp
 *
 *  Created on: 13 de out de 2016
 *      Author: antonio
 */

#include "VoiceHypotesisProcessor.h"


VoiceHypotesisProcessor::VoiceHypotesisProcessor() {
	// TODO Auto-generated constructor stub

}

VoiceHypotesisProcessor::~VoiceHypotesisProcessor() {
	// TODO Auto-generated destructor stub
}

VoiceHypotesisProcessor::VoiceHypotesisProcessor(FeedbackLayer* f,Configuration* c) {
	feedback = f;
	config=c;
}

VoiceHypotesisProcessor::VoiceHypotesisProcessor(FeedbackLayer* f) {
	feedback = f;
}

void VoiceHypotesisProcessor::doProcess(const char* phrase) {

	if(strlen(phrase) <= 0)
		return;

	string shyp = phrase;
	shyp = trim(shyp);

	unsigned int pos = find(commands.begin(),commands.end(),shyp) - commands.begin();


	if(pos >= commands.size() || (pos > maxfreecommand && !gvars.setup))
		return;
	switch(pos){
		case SYSTEM_CLASSIFY:
			feedback->voiceObjectClassification();
			break;
		case SYSTEM_SETUP_ENABLE:
			gvars.setup = true;
			feedback->callVoiceFeedbackText("Modo de customização habilitado.");
			break;
		case SYSTEM_SETUP_DISABLE:
			gvars.setup = false;
			feedback->callVoiceFeedbackText("Modo de customização desabilitado.");
			break;
		case SYSTEM_OBSTACLE_DISTANCE:
			feedback->feedbackObstacleDistance();
			break;
		case SYSTEM_SHOW_PATH:
			feedback->repeatLastFoundPath();
			break;
		case SYSTEM_FEEDBACK_STOP:
			if(!gvars.stopSystemFeedback){
				gvars.stopSystemFeedback = true;
				feedback->callVoiceFeedbackText("Resposta do Sistema em Espera.");
			}
			break;
		case SYSTEM_FEEDBACK_START:
			if(gvars.stopSystemFeedback){
				feedback->callVoiceFeedbackText("Retomando Resposta do Sistema.");
				gvars.stopSystemFeedback = false;
			}
			break;
		case SYSTEM_TURN_OFF:
			gvars.haltSystem = true;
			break;
		case SYSTEM_HELP_COMMANDS:
			//feedback->helpCommands(commands);
			break;
		case SYSTEM_HELP_STOP:
			//feedback->stopHelp();
			break;
		case DEBUG_SNAPSHOT:
			if(gvars.SYSDEBUGMODE)
				gvars.takeSnapshot = true;
			break;
		case SYSTEM_DISTANCE_INCREASE:
			config->increaseMaximumDistance();
			break;
		case SYSTEM_DISTANCE_DECREASE:
			config->decreaseMaximumDistance();
			break;
		case SYSTEM_FEEDBACK_TONE:
			config->setAcousticFeedbackType(FeedbackAcousticType::Tone);
			break;
		case SYSTEM_FEEDBACK_VOICE:
			config->setAcousticFeedbackType(FeedbackAcousticType::Voice);
			break;
		case SYSTEM_VOLUME_HIGHER:
			config->increaseSystemVolume();
			feedback->callVoiceFeedbackText("Ok.");
			break;
		case SYSTEM_VOLUME_LOWER:
			config->decreaseSystemVolume();
			feedback->callVoiceFeedbackText("Ok.");
			break;
		case SYSTEM_VOLUME_MINIMUM:
			config->setSystemMinVol();
			feedback->callVoiceFeedbackText("Ok.");
			break;
		case SYSTEM_VOLUME_LOUD:
			config->setSystemHighVol();
			feedback->callVoiceFeedbackText("Ok.");
			break;
		case SYSTEM_VOLUME_DEFAULT:
			config->setSystemDefVol();
			feedback->callVoiceFeedbackText("Ok.");
			break;
		case SYSTEM_NAVIGATION_TYPE_PATH:
			config->setNavigationType(NavigationType::Path);
			break;
		case SYSTEM_NAVIGATION_TYPE_OBSTACLE:
			config->setNavigationType(NavigationType::Obstacle);
			break;
		case SYSTEM_NAVIGATION_DISTANCE_MINIMUM:
			config->setNavigationDistance(70);
			break;
		case SYSTEM_NAVIGATION_DISTANCE_DEFAULT:
			config->setNavigationDistance(100);
			break;
		case SYSTEM_NAVIGATION_DISTANCE_MAXIMUM:
			config->setNavigationDistance(130);
			break;
		case SYSTEM_FEEDBACK_TYPE_POSITIONAL:
			config->setPosStereoFeedback(FeedbackAcousticDirection::Positional);
			break;
		case SYSTEM_FEEDBACK_TYPE_STEREO:
			config->setPosStereoFeedback(FeedbackAcousticDirection::Stereo);
			break;
		case SYSTEM_DISTANCE_TYPE_METERS:
			config->setDistanceType(DistanceType::Meters);
			break;
		case SYSTEM_DISTANCE_TYPE_PACES:
			config->setDistanceType(DistanceType::Paces);
			break;
		case SYSTEM_FEEDBACK_CONTINUOUS:
			config->setSceneContFeedback(FeedbackSceneMode::Active);
			break;
		case SYSTEM_FEEDBACK_SCENE:
			config->setSceneContFeedback(FeedbackSceneMode::Passive);
			break;
		case SYSTEM_FEEDBACK_SPEED_INCREASE:
			config->updateFeedbackSpeed(config->getFeedbackSpeed()-1); //diminui um pois é o multiplicador do sleep na camada de visão, ou seja, o sleep é menor.
			feedback->callVoiceFeedbackText(to_string(config->getFeedbackSpeed()).c_str());
			break;
		case SYSTEM_FEEDBACK_SPEED_DECREASE:
			config->updateFeedbackSpeed(config->getFeedbackSpeed()+1);
			feedback->callVoiceFeedbackText(to_string(config->getFeedbackSpeed()).c_str());
			break;
		case SYSTEM_INCREASE_MAXIMUM_DISTANCE:
			config->increaseMaximumDistance();
			break;
		case SYSTEM_DECREASE_MAXIMUM_DISTANCE:
			config->decreaseMaximumDistance();
			break;
		case SYSTEM_INCREASE_MINIMUM_DISTANCE:
			//config->increaseDetectionDistance();
			break;
		case SYSTEM_DECREASE_MINIMUM_DISTANCE:
			//config->decreaseDetectionDistance();
			break;
	}
}

std::thread VoiceHypotesisProcessor::doProcessString(const char* s) {
	return std::thread( [=] {doProcess(s);} );
}

std::thread VoiceHypotesisProcessor::doProcessString(string s) {
	return std::thread( [=] {doProcess(s.c_str());} );
}

vector<string> VoiceHypotesisProcessor::getCommands() {
	return commands;
}
