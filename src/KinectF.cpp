/*
 * KinectF.cpp
 *
 *  Created on: 5 de out de 2019
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
