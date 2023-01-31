/*
 * KinectFree.h
 *
 *  Created on: 5 de out de 2019
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
