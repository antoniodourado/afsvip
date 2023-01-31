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
