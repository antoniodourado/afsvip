/*
 * Zed.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef ZED_H_
#define ZED_H_

#include <sl/Camera.hpp>
#include "DepthCamera.h"


class Zed : public DepthCamera {
public:
	Zed();
	virtual ~Zed();
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
	bool isIMUAvailable();
	int getVerticalInclination();
	int getVerticalFOV();
	Cameras ctype = Cameras::ZED;
private:
	sl::Camera zed;
	sl::InitParameters init_params;
	sl::RuntimeParameters runtime_parameters;
	sl::Resolution image_size;
	float getPixelDistance(int,int);
	cv::Mat slMat2CvMat(sl::Mat&);
	float getPixelDistance(cv::Point2f);
	float getPixelDistance(cv::Point);
};

#endif /* ZED_H_ */
