/*
 * FeedbackLayer.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_FEEDBACKLAYER_H_
#define SRC_FEEDBACKLAYER_H_
#include "VoiceFeedback.h"
#include "AudioFeedback.h"
#include "DetectedObject.h"
#include "CaffeClassifier.h"
#include "CodeUtil.h"
#include "TactileFeedback.h"
#include <list>
#include "__afsvipGLOBALS.h"
#include "Configuration.h"

class FeedbackLayer {
public:
	FeedbackLayer(AudioFeedback*,Configuration*,VoiceFeedback*);
	FeedbackLayer(AudioFeedback*,Configuration*,VoiceFeedback*,TactileFeedback*);
	virtual ~FeedbackLayer();
	const char* doObjectDetectionFeedback(DetectedObject*);
	void voiceObjectClassification();
	const char* doPathNavigationFeedback(pair<int,int>,int);
	list<string> doPathNavigationFeedback(list<pair<int,int>>,int);
	void doPathNotFoundFeedback();
	void callVoiceFeedbackText(const char*);
	void callTactileFeedback(Direction,int,int);
	void callTactileFeedback(Direction,VibrationType);
	void repeatLastFoundPath();
	void feedbackObstacleDistance();
	char* getDistanceString(float);
	static const char* getMeasurementString(float,bool);
	void helpCommands(vector<string>);
	void stopHelp();

	void callAutoFeedback(FeedbackMethod,int,...);
	void callAutoFeedback(const char*,Direction,VibrationType);
	void callAutoFeedback(const char*,Direction,int,int);

private:
	CaffeClassifier* cclass = new CaffeClassifier();
	TactileFeedback* tactile = NULL;
	VoiceFeedback* voice = NULL;
	AudioFeedback* audio = NULL;
	Configuration* config = NULL;
	DetectedObject* object = NULL;
	DetectedObject* lobject = NULL;
	DetectedObject lastObject;
	CodeUtil cutil;
	float lastFoundPath;
	bool stopHelpVar;
	thread vthread;
	ALint *sstate = NULL;


private:
	float getLRAudioPosFromObj(DetectedObject*);
	float getLRAudioPosFromXCoord(int,int);
	Direction getFeedbackDirection(float);
	char* getDirectionalString(float);
	string float2string(float);
	bool checkRecentlyDetectedObj();
};

#endif /* SRC_FEEDBACKLAYER_H_ */
