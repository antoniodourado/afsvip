/*
 * VILayer.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "VILayer.h"

/*VILayer::VILayer() {
	psc = new PSContinuous();
}*/

pthread_t VILayer::startVoiceInteractionp(){
	//listenThread = psc->listenThread();
	//listenThread.join();
	rc = pthread_create(&listenThreadp,NULL,&PSContinuous::listenT,(void *)t);
	if(rc)
		cout << "Erro" << endl;
	return listenThreadp;
}

void VILayer::startVoiceInteraction(){
	listenThread = psc->listenThread();
	listenThread.join();
	//psc->listen();
}

VILayer::VILayer(FeedbackLayer* f) {
	feedback = f;
	psc = new PSContinuous(f);
}

VILayer::VILayer(FeedbackLayer* f, Configuration* c) {
	feedback = f;
	psc = new PSContinuous(f,c);
}

std::thread VILayer::startListening(){
	return std::thread([=] {startVoiceInteraction();});
}

VILayer::~VILayer() {
	delete psc;
}

