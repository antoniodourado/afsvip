/*
 * DetectedObject.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "DetectedObject.h"

DetectedObject::DetectedObject() {
	// TODO Auto-generated constructor stub

}

DetectedObject::~DetectedObject() {
	// TODO Auto-generated destructor stub
}

void DetectedObject::processRGBRegion() {
	rgbRegion = rgbImage(bounds);
}
