/*
 * RealSense435i.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "RealSense435i.h"

RealSense435i::RealSense435i() {
	holefiller.set_option(RS2_OPTION_HOLES_FILL,1);
	depthColor.set_option(RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED, 1.f);
	depthColor.set_option(RS2_OPTION_COLOR_SCHEME, 2.f);
	cfg.enable_stream(RS2_STREAM_COLOR, imgw, imgh, RS2_FORMAT_BGR8, 30);
	cfg.enable_stream(RS2_STREAM_DEPTH, imgw, imgh, RS2_FORMAT_Z16, 30);
	cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
	cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
	pipe.start(cfg);
	//pipe.start();

}

RealSense435i::~RealSense435i() {
	pipe.stop();
	cfg.disable_all_streams();
}

cv::Mat RealSense435i::getPCMFrame() {
	pc.map_to(color_frame);
	points = pc.calculate(depth_frame);
	cv::Mat pcm = frame_to_mat(points.get());
	return pcm;
}

cv::Mat RealSense435i::getRGBFrame() {
	frames = pipe.wait_for_frames();
	color_frame = frames.get_color_frame();
	//rgbFMat = frame_to_mat(color_frame);
	cv::Mat rgbFMat(cv::Size(imgw, imgh), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
	return rgbFMat.clone();
}

cv::Mat RealSense435i::getDepthFrame() {
	frames = pipe.wait_for_frames();
	depth_frame = frames.get_depth_frame();
	//depthFMat = frame_to_mat(depth_frame);
	rs2::frame filled = depth_frame;
	filled = holefiller.process(filled);
	//cv::Mat dMat(cv::Size(imgw, imgh), CV_8UC3, (void*)depthColor.colorize(depth_frame).get_data(), cv::Mat::AUTO_STEP);
	cv::Mat dMat(cv::Size(imgw, imgh), CV_8UC3, (void*)depthColor.colorize(filled).get_data(), cv::Mat::AUTO_STEP);
	cv::cvtColor(dMat,dMat,CV_BGRA2GRAY);
	//imshow("D",dMat);
	//cvWaitKey(300000);
	return dMat;
}

cv::Mat RealSense435i::getRGBFrame(cv::Mat dep) {
	cv::Mat rgb = rgbFMat.clone();
	rgb.setTo(cv::Scalar(255,255,255),dep==255);
	return rgb;
}

cv::Mat RealSense435i::getDepthFrame(int d) {
	cv::Mat df = depthFMat.clone();
	df.setTo(cv::Scalar(255),df < d);
	return df;
}

cv::Mat RealSense435i::getDepthFrame(cv::Mat mat, float dist) {
	cv::Mat df = getDepthFrame();
	df.setTo(cv::Scalar(1),mat>dist);
	return df;
}

void RealSense435i::getDepthFrame(cv::Mat* df,cv::Mat mat, float dist) {
	df->setTo(cv::Scalar(1),mat>dist);
}

cv::Mat RealSense435i::getDistMap() {
	frames = pipe.wait_for_frames();
	depth_frame = frames.get_depth_frame();
	rs2::frame filled = depth_frame;
	filled = holefiller.process(filled);
	distFMat = depth_frame_to_meters(pipe,filled);
	distFMat *= 1000;
	return distFMat.clone();
}

cv::Mat RealSense435i::getDistRGBFrame(cv::Mat rgb, float dist) {
	rgb.setTo(cv::Scalar(1,1,1),distFMat>dist);
	return rgb;
}

cv::Mat RealSense435i::getNearestObject(cv::Mat map, cv::Mat img, int max,
		int tol=0) {
	cv::Mat out = img.clone();
	out.setTo(cv::Scalar(1,1,1),map > (max+tol));
	return out;
}

int RealSense435i::getImageWidth() {
	return imgw;
}

int RealSense435i::getImageHeight() {
	return imgh;
}

bool RealSense435i::isCameraOk() {
	return true;
}

// Convert rs2::frame to cv::Mat
cv::Mat RealSense435i::frame_to_mat(const rs2::frame& f)
{
	using namespace cv;
	using namespace rs2;

	auto vf = f.as<video_frame>();
	const int w = vf.get_width();
	const int h = vf.get_height();

	if (f.get_profile().format() == RS2_FORMAT_BGR8)
	{
		return Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
	}
	else if (f.get_profile().format() == RS2_FORMAT_RGB8)
	{
		auto r = Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
		cvtColor(r, r, COLOR_RGB2BGR);
		return r;
	}
	else if (f.get_profile().format() == RS2_FORMAT_Z16)
	{
		return Mat(Size(w, h), CV_16UC1, (void*)f.get_data(), Mat::AUTO_STEP);
	}
	else if (f.get_profile().format() == RS2_FORMAT_Y8)
	{
		return Mat(Size(w, h), CV_8UC1, (void*)f.get_data(), Mat::AUTO_STEP);
	}
	else if (f.get_profile().format() == RS2_FORMAT_DISPARITY32)
	{
		return Mat(Size(w, h), CV_32FC1, (void*)f.get_data(), Mat::AUTO_STEP);
	}

	throw std::runtime_error("Frame format is not supported yet!");
}

// Converts depth frame to a matrix of doubles with distances in meters
cv::Mat RealSense435i::depth_frame_to_meters(const rs2::pipeline& pipe, const rs2::depth_frame& f)
{
	using namespace cv;
	using namespace rs2;
	Mat dm = frame_to_mat(f);
	dm.convertTo(dm, CV_64F);
	auto depth_scale = pipe.get_active_profile()
        				.get_device()
						.first<depth_sensor>()
						.get_depth_scale();
	dm = dm * depth_scale;
	dm.convertTo(dm, CV_32FC1);
	patchNaNs(dm,0);
	return dm;
}

float RealSense435i::getPixelDistance(int i, int j) {
	rs2::depth_frame depth = frames.get_depth_frame();
	return depth.get_distance(i,j);
}

float RealSense435i::getPixelDistance(cv::Point2f p) {
	return getPixelDistance(p.x,p.y);
}

bool RealSense435i::isIMUAvailable() {
	bool found_gyro = false;
	bool found_accel = false;
	rs2::context ctx;
	for (auto dev : ctx.query_devices())
	{
		// The same device should support gyro and accel
		found_gyro = false;
		found_accel = false;
		for (auto sensor : dev.query_sensors())
		{
			for (auto profile : sensor.get_stream_profiles())
			{
				if (profile.stream_type() == RS2_STREAM_GYRO)
					found_gyro = true;

				if (profile.stream_type() == RS2_STREAM_ACCEL)
					found_accel = true;
			}
		}
		if (found_gyro && found_accel)
			break;
	}
	return found_gyro && found_accel;
}

int RealSense435i::getVerticalInclination() {
	frames = pipe.wait_for_frames();
	if(rs2::motion_frame gyro_frame = frames.first_or_default(RS2_STREAM_GYRO))// && gyro_frame.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
		process_gyro(gyro_frame.get_motion_data(),gyro_frame.get_timestamp());
	if(rs2::motion_frame accel_frame = frames.first_or_default(RS2_STREAM_ACCEL))// && accel_frame.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
		process_accel(accel_frame.get_motion_data());
	float03 ftheta=get_theta();
	return trunc(-1*(ftheta.z*180/PI));
}

float RealSense435i::getPixelDistance(cv::Point p) {
	return getPixelDistance(p.x,p.y);
}

void RealSense435i::process_gyro(rs2_vector gyro_data, double ts)
{
	if (first) // On the first iteration, use only data from accelerometer to set the camera's initial position
	{
		last_ts_gyro = ts;
		return;
	}
	// Holds the change in angle, as calculated from gyro
	float03 gyro_angle;

	// Initialize gyro_angle with data from gyro
	gyro_angle.x = gyro_data.x; // Pitch
	gyro_angle.y = gyro_data.y; // Yaw
	gyro_angle.z = gyro_data.z; // Roll

	// Compute the difference between arrival times of previous and current gyro frames
	double dt_gyro = (ts - last_ts_gyro) / 1000.0;
	last_ts_gyro = ts;

	// Change in angle equals gyro measures * time passed since last measurement
	gyro_angle = gyro_angle * dt_gyro;

	// Apply the calculated change of angle to the current angle (theta)
	std::lock_guard<std::mutex> lock(theta_mtx);
	theta.add(-gyro_angle.z, -gyro_angle.y, gyro_angle.x);
}

void RealSense435i::process_accel(rs2_vector accel_data)
{
	// Holds the angle as calculated from accelerometer data
	float03 accel_angle;

	// Calculate rotation angle from accelerometer data
	accel_angle.z = atan2(accel_data.y, accel_data.z);
	accel_angle.x = atan2(accel_data.x, sqrt(accel_data.y * accel_data.y + accel_data.z * accel_data.z));

	// If it is the first iteration, set initial pose of camera according to accelerometer data (note the different handling for Y axis)
	std::lock_guard<std::mutex> lock(theta_mtx);
	if (first)
	{
		//first = false;
		theta = accel_angle;
		// Since we can't infer the angle around Y axis using accelerometer data, we'll use PI as a convetion for the initial pose
		theta.y = PI;
	}
	else
	{
		/*
            Apply Complementary Filter:
                - high-pass filter = theta * alpha:  allows short-duration signals to pass through while filtering out signals
                  that are steady over time, is used to cancel out drift.
                - low-pass filter = accel * (1- alpha): lets through long term changes, filtering out short term fluctuations
		 */
		theta.x = theta.x * alpha + accel_angle.x * (1 - alpha);
		theta.z = theta.z * alpha + accel_angle.z * (1 - alpha);
	}
}

// Returns the current rotation angle
float03 RealSense435i::get_theta()
{
	std::lock_guard<std::mutex> lock(theta_mtx);
	return theta;
}

int RealSense435i::getVerticalFOV() {
	return 58;
}
