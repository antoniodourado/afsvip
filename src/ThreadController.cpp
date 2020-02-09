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
 * ThreadController.cpp
 *
 *  Created on: 26 de abr de 2019
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
