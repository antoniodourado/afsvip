/*
 * CVLayer.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "CVLayer.h"

CVLayer::CVLayer()
{
	switch (checkCamera())
	{
	case Cameras::ZED:
		camera = new Zed();
		kinPattern = false;
		break;
	case Cameras::KINECT:
		camera = new Kinect();
		kinPattern = true;
		break;
	default:
		feedback->callVoiceFeedbackText("Nenhuma câmera.");
		gvars.haltSystem = true;
		break;
	}
	larg *= IMGWIDTH;
}

CVLayer::CVLayer(FeedbackLayer *f)
{
	feedback = f;
	switch (checkCamera())
	{
	case Cameras::ZED:
	{
		camera = new Zed();
		kinPattern = false;
		MIN_THRESH = 205;
		IMGWIDTH = camera->getImageWidth();
		IMGHEIGHT = camera->getImageHeight();
		pathnavobjthresh = 185;
	}
	break;
	case Cameras::D435I:
	{
		camera = new RealSense435i();
		kinPattern = false;
		MIN_THRESH = 205;
		IMGWIDTH = camera->getImageWidth();
		IMGHEIGHT = camera->getImageHeight();
		pathnavobjthresh = 185;
	}
	break;
	case Cameras::KINECT:
	{
		camera = new Kinect();
		kinPattern = true;
		MIN_THRESH = 50;
		IMGWIDTH = camera->getImageWidth();
		IMGHEIGHT = camera->getImageHeight();
		pathnavobjthresh = 70;
	}
	break;
	default:
		feedback->callVoiceFeedbackText("Nenhuma câmera.");
		exit(0);
		break;
	}
	larg = IMGWIDTH * 0.08;
}

CVLayer::CVLayer(FeedbackLayer *f, DepthCamera *cam, Configuration *c)
{ // @suppress("Class members should be properly initialized")
	feedback = f;
	camera = cam;
	config = c;
	if (dynamic_cast<Zed *>(camera))
	{
		kinPattern = false;
		MIN_THRESH = 205;
		IMGWIDTH = camera->getImageWidth();
		IMGHEIGHT = camera->getImageHeight();
		pathnavobjthresh = 185;
	}
	else if (dynamic_cast<RealSense435i *>(camera))
	{
		kinPattern = false;
		MIN_THRESH = 205;
		IMGWIDTH = camera->getImageWidth();
		IMGHEIGHT = camera->getImageHeight();
		pathnavobjthresh = 185;
	}
	else if (dynamic_cast<Kinect *>(camera))
	{
		kinPattern = true;
		MIN_THRESH = 50;
		IMGWIDTH = camera->getImageWidth();
		IMGHEIGHT = camera->getImageHeight();
		pathnavobjthresh = 70;
	}
	else
	{
		feedback->callVoiceFeedbackText("Nenhuma câmera.");
		exit(0);
	}
	larg = IMGWIDTH * 0.08;
}

void CVLayer::processVideo()
{
	int thresh = MIN_THRESH;
	bool updateScene = false;
	bool updateThresh = false;
	float maxdistance = config->getMaximumDistance();
	float maxdistmm = maxdistance * 1000;
	float mindistance = config->getMinimumDistance();
	float mindistmm = mindistance * 1000;
	float newmaxDist, newminDist;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	bool pathnav = false;
	cv::Mat cannyOut;
	cv::Mat freepathimg;
	while (!gvars.haltSystem)
	{
		if (camera->isCameraOk())
		{
			if (gvars.stopSystemFeedback)
			{
				destroyAllWindows();
				continue;
			}
			BOOST_LOG_TRIVIAL(info) << "###################### Inicio do processamento do frame ############################";
			// config->reloadConfig();
			tic();

			// Feedback continuo
			continuousFeedback = config->isContinuousFeedback();

			// Navegação por caminho
			pathnav = config->isPathNavigation();

			newmaxDist = config->getMaximumDistance();
			newminDist = config->getNavigationDistance();
			if (newminDist != mindistance)
			{
				mindistance = newminDist;
				mindistmm = mindistance * 1000;
				BOOST_LOG_TRIVIAL(info) << "Detectando a partir de " << mindistance << "m";
			}
			if (newmaxDist != maxdistance)
			{
				maxdistance = newmaxDist;
				maxdistmm = maxdistance * 1000;
				BOOST_LOG_TRIVIAL(info) << "Detectando até " << maxdistance << "m";
			}

			/*if(camera->isIMUAvailable())
				BOOST_LOG_TRIVIAL(trace) << "Camera Y angle: " << camera->getVerticalInclination() << "°";*/

			//**********************************************
			//
			// Obtenção de novos frames || DATA ACQUISITION
			//
			//**********************************************
			if (!corrScene.empty())
				pcorrScene = corrScene;
			tic();
			distFrame = camera->getDistMap();
			absDistFrame = distFrame.clone();
			absDepth = camera->getDepthFrame();
			// currDepth = camera->getDepthFrame(distFrame,maxdistmm);
			// currDepth = absDepth.clone();
			// camera->getDepthFrame(&currDepth,distFrame,maxdistmm);
			currRGB = camera->getRGBFrame();
			absRGB = currRGB.clone();
			distRGB = currRGB.clone();

			if (kinPattern)
				distRGB.setTo(cv::Scalar(255, 255, 255), distFrame > maxdistmm);
			else
			{
				distRGB.setTo(cv::Scalar(1, 1, 1), distFrame > maxdistmm);
				distRGB.setTo(cv::Scalar(1, 1, 1), distFrame <= 0);
				// currRGB.setTo(cv::Scalar(1),distFrame <= 0);
			}

			if (gvars.takeSnapshot || gvars.collectData)
			{
				gvars.snaps.snapshots = new list<pair<string, cv::Mat>>();
				gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("absDepth", absDepth.clone()));
			}

			//********************************************
			// fixDepthImageDistance(&currDepth);
			//***********************************
			// DATA PRE-PROCESSING
			//***********************************
			fixDepthImageDistance(&absDepth);
			currDepth = absDepth.clone();
			corrScene = isRGBScene ? absRGB : absDepth;

			if (gvars.takeSnapshot || gvars.collectData)
			{

				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("distFrame",distFrame.clone()));
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("currRGB",currRGB.clone()));
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("distRGB",distRGB.clone()));
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("currDepth",currDepth.clone()));

				int slice = IMGHEIGHT / 4;
				cv::Rect imgbottom(0, (IMGHEIGHT - slice - 1), IMGWIDTH, slice);
				rgbMarked = currRGB.clone();
				depthMarked = absDepth.clone();
				// cv::Mat brgb = currRGB(imgbottom).clone();
				// cv::Mat bdep = absDepth(imgbottom).clone();
				rectangle(rgbMarked, imgbottom, Scalar(0, 0, 255), 1, 8);
				rectangle(depthMarked, imgbottom, Scalar(0, 0, 255), 1, 8);
				cv::putText(depthMarked, "Min: " + to_string(mindistance) + " | Max: " + to_string(maxdistance), Point(0, IMGHEIGHT * 0.1), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 0, 0), 2);
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("delimBottom",rgbMarked));
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("delimBottomDep",depthMarked));
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("bottomRGB",brgb));
				// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("bottomDepth",bdep));
			}

			//*********************************************************
			//
			//	INFORMATION EXTRACTION
			//
			//*********************************************************
			// Caso o frame tenha mudado ou o feedback seja contínuo, o processamento se inicia.
			if (continuousFeedback || (frameChanged() || updateScene || updateThresh))
			{
				if (updateScene)
					updateScene = false;
				avgSliced = NULL;

				//********************************************//
				// BUSCA CAMINHO SEGURO PARA INDICAR		  //
				//********************************************//
				if (pathnav)
				{

					list<pair<int, int>> paths = getSafestPath(absDepth, distFrame, &freepathimg);

					if (paths.size() < 1)
					{
						BOOST_LOG_TRIVIAL(info) << "Couldn't find any path";
						feedback->doPathNotFoundFeedback();
						if (gvars.collectData)
						{
							cv::putText(rgbMarked, "No paths.", Point(round(IMGWIDTH / 4), round(IMGHEIGHT / 2)), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 0, 0), 2);
							gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("rgbMarked", rgbMarked));
							gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("depthMarked", depthMarked));
							CodeUtil::saveSnapshotData();
						}
						continue;
					}
					pair<int, int> avgmd = paths.front();
					double imgmid = IMGWIDTH / 2;
					BOOST_LOG_TRIVIAL(info) << "paths found: " << paths.size();
					if (paths.size() > 1 && !multiPathDetected && abs(difftime(lastMultiPathWarn, time(NULL))) > 10)
					{
						multiPathDetected = true;
						multipaths = paths;
						lastMultiPathWarn = time(NULL);
					}
					else
					{
						if (paths.size() > 1)
							for (auto x : paths)
								if (x.first == imgmid ||
									(x.first < imgmid && x.first + x.second / 2 >= imgmid) ||
									(x.first > imgmid && x.first - x.second / 2 <= imgmid))
									avgmd = x;
						BOOST_LOG_TRIVIAL(info) << "closest path to the center: " << avgmd.first;
						if (avgmd.first - floor(avgmd.second / 2) + avgmd.second >= IMGWIDTH)
							avgmd.second -= abs((avgmd.first - floor(avgmd.second / 2) + avgmd.second) - IMGWIDTH) * 2;
						else if (avgmd.first - floor(avgmd.second / 2) < 0)
							avgmd.first = floor(avgmd.second / 2);
						BOOST_LOG_TRIVIAL(info) << "x: " << avgmd.first - floor(avgmd.second / 2) << " | Width: " << avgmd.second;
						cv::Rect imgpart(avgmd.first - floor(avgmd.second / 2), 0, avgmd.second, IMGHEIGHT);
						int nx = MAX(0, round(avgmd.first - larg / 2));
						cv::Rect imgpartdir(nx, 0, larg, IMGHEIGHT);
						safePathNav = avgmd;
						currDepth = absDepth(imgpartdir);
						currRGB = absRGB(imgpartdir);
						distFrame = distFrame(imgpartdir);
						avgSliced = &avgmd;

						/*safePathNav = avgmd;
						currDepth = absDepth(imgpart);
						currRGB = absRGB(imgpart);
						distFrame = distFrame(imgpart);*/
						/*if(kinPattern)
							currDepth.setTo(cv::Scalar(255),distFrame > maxdistmm);
						else
							currDepth.setTo(cv::Scalar(1),distFrame > maxdistmm);*/

						/*if((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL){
							int slice = IMGHEIGHT / 4;
							//cv::Mat chosenPath = corrRGB.clone();
							cv::Mat chosenPath = absRGB.clone();
							cv::Rect imgpath(avgmd.first-floor(avgmd.second/2),IMGHEIGHT-slice,avgmd.second,slice);
							rectangle(chosenPath,imgpath,Scalar(0,0,255),1,8);
							gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("chosenPath",chosenPath));
						}*/
					}
				}
				//*****************************************************************************************************
				//*****************************************************************************************************
				detectedObject = NULL;
				BOOST_LOG_TRIVIAL(info) << "***************************** B4 Detect ********************************";
				if ((!multiPathDetected && config->getNavigationType() == NavigationType::BothNT) || config->getNavigationType() == NavigationType::Obstacle)
				{
					cv::Mat imgbit = currDepth.clone();

					if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
						gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("dilateClose", imgbit.clone()));

					/*thresh = dist2pix(imgbit,distFrame,maxdistmm);*/

					cv::Mat tmpdst = distFrame.clone();
					tmpdst.setTo(Scalar(maxdistmm + 1), tmpdst == 0);

					cv::minMaxLoc(tmpdst, &mind, &maxd, &mind_loc, &maxd_loc);

					cv::Mat imgb1 = imgbit.clone();
					if (maxdistmm > mind)
					{
						// imgb1.setTo(Scalar(1),distFrame>maxdistmm);
						cv::minMaxLoc(imgbit, &minp, &maxp, &minp_loc, &maxp_loc);
						cv::floodFill(imgb1, maxp_loc, Scalar(0), 0, maxp * 0.1, maxp * 0.1, 4 | (0 << 8) | FLOODFILL_FIXED_RANGE);
						imgb1.setTo(Scalar(255), imgb1 > 0);

						// threshold(imgbit,imgbit,thresh,255,CV_THRESH_BINARY);

						// threshold(imgb1,imgb1,127,255,CV_THRESH_BINARY);

						Canny(imgb1, threshOut, 1, 0, 3);
						if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
						{
							gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("threshold", imgbit.clone()));
							gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("canny", threshOut.clone()));
						}
						// findContours(threshOut,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
						bitwise_not(imgb1, imgb1);
						findContours(imgb1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
						if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
							gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("contours", threshOut.clone()));

						//**********************************************
						// FEEDBACK DO OBJETO DETECTADO
						//****************************************
						BOOST_LOG_TRIVIAL(info) << "***************************** B5 Detect ********************************";
						detectedObject = getClosestObject(imgbit, distFrame, contours);
						BOOST_LOG_TRIVIAL(info) << "***************************** B6 Detect ********************************";
					}
				}
				if (detectedObject != NULL)
				{
					// fixBoundingRect(detectedObject,25);
					// detectedObject->rgbImage = currRGB;

					detectedObject->rgbImage = absRGB;
					detectedObject->processRGBRegion();
					if (avgSliced != NULL)
					{
						detectedObject->centroid.x += avgSliced->first;
						detectedObject->bounds.x = avgSliced->first + detectedObject->bounds.x;
						cv::Mat dobj = absDepth.clone();
						double mp = dobj.at<uchar>(detectedObject->centroid);
						cv::floodFill(dobj, detectedObject->centroid, Scalar(0), 0, mp * 0.1, mp * 0.1, 4 | (0 << 8) | FLOODFILL_FIXED_RANGE);
						dobj.setTo(Scalar(255), dobj > 0);
						bitwise_not(dobj, dobj);
						findContours(dobj, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
						detectedObject = getClosestObject(dobj, absDistFrame, contours);
						if (detectedObject != NULL)
						{
							detectedObject->rgbImage = absRGB;
							detectedObject->processRGBRegion();
							// imshow("OBS",detectedObject->rgbRegion);
							// cvWaitKey(30);
						}
					}

					if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
					{
						if (avgSliced)
							rectangle(absRGB, detectedObject->bounds.tl(), detectedObject->bounds.br(), Scalar(0, 255, 255), 2, 8, 0);
						else
							rectangle(currRGB, detectedObject->bounds.tl(), detectedObject->bounds.br(), Scalar(0, 255, 255), 2, 8, 0);

						gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("obstacle", detectedObject->rgbRegion.clone()));
					}
					/*if(!pathnav){
						thresh = MIN_THRESH;
						updateThresh = false;
					}*/
					BOOST_LOG_TRIVIAL(info) << "***************************** B7 Detect ********************************";

					//**********************************
					//  INFORMATION TO FEEDBACK
					//**********************************
					feedback->doObjectDetectionFeedback(detectedObject);

					BOOST_LOG_TRIVIAL(info) << "***************************** B8 Detect ********************************";
				}
				else if (!pathnav)
				{
					BOOST_LOG_TRIVIAL(info) << "***************************** B9 Detect ********************************";
					//**********************************
					//  INFORMATION TO FEEDBACK
					//**********************************
					feedback->callAutoFeedback("Nenhum obstáculo.", Direction::back, VibrationType::ACK);
					BOOST_LOG_TRIVIAL(info) << "***************************** B10 Detect ********************************";
					/*if(kinPattern){
						thresh += 10;
						if(thresh > 250)
							thresh = MIN_THRESH;
					}
					else{
						thresh -= 10;
						if(thresh < 20)
							thresh = MIN_THRESH;
					}
					updateThresh = true;
					cout << "Closest object not found. Adjusting threshold to " << thresh << endl;*/
				}
				//**********************************************
				// FEEDBACK DO CAMINHO SEGURO
				//****************************************
				else
				{
					if (multiPathDetected)
					{
						multiPathDetected = false;
						//**********************************
						//  INFORMATION TO FEEDBACK
						//**********************************
						list<string> dirs = feedback->doPathNavigationFeedback(multipaths, IMGWIDTH);
						if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
						{
							int slice = IMGHEIGHT / 4;
							// cv::Mat chosenPath = absRGB.clone();
							for (pair<int, int> p : multipaths)
							{
								cv::Rect imgpath(p.first - floor(p.second / 2), IMGHEIGHT - slice, p.second, slice);
								rectangle(rgbMarked, imgpath, Scalar(0, 255, 0), 1, 8);
								cv::putText(rgbMarked, dirs.front(), Point(p.first, IMGHEIGHT - (slice / 2)), FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 0), 2);
								dirs.pop_front();
							}
							// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("chosenPath",chosenPath));
						}
					}
					else
					{
						//**********************************
						//  INFORMATION TO FEEDBACK
						//**********************************
						const char *dir = feedback->doPathNavigationFeedback(safePathNav, IMGWIDTH);
						BOOST_LOG_TRIVIAL(info) << "(CVLayer) ***********Caminho***********: " << dir;
						if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
						{
							int slice = IMGHEIGHT / 4;
							// cv::Mat chosenPath = absRGB.clone();
							cv::Rect imgpath(safePathNav.first - floor(safePathNav.second / 2), IMGHEIGHT - slice, safePathNav.second, slice);
							rectangle(rgbMarked, imgpath, Scalar(0, 255, 0), 1, 8);
							cv::putText(rgbMarked, dir, Point(safePathNav.first, IMGHEIGHT - (slice / 2)), FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 2);
							// gvars.snaps.snapshots->push_front(pair<string,cv::Mat>("chosenPath",rgbMarked));
						}
					}
				}
			}
		}
		if (gvars.takeSnapshot && gvars.snaps.snapshots != NULL)
		{
			CodeUtil::saveSnapshots(*gvars.snaps.snapshots);
			list<pair<string, cv::Mat>> *aux = gvars.snaps.snapshots;
			gvars.snaps.snapshots = NULL;
			delete (aux);
			gvars.takeSnapshot = false;
		}

		if (gvars.collectData)
		{
			gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("rgbMarked", rgbMarked));
			gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("depthMarked", depthMarked));
			CodeUtil::saveSnapshotData();
		}

		if (continuousFeedback)
		{
			config->reloadConfig();
			feedbackSpeed = config->getFeedbackSpeed();
			usleep(feedbackSpeed * feedbackSpeed * 5000);
		}
		toc();
		BOOST_LOG_TRIVIAL(info) << "###################### Fim do processamento do frame ############################";
	}
}

