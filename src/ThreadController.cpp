/*
 * ThreadController.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "ThreadController.h"

ThreadController::ThreadController(VILayer* v, PILayer* p) {
	vi=v;
	pi=p;
}

std::thread ThreadController::startThreads() {
	return std::thread([=] {createAndRun();});
}

ThreadController::~ThreadController() {
	// TODO Auto-generated destructor stub
}

void ThreadController::createAndRun() {
	std::thread voiceListener;
	std::thread keyListener;
	bool vUserPause = false;
	bool kUserPause = false;
	std::future<void> vF;
	std::future<void> kF;
	vF = std::async(std::launch::async,&VILayer::startVoiceInteraction,vi);
	kF = std::async(std::launch::async,&PILayer::startKeyboardListener,pi);

	while(!gvars.haltSystem){
		if(!gvars.stopVoiceListener && vUserPause){
			vF = std::async(std::launch::async,&VILayer::startVoiceInteraction,vi);
			vUserPause=false;
		}
		else if(gvars.stopVoiceListener && !vUserPause){
			//vF.get();
			vUserPause = true;
		}

		if(!gvars.stopKeyListener && kUserPause){
			kF = std::async(std::launch::async,&PILayer::startKeyboardListener,pi);
			kF.get();
			kUserPause=false;
		}
		else if(gvars.stopKeyListener && !kUserPause){
			kF.wait();
			kUserPause = true;
		}
	}
}
