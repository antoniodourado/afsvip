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
 * VILayer.cpp
 *
 *  Created on: 25 de jul de 2016
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

