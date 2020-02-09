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
 * KinectFree.h
 *
 *  Created on: 7 de mai de 2017
 *      Author: antonio
 */

#ifndef KINECTFREE_H_
#define KINECTFREE_H_

#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "KinectFreenect.h"
#include <unistd.h>

using namespace cv;


class KinectF {
public:
	KinectF();
	virtual ~KinectF();
	bool isKinectOK();
	cv::Mat getRGBFrame();
	cv::Mat getDepthFrame();
	cv::Mat getDepthFrame(cv::Mat,float);
	cv::Mat getDistMap();
	cv::Mat getDistRGBFrame(cv::Mat,float);
	cv::Mat getNearestObject(cv::Mat,cv::Mat,int,int);
private:
	Freenect::Freenect freenect;
	KinectFreenect& kinect = freenect.createDevice<KinectFreenect>(0);

	cv::Mat distMat = cv::Mat::zeros(Size(640,480),CV_16UC1);
	cv::Mat depthMat = cv::Mat::zeros(Size(640,480),CV_8UC1);
	cv::Mat rgbMat = cv::Mat::zeros(Size(640,480),CV_8UC3);



};



#endif /* KINECTFREE_H_ */
