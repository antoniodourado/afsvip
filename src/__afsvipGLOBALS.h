/*
 * globals.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC___AFSVIPGLOBALS_H_
#define SRC___AFSVIPGLOBALS_H_
#include <iostream>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <string.h>
#include <sstream>
#include <chrono>
#include <utility>
#include <time.h>
#include <atomic>
#include <mutex>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <opencv2/core.hpp>
#include <cstdarg>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

#ifndef _PI
const double _PI = 3.14159265358979323846;
#endif

using namespace std;

typedef std::chrono::high_resolution_clock::time_point TPointVar;

enum sysState
{
	OFF = 0,
	ON = 1,
	USERSETUP = 2
};

enum Cameras
{
	NONE,
	ZED,
	KINECT,
	D435I,
};

enum VibrationType
{
	OPT_IN,
	OPT_OUT,
	ACK,
	NTIMES_FAST,
	NTIMES_SLOW,
	CAM_RAISE,
	CAM_LOWER,
	SAVED,
	ERROR,
	REQ_PHONE,
	UHEIGHT,
	VIB_1,
	VIB_2,
	VIB_3,
	VIB_4,
	VIB_5,
	VIB_6,
	VIB_7,
	VIB_8,
	VIB_9,
	VIB_0

};

struct snapshotdata
{
	list<pair<string, cv::Mat>> *snapshots = NULL;
	int snapshotnum = 1;
	const char *fpath;
	const char *tid;
};

// extern bool updatedDistanceConfig = false;
extern struct globalvars
{
	bool SYSDEBUGMODE = false;
	bool takeSnapshot = false;
	bool collectData = false;
	bool isFreenectDriver = false;
	bool stopSystemFeedback = false;
	bool haltSystem = false;
	bool setup = false;
	bool stopVoiceListener = false;
	bool stopKeyListener = false;
	bool sendingFeedback = false;
	bool systemBoot = false;
	TPointVar userSetupTic;
	bool shutdownComputer = false;
	bool soundInUse = false;
	bool headless = false;
	list<pair<string, cv::Mat>> *snapshots = NULL;
	int snapshotnum = 1;
	struct snapshotdata snaps;
	bool systemVolume = true;
	bool reducedMotors = true;
	sysState systemState = sysState::OFF;
} gvars;

extern std::mutex configmutex;
extern const string SYSLOGDATETIME;

// extern bool SYSDEBUGMODE;
// extern bool takeSnapshot;
// extern bool isFreenectDriver;
// extern bool globalvars.stopSystemFeedback;
// extern bool globalvars.haltSystem;
// extern bool setup;
// extern bool stopVoiceListener;
// extern bool stopKeyListener;
extern TPointVar timeTic, timeToc;
char *randomString(int);
int randomInt();
std::string trim(std::string);
std::vector<std::string> splitString(std::string, char);
TPointVar getCurrTimePoint();
double compareTPoint(TPointVar, TPointVar);
void tic();
TPointVar tic1();
void toc(std::string = "tic............................TOC: ");
double toc1();
double tocX(TPointVar);
char *getYMDTime();
void init_logging();
int calcRecCameraAngle(int, int, int, int);
double degree2rad(int);
int rad2degree(double);

#endif /* SRC___AFSVIPGLOBALS_H_ */
