/*
 * Configuration.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC_CONFIGURATION_H_
#define SRC_CONFIGURATION_H_

#include "ConfigHandler.h"

class Configuration
{
public:
	Configuration(ConfigHandler *);
	virtual ~Configuration();
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
	FeedbackAcousticType getAcousticFeedbackType();
	void setAcousticFeedbackType(FeedbackAcousticType);
	bool isVoiceFeedback();
	void setToneFeedback();
	void setVoiceFeedback();
	bool isPathNavigation();
	void setNavigationType(string);
	bool isPositionalFeedback();
	void setPosStereoFeedback(string);
	void setDistanceType(string);
	bool isDistanceTypeMeters();
	float getMPacesVal();
	bool isCommandPtBr();
	void setCommandLang(string);
	int getFeedbackSpeed();
	void updateFeedbackSpeed(int);
	void setSceneContFeedback(string);
	void setSceneContFeedback(FeedbackSceneMode);
	void setPosStereoFeedback(FeedbackAcousticDirection);
	bool isContinuousFeedback();
	float getNavigationDistance();
	void setNavigationDistance(float);
	NavigationType getNavigationType();
	void setNavigationType(NavigationType);
	void setDistanceType(DistanceType);
	float getDistance();
	void setDistance(float);
	void setKeypadFunction(string, int);
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
	void saveChangedConfigs();
	pair<string, int> changeEditableKey(string);

	static string getFeedbackSceneDesc(FeedbackSceneMode);
	static string getFeedbackAcousticTypeDesc(FeedbackAcousticType);
	static string getFeedbackDirectionDesc(FeedbackAcousticDirection);
	static string getFeedbackMethodDesc(FeedbackMethod);
	static string getDistanceDesc(DistanceType);
	static string getNavigationTypeDesc(NavigationType);

private:
	ConfigHandler *cfg;
	float volstep = 0.025;
	int volpstep = 5, defsvol = 50, minsvol = 20, maxsvol = 80;
	struct userConfig
	{
		float detection_distance;
		int feedback_tonetimes;
		float feedback_pacevalue;
		FeedbackAcousticType feedback_type;
		FeedbackSceneMode feedback_contscene;
		int feedback_svolume;
		DistanceType feedback_distancetype;
		float feedback_volume;
		FeedbackAcousticDirection feedback_positional;
		int voice_speed;
		int voice_gap;
		int feedback_speed;
		FeedbackMethod feedback_method;
		float navigation_distance;
		NavigationType navigation_type;
		std::map<string, int> keymap;
		int height;
	} localConfig;

	enum ConfigList
	{
		DETECTION_DISTANCE_CFG,
		FEEDBACK_TONETIMES_CFG,
		FEEDBACK_PACEVALUE_CFG,
		FEEDBACK_TYPE_CFG,
		FEEDBACK_CONTSCENE_CFG,
		FEEDBACK_SVOLUME_CFG,
		FEEDBACK_DISTANCETYPE_CFG,
		FEEDBACK_VOLUME_CFG,
		FEEDBACK_POSITIONAL_CFG,
		VOICE_SPEED_CFG,
		VOICE_GAP_CFG,
		FEEDBACK_SPEED_CFG,
		FEEDBACK_METHOD_CFG,
		NAVIGATION_DISTANCE_CFG,
		NAVIGATION_TYPE_CFG,
		KEYMAP_CFG,
		HEIGHT_CFG
	};

	std::list<ConfigList> changedConfigs;

	/*struct changedConfig{
		bool detection_distance;
		bool feedback_tonetimes;
		bool feedback_pacevalue;
		bool feedback_type;
		bool feedback_contscene;
		bool feedback_svolume;
		bool feedback_distancetype;
		bool feedback_volume;
		bool feedback_positional;
		bool voice_speed;
		bool voice_gap;
		bool feedback_speed;
		bool feedback_method;
		bool navigation_distance;
		bool navigation_type;
		bool keymap;
		bool height;
	};*/

	float defvol = 0.25;
};

#endif /* SRC_CONFIGURATION_H_ */
