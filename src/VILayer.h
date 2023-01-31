/*
 * VILayer.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_VILAYER_H_
#define SRC_VILAYER_H_
#include "PSContinuous.h"
#include "FeedbackLayer.h"
#include <thread>
#include <pthread.h>
#include <iostream>
#include "Configuration.h"

//**********************************
//  INTERACTOR 2
//**********************************

class VILayer
{
public:
	// VILayer();
	VILayer(FeedbackLayer *);
	VILayer(FeedbackLayer *, Configuration *);
	virtual ~VILayer();
	void startVoiceInteraction();
	pthread_t startVoiceInteractionp();
	std::thread startListening();
	// void *startListening();
private:
	pthread_t listenThreadp = pthread_self();
	int rc = 0;
	long t = 0;
	FeedbackLayer *feedback;
	PSContinuous *psc = NULL;
	std::thread listenThread;
};

#endif /* SRC_VILAYER_H_ */
