/*
 * CodeUtil.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_CODEUTIL_H_
#define SRC_CODEUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <deque>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstdlib>
#include <thread>
#include "__afsvipGLOBALS.h"

using namespace cv;
using namespace std;

class CodeUtil {
public:
	CodeUtil();
	virtual ~CodeUtil();
	static void saveSnapshots(list<pair<string,cv::Mat>>);
	static pair<const char*,const char*> createSnapshotFilePath();
	static bool saveSnapshotData();
	static std::thread imgwrite_thread(const char*,cv::Mat);
	static std::thread systemCommand_thread(const char*);
	char* randomString(int);
	int randomInt();
	string char2string(const char*);
	static string trim(string);
	static std::vector<string> splitString(string,char);
	static void imHist(cv::Mat,int,double,double,bool);
	class CorrelationQueue : public deque<double>{
	public:
		double getMean();
		void add(double);
		void clear();
	private:
		static const int LIMIT = 100;
	};
};

#endif /* SRC_CODEUTIL_H_ */
