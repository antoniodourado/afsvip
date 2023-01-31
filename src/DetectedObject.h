/*
 * DetectedObject.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC_DETECTEDOBJECT_H_
#define SRC_DETECTEDOBJECT_H_
#include <opencv2/core/core.hpp>

using namespace cv;

class DetectedObject {
public:
	DetectedObject();
	virtual ~DetectedObject();
	void processRGBRegion();

public:
	long id;
	cv::Mat rgbImage;
	cv::Mat rgbRegion;
	Rect bounds;
	float centroidDistance;
	Point2f centroid;
	time_t detectionTime;
};

#endif /* SRC_DETECTEDOBJECT_H_ */
