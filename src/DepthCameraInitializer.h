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
 * DepthCameraInitializer.h
 *
 *  Created on: 9 de set de 2019
 *      Author: antonio
 */
#include "Kinect.h"
#include "Zed.h"
#include "RealSense435i.h"
#include "CVLayer.h"
#include "__afsvipGLOBALS.h"

#ifndef SRC_DEPTHCAMERAINITIALIZER_H_
#define SRC_DEPTHCAMERAINITIALIZER_H_

class DepthCameraInitializer {
public:
	DepthCameraInitializer();
	virtual ~DepthCameraInitializer();
	DepthCamera* getDepthCamera();
private:
	DepthCamera* camera;
	string usbID[4] = { "NULL", "2b03:f582", "045e:02ae", "8086:0b3a" };
	Cameras checkCamera();
};

#endif /* SRC_DEPTHCAMERAINITIALIZER_H_ */
