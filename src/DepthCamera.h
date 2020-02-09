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
 * DepthCamera.h
 *
 *  Created on: 4 de dez de 2017
 *      Author: antonio
 */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "__afsvipGLOBALS.h"

#ifndef DEPTHCAMERA_H_
#define DEPTHCAMERA_H_

class DepthCamera {
public:
	DepthCamera(){};
	virtual ~DepthCamera(){};
	virtual cv::Mat getPCMFrame() = 0;
	virtual cv::Mat getRGBFrame() = 0;
	virtual cv::Mat getDepthFrame() = 0;
	virtual cv::Mat getRGBFrame(cv::Mat) = 0;
	virtual cv::Mat getDepthFrame(int) = 0;
	virtual cv::Mat getDepthFrame(cv::Mat,float) = 0;
	virtual void getDepthFrame(cv::Mat*,cv::Mat,float) = 0;
	virtual cv::Mat getDistMap() = 0;
	virtual cv::Mat getDistRGBFrame(cv::Mat,float) = 0;
	virtual cv::Mat getNearestObject(cv::Mat,cv::Mat,int,int) = 0;
	virtual float getPixelDistance(int,int) = 0;
	virtual float getPixelDistance(cv::Point2f) = 0;
	virtual float getPixelDistance(cv::Point) = 0;
	virtual bool isIMUAvailable() = 0;
	virtual int getVerticalInclination() = 0;
	virtual int getImageWidth() = 0;
	virtual int getImageHeight() = 0;
	virtual bool isCameraOk() = 0;
	virtual int getVerticalFOV() = 0;
};

#endif /* DEPTHCAMERA_H_ */
