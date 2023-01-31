/*
 * CVLayer.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_CVLAYER_H_
#define SRC_CVLAYER_H_

#include "Kinect.h"
#include "Zed.h"
#include "RealSense435i.h"
#include "CodeUtil.h"
#include "DetectedObject.h"
#include "CaffeClassifier.h"
#include "FeedbackLayer.h"
#include <unistd.h>
#include <time.h>
#include <thread>
#include <chrono>
#include <string>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <libusb.h>
#include "__afsvipGLOBALS.h"
#include "Configuration.h"


using namespace std::chrono;
using namespace cv;

/*#define MIN_THRESH 50 //KINECT
#define MIN_THRESH 205 //ZED

#define IMGWIDTH 640
#define IMGHEIGHT 480*/

#define COUNTOUR_MIN_AREA 50



class CVLayer {



public:
	CVLayer();
	CVLayer(FeedbackLayer*);
	CVLayer(FeedbackLayer*,DepthCamera*,Configuration*);
	virtual ~CVLayer();
	void processVideo();
	double imgCorrelation(cv::Mat curr, cv::Mat prev);
	std::thread startVideoProcess();
	DepthCamera* getCamera();

public:
	string usbID[4] = { "NULL", "2b03:f582", "045e:02ae", "8086:0b3a" };

	enum DisplayWindow
	{
		RGB,
		DISTRGB,
		DEPTH,
		DISTDEPTH,
		FREEPATH
	};
	int DWINDOW=0;
	int MIN_THRESH; //ZED
	int IMGWIDTH;
	int IMGHEIGHT;
private:
	CodeUtil::CorrelationQueue corrqueue;
	DepthCamera* camera;
	cv::Mat currRGB,corrRGB;
	cv::Mat currDepth,corrDepth;
	cv::Mat prevRGB,pcorrRGB;
	cv::Mat prevDepth,pcorrDepth;
	cv::Mat corrScene,pcorrScene;
	cv::Mat distRGB, threshOut, nearest, distFrame,absDepth,absRGB,absDistFrame;
	double prevCorr;
	Configuration* config;
	DetectedObject* detectedObject;
	FeedbackLayer* feedback;
	double minp, mind, maxp, maxd;
	cv::Point minp_loc, mind_loc, maxp_loc,maxd_loc;
private:

	pair<int,int> safePathNav;
	list<pair<int,int>> pathNavList;
	//list<pair<string,cv::Mat>> *snapshots = NULL;
	bool continuousFeedback=true;
	int feedbackSpeed;
	time_t lastMultiPathWarn = time(NULL);
	list<pair<int,int>> multipaths;
	bool multiPathDetected = false;
	const int imgslices = 10;
	//const int pathnavobjthresh = 70; //kinect
	int pathnavobjthresh = 185; //zed
	bool showImg=false;
	bool kinPattern=false;
	bool isRGBScene=false;
	float larg = 0.08;
	cv::Mat rgbMarked,depthMarked;
	pair<int,int>* avgSliced = NULL;
private:
	DetectedObject* getClosestObject(cv::Mat img, cv::Mat distFrame,vector<vector<Point> > contours);
	bool frameChanged();
	bool isCentroidValid(Point2f,cv::MatSize);
	void fixBoundingRect(DetectedObject*,int);
	list<pair<int,int>> getSafestPath(cv::Mat,cv::Mat,cv::Mat*);
	void fixDepthImageDistance(cv::Mat*);
	Cameras checkCamera();
	int dist2pix(cv::Mat,cv::Mat,float);
};

#endif /* SRC_CVLAYER_H_ */
