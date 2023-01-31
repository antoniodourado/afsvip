/*
 * CodeUtil.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "CodeUtil.h"

CodeUtil::CodeUtil() {
	// TODO Auto-generated constructor stub

}


char* CodeUtil::randomString(int len) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char* out = (char*) malloc(len+1);

    srand(time(NULL));

    for (int i = 0; i <= len; ++i) {
        out[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return out;
}

int CodeUtil::randomInt(){
	srand(time(NULL));
	return rand() % 1000;
}

string CodeUtil::char2string(const char* word){
	string x(word,strlen(word));
	return x;
}

CodeUtil::~CodeUtil() {
	// TODO Auto-generated destructor stub
}

string CodeUtil::trim(string allocator) {
	return boost::algorithm::trim_copy(allocator);
}


std::vector<string> CodeUtil::splitString(string allocator, char delim) {
	std::vector<string> elems;
	std::stringstream ss;
	ss.str(allocator);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

double CodeUtil::CorrelationQueue::getMean() {
	double sum = 0;
	for(int i=0;i<this->size();i++)
		sum = sum + this->at(i);
	return sum/this->size();
}

void CodeUtil::CorrelationQueue::add(double double1) {
	if(this->size() >= LIMIT)
		this->pop_front();
	this->push_back(double1);
}

void CodeUtil::CorrelationQueue::clear(){
	while(this->size() > 0)
		this->pop_front();
}

void CodeUtil::imHist(cv::Mat mat, int bins, double min=0, double max=255, bool bool1=false) {
	float hranges[2] = {min,max};
	const float* ranges[1] = {hranges};
	int channels[1] = {0};
	cv::MatND hist;

	cv::calcHist(&mat,
				1,         //histogram from 1 image only
				0,  //the channel used
				cv::Mat(), //no mask is used
				hist,      //the resulting histogram
				1,         //it is a 1D histogram
				&bins,  //number of bins
				ranges,     //pixel value range
				true);

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/bins );
	cv::Mat histImg( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );
	normalize(hist, hist, 0, histImg.rows, NORM_MINMAX, -1, cv::Mat() );

	double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);




    for(int i=1;i<=bins;i++){
    	float binVal = hist.at<float>(i);
    	rectangle(histImg,
    			  Point(bin_w*(i-1),hist_h - cvRound(hist.at<float>(i-1))),
				  Point(bin_w*(i),hist_h-cvRound(hist.at<float>(i))),
				  Scalar(0,0,0),
				  CV_FILLED);
    }

    imshow("Hist",histImg);

}

void CodeUtil::saveSnapshots(list<pair<string, cv::Mat>> snaps){
	time_t tnow = time(NULL);
	const char* t0 = "mkdir -p ";
	const char* t1 = "Assets/";
	const char* t2 = getYMDTime();
	const char* t3 = "/";
	char* text = (char*) malloc(strlen(t0)+strlen(t1)+strlen(t2)+strlen(t3)+1);
	strcpy(text,t0);
	strcat(text,t1);
	strcat(text,t2);
	strcat(text,t3);
	const int err = system(text);
	if(err == -1){
		BOOST_LOG_TRIVIAL(error) << "Error during creation of snapshot folder. STOP.";
		return;
	}
	char* fpath = (char*) malloc(strlen(t1)+strlen(t2)+strlen(t3));
	strcpy(fpath,t1);
	strcat(fpath,t2);
	strcat(fpath,t3);

	//cout << fpath << endl;

	for(auto snap : snaps){
		const char* name = snap.first.c_str();
		cv::Mat img = snap.second;
		const char* ext = ".jpg";
		char* fullpath = (char*) malloc(strlen(fpath)+strlen(name)+strlen(ext)+1);
		strcpy(fullpath,fpath);
		strcat(fullpath,name);
		strcat(fullpath,ext);

		//cout << fullpath << endl;
		imwrite(fullpath,img);
	}


}

pair<const char*,const char*> CodeUtil::createSnapshotFilePath(){
	time_t tnow = time(NULL);
	const char* t0 = "mkdir -p ";
	const char* t1 = "Assets/";
	const char* t2 = getYMDTime();
	const char* t3 = "/";
	char* text = (char*) malloc(strlen(t0)+strlen(t1)+strlen(t2)+strlen(t3)+1);
	strcpy(text,t0);
	strcat(text,t1);
	strcat(text,t2);
	strcat(text,t3);
	const int err = system(text);
	if(err == -1){
		BOOST_LOG_TRIVIAL(error) << "Error during creation of snapshot folder. STOP.";
		exit(1);
	}
	char* fpath = (char*) malloc(strlen(t1)+strlen(t2)+strlen(t3));
	strcpy(fpath,t1);
	strcat(fpath,t2);
	strcat(fpath,t3);

	return pair<const char*,const char*>(fpath,t2);
}

bool CodeUtil::saveSnapshotData(){
	if(!gvars.collectData)
		return false;
	const char* fpath = gvars.snaps.fpath;

	//cout << fpath << endl;

	vector<string> files;
	const char* ext = ".jpg";
	const char* undr = "_";
	string numsnap = to_string(gvars.snaps.snapshotnum);
	for(pair<string,cv::Mat> snap : *gvars.snaps.snapshots){
		const char* name = snap.first.c_str();
		cv::Mat img = snap.second;
		char* fullpath = (char*) malloc(strlen(fpath)+strlen(gvars.snaps.tid)+strlen(name)+2*strlen(undr)+strlen(numsnap.c_str())+strlen(ext)+1);
		strcpy(fullpath,fpath);
		strcat(fullpath,gvars.snaps.tid);
		strcat(fullpath,undr);
		strcat(fullpath,numsnap.c_str());
		strcat(fullpath,undr);
		strcat(fullpath,name);
		strcat(fullpath,ext);
		std::thread wthread = imgwrite_thread(fullpath,img);
		wthread.detach();
		files.push_back(std::string(fullpath));
	}
	string cmd = "tar -cjf "+string(fpath)+string(gvars.snaps.tid)+"_"+numsnap+".tar.bz2";
	for(string f : files)
		cmd = cmd+" "+f;
	cmd += " --remove-files &";
	//system(cmd.c_str());
	//BOOST_LOG_TRIVIAL(info) << "Saving Feedback Set " << numsnap << " as " << string(fpath)+string(gvars.snaps.tid)+"_"+numsnap+".tar.bz2";
	//std::thread cmdthread = systemCommand_thread(cmd.c_str());
	//cmdthread.detach();
	//system(cmd.c_str());
	list<pair<string,cv::Mat>>* aux = gvars.snaps.snapshots;
	gvars.snaps.snapshots = NULL;
	delete aux;
	gvars.snaps.snapshotnum++;
	return true;
}

std::thread CodeUtil::imgwrite_thread(const char* fp, cv::Mat img) {
	return std::thread([=] {imwrite(fp,img);});
}

std::thread CodeUtil::systemCommand_thread(const char* cmd) {
	cout << cmd << endl;
	return std::thread([=] {system(cmd);});
}
