/*
 * Zed.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "Zed.h"

Zed::Zed() : DepthCamera(){
	init_params.camera_resolution = sl::RESOLUTION_HD720;
	init_params.depth_mode = sl::DEPTH_MODE_QUALITY;
	init_params.coordinate_units = sl::UNIT_MILLIMETER;
	sl::ERROR_CODE err = zed.open(init_params);
	if (err != sl::SUCCESS) {
		BOOST_LOG_TRIVIAL(error) << errorCode2str(err).c_str();
		zed.close();
		return; // Quit if an error occurred
	}
	runtime_parameters.sensing_mode = sl::SENSING_MODE_STANDARD;
	image_size = zed.getResolution();
	zed.setDepthMaxRangeValue(9999999);
	//image_size.width = image_size.w;
	//image_size.height = 480;
}

Zed::~Zed() {
	zed.close();
}

cv::Mat Zed::getPCMFrame() {
	cv::Mat pcm;
	return pcm;
}

cv::Mat Zed::getRGBFrame() {
	sl::Mat image_zed(image_size.width, image_size.height, sl::MAT_TYPE_8U_C4);
	cv::Mat image_ocv = slMat2CvMat(image_zed);

	while(zed.grab(runtime_parameters) != sl::SUCCESS) {
		continue;
	}
	zed.retrieveImage(image_zed, sl::VIEW_RIGHT, sl::MEM_CPU, image_size.width, image_size.height);
	return image_ocv.clone();
}

cv::Mat Zed::getDepthFrame() {
	sl::Mat depth_image_zed(image_size.width, image_size.height, sl::MAT_TYPE_8U_C4);
	cv::Mat depth_image_ocv = slMat2CvMat(depth_image_zed);

	while(zed.grab(runtime_parameters) != sl::SUCCESS) {
		continue;
	}
	zed.retrieveImage(depth_image_zed, sl::VIEW_DEPTH, sl::MEM_CPU, image_size.width, image_size.height);
	cv::cvtColor(depth_image_ocv,depth_image_ocv,CV_BGRA2GRAY);
	return depth_image_ocv.clone();
}

cv::Mat Zed::getRGBFrame(cv::Mat dep){
	cv::Mat img=getRGBFrame();
	return img.setTo(cv::Scalar(255,255,255),dep==255);
}

cv::Mat Zed::getDepthFrame(int pix){
	cv::Mat img=getDepthFrame();
	img.setTo(cv::Scalar(255),img<pix);
	return img;
}

cv::Mat Zed::getDistMap(){
	sl::Mat image;
	sl::Mat depth_map;
	if (zed.grab() == sl::SUCCESS) {
	  // A new image and depth is available if grab() returns SUCCESS
	  zed.retrieveImage(image, sl::VIEW_RIGHT); // Retrieve left image
	  zed.retrieveMeasure(depth_map, sl::MEASURE_DEPTH); // Retrieve depth
	}
	cv::Mat dist = slMat2CvMat(depth_map);
	cv::patchNaNs(dist,0);
	dist.setTo(cv::Scalar(0),dist<0);
	return dist.clone();
}

cv::Mat Zed::getDepthFrame(cv::Mat mat, float dist) {
	cv::Mat img=getDepthFrame();
	img.setTo(cv::Scalar(1),mat>dist); //threshold de distancia máxima
	return img;
}

void Zed::getDepthFrame(cv::Mat* img, cv::Mat mat, float dist) {
	img->setTo(cv::Scalar(1),mat>dist); //threshold de distancia máxima
}

cv::Mat Zed::getNearestObject(cv::Mat map, cv::Mat img, int max, int tol=0){
	cv::Mat out = img;
	out.setTo(cv::Scalar(1,1,1),map > (max+tol));
	return out;
}

cv::Mat Zed::getDistRGBFrame(cv::Mat RGB,float dist) {
	cv::Mat img,out;
	img=getDistMap();
	out = RGB.clone();
	out.setTo(cv::Scalar(1,1,1),img > dist);
	return out.clone();
}

bool Zed::isCameraOk() {
	return zed.isOpened();
}

cv::Mat Zed::slMat2CvMat(sl::Mat& input) {
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
		case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
		case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
		case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
		case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
		case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
		case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
		case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
		case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
		default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::cv::Mat (getPtr<T>())
	// cv::Mat and sl::cv::Mat will share a single memory structure
	//return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM_CPU));
	return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM_CPU));

}

int Zed::getImageWidth() {
	return (int)zed.getResolution().width;
}

int Zed::getImageHeight() {
	return (int)zed.getResolution().height;
}

float Zed::getPixelDistance(int x, int y) {
	cv::Mat dst = getDistMap();
	return dst.at<float>(x,y);
}

float Zed::getPixelDistance(cv::Point2f p) {
	return getPixelDistance(p.x,p.y);
}

float Zed::getPixelDistance(cv::Point p) {
	return getPixelDistance(p.x,p.y);
}

bool Zed::isIMUAvailable() {
	return true;
}

int Zed::getVerticalInclination() {
	return 0;
}

int Zed::getVerticalFOV() {
	return 0;
}