double CVLayer::imgCorrelation(cv::Mat curr, cv::Mat prev)
{
	cv::Mat f1, f2;
	curr.convertTo(f1, CV_32F);
	prev.convertTo(f2, CV_32F);

	int pix = f1.rows * f1.cols;

	Scalar f1m, f1std, f2m, f2std;
	meanStdDev(f1, f1m, f1std);
	meanStdDev(f2, f2m, f2std);

	double covariance = ((cv::Mat)(f1 - f1m)).dot(f2 - f2m) / pix;
	double correlation = covariance / (f1std[0] * f2std[0]);

	return correlation;
}

CVLayer::~CVLayer()
{
	/*
		RealSense435i* r = dynamic_cast<RealSense435i*>(camera);
		Zed* z = dynamic_cast<Zed*>(camera);
		Kinect* k = dynamic_cast<Kinect*>(camera);
		if(z!=NULL){
			z->~Zed();
			delete z;
		}
		else if(r!=NULL){
			r->~RealSense435i();
			delete r;
		}
		else if(k!=NULL){
			k->~Kinect();
			delete k;
		}
	 */
	cv::destroyAllWindows();
}

bool CVLayer::frameChanged()
{

	if (pcorrScene.empty())
	{
		return true;
	}

	if (corrqueue.size() <= 10)
	{
		corrqueue.add(imgCorrelation(corrScene, pcorrScene));
		return false;
	}
	else
	{
		double ccorr = imgCorrelation(corrScene, pcorrScene);
		double corr = cv::abs(corrqueue.getMean() - ccorr);
		// corr = corr < 0 ? corr * -1 : corr;
		if (corr >= 0.1)
		{
			BOOST_LOG_TRIVIAL(info) << "Image changed: " << corr;
			corrqueue.clear();
			corrqueue.add(ccorr);
			return true;
		}
		else
		{
			corrqueue.add(ccorr);
			return false;
		}
	}
}

