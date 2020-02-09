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
 * KinectF.cpp
 *
 *  Created on: 7 de mai de 2017
 *      Author: antonio
 */

#include "KinectF.h"

KinectF::KinectF() {
	cout << "Starting Kinect with Fnect Drivers..." << endl;
	kinect.startVideo();
	kinect.startDepth();
	cout << "Done!" << endl;
}

KinectF::~KinectF() {
	cout << "Stopping Kinect..." << endl;
	kinect.stopVideo();
	kinect.stopDepth();
	cout << "Done!" << endl;
}

bool KinectF::isKinectOK() {
	return freenect.deviceCount() > 0;
}

cv::Mat KinectF::getRGBFrame() {
	while(!kinect.getVideo(rgbMat))
		continue;
	rgbMat.convertTo(rgbMat,CV_8UC3);
	return rgbMat;
}

cv::Mat KinectF::getDepthFrame() {
	while(!kinect.getDepth(depthMat))
		continue;
	//depthcv::Mat.convertTo(depthcv::Mat,CV_8UC1,0.05f);
	return depthMat;
}

cv::Mat KinectF::getDepthFrame(cv::Mat mat, float dist) {
	while(!kinect.getDepth(depthMat))
		continue;
	depthMat.setTo(Scalar(255),mat > dist);
	return depthMat;
}

cv::Mat KinectF::getDistMap() {
	kinect.getDist(distMat);
	return distMat;
}

cv::Mat KinectF::getDistRGBFrame(cv::Mat mat, float dist) {
	while(!kinect.getVideo(rgbMat))
		continue;
	rgbMat.setTo(Scalar(255),mat>dist);
	return rgbMat;
}

cv::Mat KinectF::getNearestObject(cv::Mat map, cv::Mat img, int max, int tol=0) {
	cv::Mat out = img;
	out.setTo(Scalar(255,255,255),map > (max+tol));
	//out.setTo(Scalar(255,255,255),map == 0);
	return out;
}
