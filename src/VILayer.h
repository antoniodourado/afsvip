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
 * VILayer.h
 *
 *  Created on: 25 de jul de 2016
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

class VILayer {
public:
	//VILayer();
	VILayer(FeedbackLayer*);
	VILayer(FeedbackLayer*,Configuration*);
	virtual ~VILayer();
	void startVoiceInteraction();
	pthread_t startVoiceInteractionp();
	std::thread startListening();
	//void *startListening();
private:
	pthread_t listenThreadp = pthread_self();
	int rc=0;
	long t=0;
	FeedbackLayer* feedback;
	PSContinuous* psc = NULL;
	std::thread listenThread;
};

#endif /* SRC_VILAYER_H_ */
