/*
 * TactileFeedback.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "TactileFeedback.h"

TactileFeedback::TactileFeedback(Configuration* c) {
	config = c;
	left.push_back(gvars.reducedMotors ? gpio149 : gpio200);
	right.push_back(gvars.reducedMotors ? gpio168 : gpio38);
	sleft.push_back(gpio149);
	sright.push_back(gpio168);
	front.push_back(gvars.reducedMotors ? gpio200 : gpio12);
	back.push_back(gvars.reducedMotors ? gpio38 : gpio12);
	if(gvars.reducedMotors)
		allpins = {gpio149,gpio168,gpio200,gpio38};
	else
		allpins = {gpio149,gpio168,gpio200,gpio38,gpio12};


	gpioBatchExport(allpins);
	gpioSetPinsInOut(allpins,outputPin);
	gpioSetPinsValues(allpins,off);
	gpioSetPinsValues(buzzer,on);
	obstacleSignal.repeatTimes=3;
	safePathSignal.tactileDuration=500000;
}

TactileFeedback::~TactileFeedback() {
	gpioSetPinsValues(allpins,off);
	gpioBatchUnexport(allpins);
}

void TactileFeedback::doFeedback(Direction direction,
		FeedbackType type) {
	struct tactileSignalData signal;
	switch(type){
	case SAFEPATH:
		signal = safePathSignal;
		break;
	case OBSTACLE:
		signal = obstacleSignal;
		break;
	}
	switch(direction){
	case Direction::left:
		gpioSendFeedback(left,signal);
		break;
	case Direction::s_left:
		gpioSendFeedback(sleft,signal);
		break;
	case Direction::front:
		gpioSendFeedback(front,signal);
		break;
	case Direction::s_right:
		gpioSendFeedback(sright,signal);
		break;
	case Direction::right:
		gpioSendFeedback(right,signal);
		break;
	case Direction::ls_left:
		gpioSendFeedback(joinPins({left,sleft}),signal);
		break;
	case Direction::rs_right:
		gpioSendFeedback(joinPins({right,sright}),signal);
		break;
	case Direction::left_right:
		gpioSendFeedback(joinPins({left,right}),signal);
		break;
	case Direction::ls_right:
		gpioSendFeedback(joinPins({left,sright}),signal);
		break;
	case Direction::rs_left:
		gpioSendFeedback(joinPins({right,sleft}),signal);
		break;
	case Direction::back:
		gpioSendFeedback(back,signal);
		break;
	case Direction::all:
		gpioSendFeedback(allpins,signal);
		break;
	}
}

void TactileFeedback::doCustomFeedback(Direction dir,unsigned int times, unsigned int duration) {
	switch(dir){
	case Direction::left:
		gpioSendCustomFeedback(left,times,duration);
		break;
	case Direction::s_left:
		gpioSendCustomFeedback(sleft,times,duration);
		break;
	case Direction::front:
		gpioSendCustomFeedback(front,times,duration);
		break;
	case Direction::s_right:
		gpioSendCustomFeedback(sright,times,duration);
		break;
	case Direction::right:
		gpioSendCustomFeedback(right,times,duration);
		break;
	case Direction::ls_left:
		gpioSendCustomFeedback(joinPins({left,sleft}),times,duration);
		break;
	case Direction::rs_right:
		gpioSendCustomFeedback(joinPins({right,sright}),times,duration);
		break;
	case Direction::left_right:
		gpioSendCustomFeedback(joinPins({left,right}),times,duration);
		break;
	case Direction::ls_right:
		gpioSendCustomFeedback(joinPins({left,sright}),times,duration);
		break;
	case Direction::rs_left:
		gpioSendCustomFeedback(joinPins({right,sleft}),times,duration);
		break;
	case Direction::back:
		gpioSendCustomFeedback(back,times,duration);
		break;
	case Direction::all:
		gpioSendCustomFeedback(allpins,times,duration);
		break;
	}
}

void TactileFeedback::doCustomTypeFeedback(Direction dir,VibrationType vtype) {
	switch(vtype){
	case VibrationType::OPT_IN:
		doCustomFeedback(dir,1,500000);
		break;
	case VibrationType::OPT_OUT:
		doCustomFeedback(dir,1,500000);
		break;
	case VibrationType::ACK:
		doCustomFeedback(dir,1,250000);
		break;
	case VibrationType::SAVED:
		doCustomFeedback(dir,3,300000);
		break;
	case VibrationType::UHEIGHT:
		cHeight=config->getUserHeight();
		h1=(cHeight/100);
		doCustomFeedback(Direction::left,config->getUserHeight()/100,150);
		h2=(cHeight-(h1*100))/10;
		doCustomFeedback(Direction::front,h2,150);
		h3=(cHeight-(h1*100))%10;
		doCustomFeedback(Direction::right,h3,150);
		break;
	case VibrationType::CAM_RAISE:
	case VibrationType::CAM_LOWER:
		doCustomFeedback(dir,1,300000);
		break;
	case VibrationType::ERROR:
		doCustomFeedback(dir,2,250000);
		break;
	case VibrationType::REQ_PHONE:
		doCustomFeedback(dir,5,250000);
		break;
	case VibrationType::VIB_1:
		doCustomFeedback(dir,1,250000);
		break;
	case VibrationType::VIB_2:
		doCustomFeedback(dir,2,250000);
		break;
	case VibrationType::VIB_3:
		doCustomFeedback(dir,3,250000);
		break;
	case VibrationType::VIB_4:
		doCustomFeedback(dir,4,250000);
		break;
	case VibrationType::VIB_5:
		doCustomFeedback(dir,5,250000);
		break;
	case VibrationType::VIB_6:
		doCustomFeedback(dir,6,250000);
		break;
	case VibrationType::VIB_7:
		doCustomFeedback(dir,7,250000);
		break;
	case VibrationType::VIB_8:
		doCustomFeedback(dir,8,250000);
		break;
	case VibrationType::VIB_9:
		doCustomFeedback(dir,9,250000);
		break;
	case VibrationType::VIB_0:
		doCustomFeedback(dir,1,250000);
		break;
	}
}

void TactileFeedback::gpioSetPinsValues(vector<jetsonGPIO> pins,
		pinValue value) {
	for(jetsonGPIO pin : pins)
		gpioSetValue(pin,value);
}

void TactileFeedback::gpioSetPinsInOut(vector<jetsonGPIO> pins,
		pinDirection direction) {
	for(jetsonGPIO pin : pins)
		gpioSetDirection(pin,direction);
}

void TactileFeedback::gpioBatchExport(vector<jetsonGPIO> pins) {
	for(jetsonGPIO pin : pins)
		gpioExport(pin);
}

void TactileFeedback::gpioBatchUnexport(vector<jetsonGPIO> pins) {
	for(jetsonGPIO pin : pins){
		gpioUnexport(pin);
		usleep(200000);
	}
}

std::thread TactileFeedback::_doFeedback(Direction dir,
		FeedbackType type) {
	return std::thread([=] {doFeedback(dir,type);});
}

std::thread TactileFeedback::_doCustomFeedback(Direction dir,unsigned int times, unsigned int duration) {
	return std::thread([=] {doCustomFeedback(dir,times,duration);});
}

std::thread TactileFeedback::_doCustomTypeFeedback(Direction dir,VibrationType vt) {
	return std::thread([=] {doCustomTypeFeedback(dir,vt);});
}



void TactileFeedback::gpioSendFeedback(vector<jetsonGPIO> pins, struct tactileSignalData signal) {
	for(unsigned int i=0; i<signal.repeatTimes;i++){
		if(i>0)
			usleep(signal.tactileDuration);
		gpioSetPinsValues(pins,on);
		usleep(signal.tactileDuration);
		gpioSetPinsValues(pins,off);
	}
}

void TactileFeedback::gpioSendCustomFeedback(vector<jetsonGPIO> pins, unsigned int times, unsigned int duration) {
	for(unsigned int i=0; i<times;i++){
		if(i>0)
			usleep(duration);
		gpioSetPinsValues(pins,on);
		usleep(duration);
		gpioSetPinsValues(pins,off);
	}
}

vector<jetsonGPIO> TactileFeedback::joinPins(vector<vector<jetsonGPIO>> pins) {
	vector<jetsonGPIO> npin;
	for(int i=0;i<pins.size();i++)
		npin.reserve(npin.size()+pins[i].size());

	for(int i=0;i<pins.size();i++){
		npin.insert(std::end(npin),std::begin(pins[i]),std::end(pins[i]));
	}
	std::cout << npin.size() << std::endl;
	return npin;
}
