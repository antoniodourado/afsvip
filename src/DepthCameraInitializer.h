/*
 * DepthCameraInitializer.h
 *
 *  Created on: 5 de out de 2019
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