DetectedObject *CVLayer::getClosestObject(cv::Mat img, cv::Mat distFrame, vector<vector<Point>> contours)
{

	DetectedObject *dobj;
	int contPos;
	vector<Moments> moments(contours.size());
	float sum = 0;
	// float minArea;
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		moments[i] = cv::moments(contours[i], false);
		sum += moments[i].m00;
	}
	float med = sum / contours.size();
	Point2f minDist;
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		if (moments[i].m00 >= med)
		{
			int x = (int)(moments[i].m10 / moments[i].m00);
			int y = (int)(moments[i].m01 / moments[i].m00);
			Point2f mc(x, y);
			if (isCentroidValid(mc, img.size))
			{
				// sl::float1 dist = distFrame.at<sl::float1>(mc);
				float dist = camera->getPixelDistance(mc);
				if (i == 0 || ((dist < distFrame.at<float>(minDist))))
				{ // && (mc.x >= img.cols/2-larg/2 && mc.x <= img.cols/2+larg/2))){
					minDist = mc;
					contPos = i;
				}
			}
		}
	}
	if (!isCentroidValid(minDist, img.size) || minDist.x == 0)
		return NULL;

	// cout << "Area: " << contourArea(contours[contPos]) << endl;

	vector<Point> contours_poly;
	Rect boundRect;
	approxPolyDP(cv::Mat(contours[contPos]), contours_poly, 3, true);
	boundRect = boundingRect(cv::Mat(contours_poly));

	dobj = new DetectedObject;

	dobj->centroid = minDist;
	dobj->centroidDistance = camera->getPixelDistance(minDist);
	dobj->bounds = boundRect;
	dobj->detectionTime = time(NULL);

	// cout << "Dist: " << dobj->centroidDistance << " | Area: " << moments[contPos].m00 << endl;

	return dobj;
}

