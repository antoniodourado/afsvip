/*
 * RealSense435i.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */
#ifndef SRC_REALSENSE435I_H_
#define SRC_REALSENSE435I_H_
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.
#include "DepthCamera.h"
#include <librealsense2/rs.hpp>

#ifndef PI
const double PI = 3.14159265358979323846;
#endif

struct float03 {
	float x, y, z;
	float03 operator*(float t)
	{
		return { x * t, y * t, z * t };
	}

	float03 operator-(float t)
	{
		return { x - t, y - t, z - t };
	}

	void operator*=(float t)
	{
		x = x * t;
		y = y * t;
		z = z * t;
	}

	void operator=(float03 other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	void add(float t1, float t2, float t3)
	{
		x += t1;
		y += t2;
		z += t3;
	}
};

class RealSense435i : public DepthCamera{
public:
	RealSense435i();
	virtual ~RealSense435i();
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
	Cameras ctype = Cameras::D435I;

private:
	int imgw=640;
	int imgh=480;
	rs2::pipeline pipe;
	rs2::config cfg;
	rs2::frameset frames;
	rs2::frame depth_frame,color_frame;
	cv::Mat depthFMat,rgbFMat,distFMat;
	rs2::pointcloud pc;
	rs2::points points;
	rs2::colorizer depthColor;
	float03 theta;
	std::mutex theta_mtx;
	/* alpha indicates the part that gyro and accelerometer take in computation of theta; higher alpha gives more weight to gyro, but too high
	    values cause drift; lower alpha gives more weight to accelerometer, which is more sensitive to disturbances */
	float alpha = 0.98;
	bool first = true;
	// Keeps the arrival time of previous gyro frame
	double last_ts_gyro = 0;
	rs2::hole_filling_filter holefiller;

private:
	cv::Mat frame_to_mat(const rs2::frame&);
	cv::Mat depth_frame_to_meters(const rs2::pipeline&, const rs2::depth_frame&);
	float getPixelDistance(int,int);
	float getPixelDistance(cv::Point2f);
	float getPixelDistance(cv::Point);
	void process_gyro(rs2_vector, double);
	void process_accel(rs2_vector);
	float03 get_theta();

};
#endif /* SRC_REALSENSE435I_H_ */
