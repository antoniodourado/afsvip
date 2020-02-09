/*Multimodal, Customizable and Interactive Navigation System for Visually Impaired People 
Copyright (C) 2020 Antonio Miguel Batista Dourado

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/
/*
 * CodeUtil.h
 *
 *  Created on: 10 de ago de 2016
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