bool CVLayer::isCentroidValid(Point2f mc, cv::MatSize img)
{
	return !(mc.x > img().width ||
			 mc.x < 0 ||
			 mc.y > img().height ||
			 mc.y < 0);
}

cv::Rect reshapeRect(cv::Mat m, Rect r)
{
	int px = 50;
	int mw = m.cols;
	int mh = m.rows;

	r.x = (r.x - px <= 0) ? 0 : r.x - px;
	r.width = (r.x + r.width + px >= mw) ? r.width : r.width + px;
	r.width = (r.x + r.width + px >= mw) ? r.width : r.width + px;
	r.y = (r.y - px <= 0) ? 0 : r.y - px;
	r.height = (r.y + r.height + px >= mh) ? r.height : r.height + px;
	r.height = (r.y + r.height + px >= mh) ? r.height : r.height + px;

	return r;
}

void CVLayer::fixBoundingRect(DetectedObject *dobj, int pix)
{
	// cout << "Before: " << *rect << endl;

	cv::Rect *rect = &(dobj->bounds);

	int wplus = round(rect->width * 0.5);
	int hplus = round(rect->height * 0.5);
	int nw = rect->width + wplus;
	int nh = rect->height + hplus;
	rect->width = rect->x - (round(nw / 2)) || rect->x + (round(nw / 2)) > dobj->rgbImage.cols ? rect->width : nw;
	rect->height = rect->y - (round(nh / 2)) || rect->y + (round(nh / 2)) > dobj->rgbImage.rows ? rect->height : nh;
	// cout << "XX " << rect->x << " | YY: " << rect->y << endl;
	// cout << "Width: " << rect->width << " | Height: " << rect->height << endl;
	// cout << rect->x-pix << endl;
	// cout << rect->y-pix << endl;
	rect->x = rect->x - pix <= 1 ? 0 : rect->x - pix;
	rect->y = rect->y - pix <= 1 ? 0 : rect->y - pix;
}

