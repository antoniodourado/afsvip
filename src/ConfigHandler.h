/* ConfigHandler.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_CONFIGHANDLER_H_
#define SRC_CONFIGHANDLER_H_

#include <libconfig.h++>
#include <iostream>
#include <exception>
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <alsa/asoundlib.h>
#include "__afsvipGLOBALS.h"

using namespace std;
using namespace libconfig;

enum FeedbackMethod{
	Acoustic=0,
	Tactile=1,
	Both=2,
	First=Acoustic,
	Last=Both
};

enum NavigationType{
	Path=0,
	Obstacle=1,
	BothNT=2,
	FirstNT=Path,
	LastNT=BothNT
};

enum FeedbackSceneMode{
	Active=0,
	Passive=1
};

enum FeedbackAcousticType{
	Tone=0,
	Voice=1
};

enum FeedbackAcousticDirection{
	Positional=0,
	Stereo=1
};

enum DistanceType{
	Meters=0,
	Paces=1
};

enum KBCommands{
			SYSTEM_CLASSIFY,
			SYSTEM_OBSTACLE_DISTANCE,
			SYSTEM_SHOW_PATH,
			SYSTEM_TURN_OFF,
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
			SYSTEM_HELP_COMMANDS,
			SYSTEM_HELP_STOP,
			SYSTEM_FEEDBACK_STOP,
			SYSTEM_FEEDBACK_START,
			SYSTEM_SETUP_ENABLE,
			SYSTEM_SETUP_DISABLE,
			SYSTEM_VOICELISTENER_TOGGLE,
			SYSTEM_INCREASE_MAXIMUM_DISTANCE,
			SYSTEM_DECREASE_MAXIMUM_DISTANCE,
			SYSTEM_INCREASE_MINIMUM_DISTANCE,
			SYSTEM_DECREASE_MINIMUM_DISTANCE,
			SYSTEM_VOICE_SPEED_INCREASE,
			SYSTEM_VOICE_SPEED_DECREASE,
			SYSTEM_VOICE_GAP_INCREASE,
			SYSTEM_VOICE_GAP_DECREASE
		};


class ConfigHandler {
public:
	const char *configfile = "Assets/config.cfg";
	ConfigHandler();
	virtual ~ConfigHandler();
	void reloadConfig();
	float getMaximumDistance();
	float getMinimumDistance();
	void increaseMaximumDistance();
	void decreaseMaximumDistance();
	void increaseMinimumDistance();
	void decreaseMinimumDistance();
	void increaseSystemVolume();
	void decreaseSystemVolume();
	void setSystemMinVol();
	void setSystemDefVol();
	void setSystemHighVol();
	float getSystemAPIVolume();
	bool isVoiceFeedback();
	void setToneFeedback();
	void setVoiceFeedback();
	FeedbackAcousticType getAcousticFeedbackType();
	void setAcousticFeedbackType(FeedbackAcousticType);
	bool isPathNavigation();
	void setNavigationType(string);
	void setNavigationType(NavigationType);
	NavigationType getNavigationType();
	bool isPositionalFeedback();
	void setPosStereoFeedback(string);
	void setPosStereoFeedback(FeedbackAcousticDirection);
	void setDistanceType(string);
	void setDistanceType(DistanceType);
	bool isDistanceTypeMeters();
	float getMPacesVal();
	bool isCommandPtBr();
	void setCommandLang(string);
	int getFeedbackSpeed();
	void updateFeedbackSpeed(int);
	void setSceneContFeedback(string);
	void setSceneContFeedback(FeedbackSceneMode);
	bool isContinuousFeedback();
	float getNavigationDistance();
	void setNavigationDistance(float);
	float getDistance();
	void updateDetectionDistance(float);
	void updateSystemVolume(float);
	void setDistance(float);
	void setKeypadFunction(string,int);
	int getKeypadFunction(string);
	void setSystemVolume(int vol);
	int getSystemVolume();
	FeedbackMethod getFeedbackMethod();
	void setFeedbackMethod(FeedbackMethod);
	int getVoiceSpeed();
	void setVoiceSpeed(int speed);
	int getVoiceGap();
	void setVoiceGap(int gap);
	int getUserHeight();
	void setUserHeight(int);



	std::vector<string> commands = {
			"SYSTEM CLASSIFY",
			"SYSTEM OBSTACLE DISTANCE",
			"SYSTEM SHOW PATH",
			"SYSTEM TURN OFF",
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
			"SYSTEM HELP COMMANDS",
			"SYSTEM HELP STOP",
			"SYSTEM FEEDBACK STOP",
			"SYSTEM FEEDBACK START",
			"SYSTEM SETUP ENABLE",
			"SYSTEM SETUP DISABLE",
			"SYSTEM VOICE LISTENER TOGGLE",
			"SYSTEM INCREASE MAXIMUM DISTANCE",
			"SYSTEM DECREASE MAXIMUM DISTANCE",
			"SYSTEM INCREASE MINIMUM DISTANCE",
			"SYSTEM DECREASE MINIMUM DISTANCE",
			"SYSTEM VOICE SPEED INCREASE",
			"SYSTEM VOICE SPEED DECREASE",
			"SYSTEM VOICE GAP INCREASE",
			"SYSTEM VOICE GAP DECREASE"
	};


private:
	Config cfg;
	void buildNewConfigFile();
	float volstep = 0.025;
	int volpstep = 5, defsvol = 50, minsvol=20, maxsvol=80;
	float defvol = 0.25;
};

#endif /* SRC_CONFIGHANDLER_H_ */
