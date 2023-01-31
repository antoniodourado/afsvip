/*
 * KinectFreenect.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef KINECTFREENECT_H_
#define KINECTFREENECT_H_

#include <libfreenect/libfreenect.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class myMutex {
public:
	myMutex() {
		pthread_mutex_init( &m_mutex, NULL );
	}
	void lock() {
		pthread_mutex_lock( &m_mutex );
	}
	void unlock() {
		pthread_mutex_unlock( &m_mutex );
	}
private:
	pthread_mutex_t m_mutex;
};



class KinectFreenect  : public Freenect::FreenectDevice {
public:
	KinectFreenect(freenect_context *_ctx, int _index);
	virtual ~KinectFreenect();
	bool getVideo(cv::Mat& output);
	bool getDist(cv::Mat& output);
	bool getDepth(cv::Mat& output);

private:
	void VideoCallback(void* _rgb, uint32_t timestamp);
	void DepthCallback(void* _depth, uint32_t timestamp);

private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	cv::Mat depthMat;
	cv::Mat rgbMat;
	cv::Mat ownMat;
	myMutex m_rgb_mutex;
	myMutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
};

#endif /* KINECTFREENECT_H_ */