std::thread CVLayer::startVideoProcess()
{
	return std::thread([=]
					   { processVideo(); });
}

list<pair<int, int>> CVLayer::getSafestPath(cv::Mat img, cv::Mat distMat, cv::Mat *fpathret)
{
	float navdist = config->getMinimumDistance();
	navdist *= 1000;
	cv::Mat diff;
	list<pair<int, int>> pathxcoord;
	cv::Mat imgbit = img.clone();
	int slice = IMGHEIGHT / 4;
	cv::Rect recsub(0, (IMGHEIGHT - slice - 1), IMGWIDTH, slice);
	cv::Mat bpiece = imgbit(recsub).clone();
	// imshow("asdasdasssssd",bpiece);
	// cvWaitKey(30);

	cv::Mat bdist = distMat(recsub).clone();
	cv::Mat allb = cv::Mat::ones(bpiece.rows, bpiece.cols, CV_8UC1);
	allb.setTo(cv::Scalar(255), allb == 1);
	cv::compare(bpiece, allb, diff, CMP_NE);
	int nz = countNonZero(diff);
	if (nz == 0)
		return pathxcoord;

	cv::Mat threshOut;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	cv::Mat bp2 = bpiece.clone();

	int navdst = dist2pix(bpiece, bdist, navdist);

	// std::cout << navdst << std::endl;

	threshold(bpiece, bpiece, navdst, 255, CV_THRESH_BINARY_INV);
	// imshow("i1",bpiece);
	// cvWaitKey(30);

	pathxcoord.clear();
	/*
	 ******Maneira alternativa, por pixels
	 ******Talvez possa ser usada em conjunto


	Mat cpathw = Mat::ones(5,larg,CV_8UC1);
	cpathw.setTo(Scalar(255),cpathw==0);
	int largacum = larg;
	int iant = -1;

	for(int i=0;i<IMGWIDTH-larg;i++){
		inter = bpiece(cv::Rect(i,slice-5,larg,5)).clone();
		compare(inter,cpathw,diff,CMP_NE);
		int nz = countNonZero(diff);

		if(nz!=0 && largacum > larg){
			int npos = iant+(largacum/2);
			if(pathxcoord.empty())
				pathxcoord.pop_front();
			pathxcoord.push_front(npos);
			largacum=larg;
		}
		if(nz==0){
			if(pathxcoord.empty()){
				pathxcoord.push_front(i+larg/2);
				iant=i+1;
				i+=larg;
			}
			else if(i-larg<=iant*1.2 || i-largacum<=iant*1.2){
				cout << "acumulando" << endl;
				largacum += larg;
				i+=larg;
			}
			else if(iant >= 0 && i-larg > iant){
				cout << "i: " << i << "  | iant: " << iant << " | larg:" << largacum << endl;
				pathxcoord.push_front(i+larg/2);
				iant=i+1;
				i+=larg;
			}
		}
	}*/
	Mat inter = Mat::ones(5, larg, CV_8UC1);

	cv::Mat cpathb = cv::Mat::zeros(5, 1, CV_8UC1);
	for (int i = 0; i < IMGWIDTH; i++)
	{
		inter = bpiece(cv::Rect(i, slice - 5, 1, 5)).clone();
		compare(inter, cpathb, diff, CMP_NE);
		int nz = countNonZero(diff);
		if (nz == 0)
			bpiece.col(i).setTo(cv::Scalar(0));
	}

	if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
		gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("paths", bpiece.clone()));

	*fpathret = bpiece;

	cv::Mat bpcopy = bpiece.clone();
	cv::findContours(bpcopy, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	vector<Moments> moments(contours.size());
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		moments[i] = cv::moments(contours[i], false);
	}
	cv::Mat fpath = cv::Mat::ones(slice, larg, CV_8UC1);
	fpath.setTo(cv::Scalar(255), fpath == 1);
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		int x = (int)(moments[i].m10 / moments[i].m00);
		int y = (int)(moments[i].m01 / moments[i].m00);
		Point2f mc(x, y);
		if (mc.x + larg / 2 > IMGWIDTH || mc.x - larg / 2 < 0)
			continue;
		inter = bpiece(cv::Rect(mc.x - larg / 2, 0, larg, slice)).clone();
		compare(inter, fpath, diff, CMP_NE);
		int nz = countNonZero(diff);
		if (nz == 0)
		{
			pathxcoord.push_front(pair<int, int>(x, boundingRect(contours[i]).width));
		}
	}
	return pathxcoord;
}

