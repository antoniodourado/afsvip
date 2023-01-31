/*
 * DepthCamera.h
 *
 *  Created on: 5 de out de 2019
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
