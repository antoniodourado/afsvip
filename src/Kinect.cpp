#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Kinect.h"
#include <libconfig.h++>

Kinect::Kinect() : DepthCamera(){
	try{
		cout << "Starting Kinect..." << endl;
		bool ini = false;
		kinect.open(CAP_OPENNI2);
		if(!kinect.isOpened())
			if( !kinect.isOpened() )
				kinect.open( CAP_OPENNI );

		if(!kinect.isOpened())
			cout << "There was a problem while starting Kinect..." << endl;
		else{
			//kinect.set(CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_VGA_30HZ);
			kinect.set(CV_CAP_PROP_OPENNI_REGISTRATION,0);

			cout << "Kinect successfully started" << endl;

		}


	}catch(Exception err){
		cout << "Erro: " << err.msg << endl;
	}
}

cv::Mat Kinect::getPCMFrame(){
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_POINT_CLOUD_MAP))
			return img;
	}
}

cv::Mat Kinect::getRGBFrame(){
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_BGR_IMAGE)){
			return img;
		}
	}
}

cv::Mat Kinect::getDepthFrame(){
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_DEPTH_MAP)){
			img.convertTo(img,CV_8UC1,0.05f);
			return img;
		}

	}
}

cv::Mat Kinect::getRGBFrame(cv::Mat dep){
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_BGR_IMAGE))
			return img.setTo(Scalar(255,255,255),dep==255);
	}
}

cv::Mat Kinect::getDepthFrame(int pix){
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_DEPTH_MAP)){
			img.convertTo(img,CV_8UC1,0.05f);
			//threshold(img,img,pix,pix,CV_THRESH_TOZERO_INV);
			img.setTo(Scalar(255),img>pix);
			return img;
		}

	}
}

cv::Mat Kinect::getDistMap(){
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_DEPTH_MAP))
			img.convertTo(img,CV_16UC1);
		return img;
	}
}

cv::Mat Kinect::getDepthFrame(cv::Mat mat, float dist) {
	if(kinect.grab()){
		cv::Mat img;
		if(kinect.retrieve(img,CAP_OPENNI_DEPTH_MAP)){
			img.convertTo(img,CV_8UC1,0.05f);
			//threshold(img,img,pix,pix,CV_THRESH_TOZERO_INV);
			img.setTo(Scalar(255),mat>dist); //threshold de distancia máxima
			//img.setTo(Scalar(255),mat==0); //eliminação de pixels pretos
			return img;
		}

	}
}

void Kinect::getDepthFrame(cv::Mat* depthMat,cv::Mat mat, float dist) {
	depthMat->setTo(Scalar(255),mat > dist);
}

cv::Mat Kinect::getNearestObject(cv::Mat map, cv::Mat img, int max, int tol=0){
	cv::Mat out = img;
	out.setTo(Scalar(255,255,255),map > (max+tol));
	//out.setTo(Scalar(255,255,255),map == 0);
	return out;
}

bool Kinect::isKinectOK(){
	return kinect.isOpened();
}

Kinect::~Kinect() {
	if(kinect.isOpened()){
		try{
			cout << "Closing Kinect...";
			kinect.release();
			cout << "Done!" << endl;
		}catch(Exception err){
			cout << "Problem while closing Kinect: " << err.msg << endl;
		}
	}
}

cv::Mat Kinect::getDistRGBFrame(cv::Mat RGB,float dist) {
	if(kinect.grab()){
		cv::Mat img,out;
		if(kinect.retrieve(img,CAP_OPENNI_DEPTH_MAP))
			img.convertTo(img,CV_16UC1);
		out = RGB.clone();
		return out.setTo(Scalar(255,255,255),img > dist);
	}
}

bool Kinect::isCameraOk() {
	return isKinectOK();
}

int Kinect::getImageWidth() {
	return (int)kinect.get(CAP_PROP_FRAME_WIDTH);
}

int Kinect::getImageHeight() {
	return (int)kinect.get(CAP_PROP_FRAME_HEIGHT);
}

float Kinect::getPixelDistance(int x, int y) {
	cv::Mat dst = getDistMap();
	return dst.at<float>(x,y);
}

float Kinect::getPixelDistance(cv::Point2f p) {
	return getPixelDistance(p.x,p.y);
}

float Kinect::getPixelDistance(cv::Point p) {
	return getPixelDistance(p.x,p.y);
}

bool Kinect::isIMUAvailable() {
	return false;
}

int Kinect::getVerticalInclination() {
	return 0;
}

int Kinect::getVerticalFOV() {
	return 0;
}
