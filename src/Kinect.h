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
#include "DepthCamera.h"
#include <opencv2/videoio/videoio.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#ifndef CLASSES_Kinect_H_
#define CLASSES_Kinect_H_

class Kinect : public DepthCamera {
public:
	Kinect();
	virtual ~Kinect();
	bool isKinectOK();
	cv::Mat getPCMFrame();
	cv::Mat getRGBFrame();
	cv::Mat getDepthFrame();
	cv::Mat getRGBFrame(cv::Mat);
	cv::Mat getDepthFrame(int);
	cv::Mat getDepthFrame(cv::Mat,float);
	void getDepthFrame(cv::Mat*,cv::Mat,float);
	cv::Mat getDistMap();
	cv::Mat getDistRGBFrame(cv::Mat,float);
	cv::Mat getNearestObject(cv::Mat,cv::Mat,int,int);
	int getImageWidth();
	int getImageHeight();
	bool isCameraOk();
	float getPixelDistance(int,int);
	float getPixelDistance(cv::Point2f);
	float getPixelDistance(cv::Point);
	bool isIMUAvailable();
	int getVerticalInclination();
	int getVerticalFOV();
	Cameras ctype = Cameras::KINECT;
private:
	VideoCapture kinect;
};

#endif /* SRC_KINECT_H_ */
