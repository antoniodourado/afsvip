/*
 * KinectFreenect.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "KinectFreenect.h"

KinectFreenect::~KinectFreenect() {
	// TODO Auto-generated destructor stub
}

KinectFreenect::KinectFreenect(freenect_context* _ctx, int _index): Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
		m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
		m_new_depth_frame(false), depthMat(Size(640,480),CV_16UC1),
		rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
		ownMat(Size(640,480),CV_8UC3,Scalar(0)){


	for( unsigned int i = 0 ; i < 2048 ; i++) {
		float v = i/2048.0;
		v = std::pow(v, 3)* 6;
		m_gamma[i] = v*6*256;
	}

}

bool KinectFreenect::getVideo(cv::Mat& output) {
	m_rgb_mutex.lock();
	if(m_new_rgb_frame) {
		cv::cvtColor(rgbMat, output, CV_RGB2BGR);
		m_new_rgb_frame = false;
		m_rgb_mutex.unlock();
		return true;
	} else {
		m_rgb_mutex.unlock();
		return false;
	}
}

bool KinectFreenect::getDist(cv::Mat& output) {
	m_depth_mutex.lock();
	if(m_new_depth_frame) {
		depthMat.copyTo(output);
		m_new_depth_frame = false;
		m_depth_mutex.unlock();
		return true;
	} else {
		m_depth_mutex.unlock();
		return false;
	}
}

bool KinectFreenect::getDepth(cv::Mat& output) {
	m_depth_mutex.lock();
	if(m_new_depth_frame) {
		//depthMat.copyTo(output);
		depthMat.convertTo(output, CV_8UC1, 255.0/2048);
		output.setTo(Scalar(0),output==255);
		m_new_depth_frame = false;
		m_depth_mutex.unlock();
		return true;
	} else {
		m_depth_mutex.unlock();
		return false;
	}
}


void KinectFreenect::VideoCallback(void* _rgb, uint32_t timestamp) {
	//std::cout << "RGB callback" << std::endl;
	m_rgb_mutex.lock();
	uint8_t* rgb = static_cast<uint8_t*>(_rgb);
	rgbMat.data = rgb;
	m_new_rgb_frame = true;
	m_rgb_mutex.unlock();
}

void KinectFreenect::DepthCallback(void* _depth, uint32_t timestamp) {
	//std::cout << "Depth callback" << std::endl;
	m_depth_mutex.lock();
	uint16_t* depth = static_cast<uint16_t*>(_depth);
	depthMat.data = (uchar*) depth;
	m_new_depth_frame = true;
	m_depth_mutex.unlock();
}