void CVLayer::fixDepthImageDistance(cv::Mat *imgbit)
{
	cv::Mat sEl = getStructuringElement(cv::MORPH_RECT, Size(21, 21));
	cv::morphologyEx(*imgbit, *imgbit, cv::MORPH_DILATE, sEl);
	cv::morphologyEx(*imgbit, *imgbit, cv::MORPH_CLOSE, sEl);
	// imgbit->setTo(cv::Scalar(255),*imgbit==0);

	// Best morph filter
	/*float node20_data[3]={1,1,1};
	cv::Mat node20=cv::Mat(3,1,CV_32F,node20_data);
	node20.convertTo(node20,CV_8UC1);
	float node32_data[49]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	cv::Mat node32=cv::Mat(7,7,CV_32F,node32_data);
	node32.convertTo(node32,CV_8UC1);
	float node33_data[169]={0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0};
	cv::Mat node33=cv::Mat(13,13,CV_32F,node33_data);
	node33.convertTo(node33,CV_8UC1);
	cv::Mat node1=imgbit->clone();
	float node28_data[5]={1,1,1,1,1};
	cv::Mat node28=cv::Mat(1,5,CV_32F,node28_data);
	node28.convertTo(node28,CV_8UC1);
	cv::Mat node42;
	cv::morphologyEx(node1,node42,cv::MORPH_ERODE,node28);
	cv::Mat node55;
	cv::morphologyEx(node42,node55,cv::MORPH_DILATE,node33);
	cv::Mat node66;
	cv::morphologyEx(node55,node66,cv::MORPH_DILATE,node33);
	cv::Mat node126;
	cv::morphologyEx(node66,node126,cv::MORPH_DILATE,node32);
	float node5_data[3]={1,1,1};
	cv::Mat node5=cv::Mat(1,3,CV_32F,node5_data);
	node5.convertTo(node5,CV_8UC1);
	cv::Mat node83;
	cv::morphologyEx(node5,node83,cv::MORPH_ERODE,node20);
	cv::Mat node163;
	cv::morphologyEx(node126,node163,cv::MORPH_DILATE,node83);
	cv::Mat node384;
	cv::morphologyEx(node163,node384,cv::MORPH_DILATE,node20);
	*imgbit=node384.clone();
	imgbit->setTo(cv::Scalar(1),*imgbit==0);*/
}

