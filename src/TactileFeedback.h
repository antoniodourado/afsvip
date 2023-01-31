/*
 * TactileFeedback.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC_TACTILEFEEDBACK_H_
#define SRC_TACTILEFEEDBACK_H_
#include "jetsonGPIO.h"
#include "__afsvipGLOBALS.h"
#include <vector>
#include <thread>
#include "Configuration.h"

using namespace jetsonNano;
//using namespace jetsonTX2;

enum FeedbackType{
	SAFEPATH = 0,
	OBSTACLE = 1
};

enum Direction{
	left = 1,
	s_left = 2,
	front = 3,
	s_right = 4,
	right = 5,
	ls_left = 6,
	rs_right = 7,
	left_right = 8,
	ls_right = 9,
	rs_left = 10,
	all = 11,
	back = 12,
	custom = 13,
	none=99
};

class TactileFeedback {
public:
	TactileFeedback(Configuration*);
	virtual ~TactileFeedback();
	std::thread _doFeedback(Direction,FeedbackType);
	void doFeedback(Direction,FeedbackType);
	void doCustomFeedback(Direction,unsigned int,unsigned int);
	void doCustomTypeFeedback(Direction,VibrationType);
	std::thread _doCustomFeedback(Direction,unsigned int,unsigned int);
	std::thread _doCustomTypeFeedback(Direction,VibrationType);
private:
	jetsonNanoGPIONumber dir,pdir,fr,pesq,esq;
	vector<jetsonGPIO> buzzer{gpio17};
	/*vector<jetsonGPIO> right{gpio38};
	vector<jetsonGPIO> left{gpio200};
	vector<jetsonGPIO> sleft{gpio149};
	vector<jetsonGPIO> sright{gpio168};
	vector<jetsonGPIO> front{gpio12};
	vector<jetsonGPIO> back{gpio38};
	vector<jetsonGPIO> allpins{gpio149,gpio38,gpio200,gpio168,gpio12};*/
	vector<jetsonGPIO> right;
	vector<jetsonGPIO> left;
	vector<jetsonGPIO> sleft;
	vector<jetsonGPIO> sright;
	vector<jetsonGPIO> front;
	vector<jetsonGPIO> back;
	vector<jetsonGPIO> allpins;
	Configuration* config;
	unsigned int tactileDuration = 200000;
	unsigned int repeatTimes = 1;
	struct tactileSignalData{
		unsigned int tactileDuration = 200000;
		unsigned int repeatTimes = 1;
	};
	struct tactileSignalData safePathSignal;
	struct tactileSignalData obstacleSignal;
	int cHeight,h1,h2,h3;
private:
	void gpioBatchExport(vector<jetsonGPIO>);
	void gpioBatchUnexport(vector<jetsonGPIO>);
	void gpioSetPinsValues(vector<jetsonGPIO>,pinValue);
	void gpioSetPinsInOut(vector<jetsonGPIO>,pinDirection);
	void gpioSendFeedback(vector<jetsonGPIO>,struct tactileSignalData);
	void gpioSendCustomFeedback(vector<jetsonGPIO>,unsigned int,unsigned int);
	vector<jetsonGPIO> joinPins(vector<vector<jetsonGPIO>>);
};

#endif /* SRC_TACTILEFEEDBACK_H_ */
