/*
 * DepthCameraInitializer.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "DepthCameraInitializer.h"

DepthCameraInitializer::DepthCameraInitializer() {
	switch(checkCamera()){
		case Cameras::ZED:
		{
			camera = new Zed();
		}
		break;
		case Cameras::D435I:
		{
			camera = new RealSense435i();
		}
		break;
		case Cameras::KINECT:
		{
			camera = new Kinect();
		}
		break;
		default:
			camera=NULL;
			break;
		}
}

DepthCameraInitializer::~DepthCameraInitializer() {
	//cv::destroyAllWindows();
	if(camera==NULL)
		return;
	RealSense435i* r = dynamic_cast<RealSense435i*>(camera);
	Zed* z = dynamic_cast<Zed*>(camera);
	Kinect* k = dynamic_cast<Kinect*>(camera);
	if(z!=NULL){
		z->~Zed();
		delete z;
	}
	else if(r!=NULL){
		//r->~RealSense435i();
		delete r;
	}
	else if(k!=NULL){
		k->~Kinect();
		delete k;
	}
	camera=NULL;
}

DepthCamera* DepthCameraInitializer::getDepthCamera() {
	return camera;
}

Cameras DepthCameraInitializer::checkCamera() {
	libusb_device **devs;
	libusb_context *ctx = NULL;
	int r;
	ssize_t cnt;
	r = libusb_init(&ctx);
	if(r < 0) {
		BOOST_LOG_TRIVIAL(error) << "Init Error "<<r;
		return Cameras::NONE;
	}
	libusb_set_debug(ctx, 3);
	cnt = libusb_get_device_list(ctx, &devs);
	if(cnt < 0) {
		BOOST_LOG_TRIVIAL(error) << "Get Device Error";
	}
	ssize_t i;
	std::string resV;
	std::string resP;
	for(i = 0; i < cnt; i++) {
		libusb_device_descriptor desc;  //print specs of this device
		int r = libusb_get_device_descriptor(devs[i],&desc);
		if(r < 0){
			BOOST_LOG_TRIVIAL(error) << "Failed to get usb dev. desc.";
			continue;
		}
		std::stringstream ssV;
		std::stringstream ssP;
		ssV << std::hex << std::setw(4) << std::setfill('0') << desc.idVendor;
		ssP << std::hex << std::setw(4) << std::setfill('0') << desc.idProduct;
		ssV >> resV;
		ssP >> resP;
		resV.append(":");
		resV.append(resP);
		if(resV.compare(usbID[Cameras::ZED])==0){
			libusb_free_device_list(devs, 1); //free the list, unref the devices in it
			libusb_exit(ctx); //close the session
			return Cameras::ZED;
		}
		else if(resV.compare(usbID[Cameras::D435I])==0){
			libusb_free_device_list(devs, 1); //free the list, unref the devices in it
			libusb_exit(ctx); //close the session
			return Cameras::D435I;
		}
		else if(resV.compare(usbID[Cameras::KINECT])==0){
			libusb_free_device_list(devs, 1); //free the list, unref the devices in it
			libusb_exit(ctx); //close the session
			return Cameras::KINECT;
		}
	}
	libusb_free_device_list(devs, 1); //free the list, unref the devices in it
	libusb_exit(ctx); //close the session
	return Cameras::NONE;
}