Cameras CVLayer::checkCamera()
{
	libusb_device **devs;
	libusb_context *ctx = NULL;
	int r;
	ssize_t cnt;
	r = libusb_init(&ctx);
	if (r < 0)
	{
		BOOST_LOG_TRIVIAL(error) << "Init Error " << r;
		return Cameras::NONE;
	}
	libusb_set_debug(ctx, 3);
	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0)
	{
		BOOST_LOG_TRIVIAL(error) << "Get Device Error";
	}
	ssize_t i;
	std::string resV;
	std::string resP;
	for (i = 0; i < cnt; i++)
	{
		libusb_device_descriptor desc; // print specs of this device
		int r = libusb_get_device_descriptor(devs[i], &desc);
		if (r < 0)
		{
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
		if (resV.compare(usbID[Cameras::ZED]) == 0)
		{
			libusb_free_device_list(devs, 1); // free the list, unref the devices in it
			libusb_exit(ctx);				  // close the session
			return Cameras::ZED;
		}
		else if (resV.compare(usbID[Cameras::D435I]) == 0)
		{
			libusb_free_device_list(devs, 1); // free the list, unref the devices in it
			libusb_exit(ctx);				  // close the session
			return Cameras::D435I;
		}
		else if (resV.compare(usbID[Cameras::KINECT]) == 0)
		{
			libusb_free_device_list(devs, 1); // free the list, unref the devices in it
			libusb_exit(ctx);				  // close the session
			return Cameras::KINECT;
		}
	}
	libusb_free_device_list(devs, 1); // free the list, unref the devices in it
	libusb_exit(ctx);				  // close the session
	return Cameras::NONE;
}

DepthCamera *CVLayer::getCamera()
{
	return camera;
}

int CVLayer::dist2pix(cv::Mat pix, cv::Mat dist, float navdist)
{
	cv::Point pos;
	cv::Mat absdff;
	double min, max;
	cv::Point min_loc, max_loc;
	cv::Mat dstMat = dist.clone();
	cv::minMaxLoc(dstMat, &min, &max, &min_loc, &max_loc);
	dstMat.setTo(cv::Scalar(max), dstMat < navdist);
	cv::minMaxLoc(dstMat, &min, &max, &min_loc, &max_loc);
	int navdstp = (int)pix.at<uchar>(min_loc);
	return navdstp;
}
