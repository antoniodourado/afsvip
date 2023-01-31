/*
 * VoiceHypotesisProcessor.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "VoiceHypotesisProcessor.h"

VoiceHypotesisProcessor::VoiceHypotesisProcessor()
{
	// TODO Auto-generated constructor stub
}

VoiceHypotesisProcessor::~VoiceHypotesisProcessor()
{
	// TODO Auto-generated destructor stub
}

VoiceHypotesisProcessor::VoiceHypotesisProcessor(FeedbackLayer *f, Configuration *c)
{
	feedback = f;
	config = c;
}

VoiceHypotesisProcessor::VoiceHypotesisProcessor(FeedbackLayer *f)
{
	feedback = f;
}

void VoiceHypotesisProcessor::doProcess(const char *phrase)
{

	if (strlen(phrase) <= 0)
		return;

	string shyp = phrase;
	shyp = trim(shyp);

	unsigned int pos = find(commands.begin(), commands.end(), shyp) - commands.begin();

	if (pos >= commands.size() || (pos > maxfreecommand && !gvars.setup))
		return;
	switch (pos)
	{
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
		if (!gvars.stopSystemFeedback)
		{
			gvars.stopSystemFeedback = true;
			feedback->callVoiceFeedbackText("Resposta do Sistema em Espera.");
		}
		break;
	case SYSTEM_FEEDBACK_START:
		if (gvars.stopSystemFeedback)
		{
			feedback->callVoiceFeedbackText("Retomando Resposta do Sistema.");
			gvars.stopSystemFeedback = false;
		}
		break;
	case SYSTEM_TURN_OFF:
		gvars.haltSystem = true;
		break;
	case SYSTEM_HELP_COMMANDS:
		// feedback->helpCommands(commands);
		break;
	case SYSTEM_HELP_STOP:
		// feedback->stopHelp();
		break;
	case DEBUG_SNAPSHOT:
		if (gvars.SYSDEBUGMODE)
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
		config->updateFeedbackSpeed(config->getFeedbackSpeed() - 1); // diminui um pois é o multiplicador do sleep na camada de visão, ou seja, o sleep é menor.
		feedback->callVoiceFeedbackText(to_string(config->getFeedbackSpeed()).c_str());
		break;
	case SYSTEM_FEEDBACK_SPEED_DECREASE:
		config->updateFeedbackSpeed(config->getFeedbackSpeed() + 1);
		feedback->callVoiceFeedbackText(to_string(config->getFeedbackSpeed()).c_str());
		break;
	case SYSTEM_INCREASE_MAXIMUM_DISTANCE:
		config->increaseMaximumDistance();
		break;
	case SYSTEM_DECREASE_MAXIMUM_DISTANCE:
		config->decreaseMaximumDistance();
		break;
	case SYSTEM_INCREASE_MINIMUM_DISTANCE:
		config->increaseDetectionDistance();
		break;
	case SYSTEM_DECREASE_MINIMUM_DISTANCE:
		config->decreaseDetectionDistance();
		break;
	case SYSTEM_VOICE_GAP_INCREASE:
		int vspd = cv::min(config->getVoiceSpeed() + 1, 1000);
		config->setVoiceSpeed(vspd);
		VoiceFeedback::updateVoiceSpeed(vspd);
		break;
	case SYSTEM_VOICE_GAP_DECREASE:
		int vspd = cv::min(config->getVoiceSpeed() - 1, 1000);
		config->setVoiceSpeed(vspd);
		VoiceFeedback::updateVoiceSpeed(vspd);
		break;
	case SYSTEM_VOICE_GAP_INCREASE:
		int vgap = cv::min(config->getVoiceGap() + 1, 1000);
		config->setVoiceGap(vgap);
		VoiceFeedback::updateVoiceGap(vgap);
		break;
	case SYSTEM_VOICE_GAP_DECREASE:
		int vgap = cv::min(config->getVoiceGap() - 1, 1000);
		config->setVoiceGap(vgap);
		VoiceFeedback::updateVoiceGap(vgap);
		break;
	}
}

std::thread VoiceHypotesisProcessor::doProcessString(const char *s)
{
	return std::thread([=]
					   { doProcess(s); });
}

std::thread VoiceHypotesisProcessor::doProcessString(string s)
{
	return std::thread([=]
					   { doProcess(s.c_str()); });
}

vector<string> VoiceHypotesisProcessor::getCommands()
{
	return commands;
}
