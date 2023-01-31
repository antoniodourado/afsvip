/*
 * PILayer.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */
#ifndef SRC_PILAYER_H_
#define SRC_PILAYER_H_
#include <ncurses.h>
#include <thread>
#include "FeedbackLayer.h"
#include "CVLayer.h"
#include "__afsvipGLOBALS.h"
#include <fcntl.h>
#include <libinput.h>
#include <libudev.h>
#include <poll.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <vector>
#include <string>
#include <boost/variant.hpp>
#include <map>
#include <signal.h>
#include <errno.h>
#include "Configuration.h"

using namespace boost;

typedef variant<int,float,bool,std::string> tMapType;
typedef map<std::string, tMapType> FuncParam;

//********************************** 
//  INTERACTOR 1
//**********************************

enum UserSetup{
	USERHEIGHT,
	NOTCHOSEN
};

struct TempUserSetup{
	int heightdigit;
	int height;
};

class PILayer {
public:
	PILayer(Configuration*,FeedbackLayer*);
	PILayer(Configuration*,FeedbackLayer*,DepthCamera*);
	virtual ~PILayer();
	std::thread startListening();
	Configuration* config;
	FeedbackLayer* feedback;
	double kpresstime=2000;
	void startKeyboardListener();

	enum KBLabels{
		KP0 = KEY_KP0,
		KP1 = KEY_KP1,
		KP2 = KEY_KP2,
		KP3 = KEY_KP3,
		KP4 = KEY_KP4,
		KP5 = KEY_KP5,
		KP6 = KEY_KP6,
		KP7 = KEY_KP7,
		KP8 = KEY_KP8,
		KP9 = KEY_KP9,
		KPA = -1,
		KPB = KEY_KPDOT,
		KPC = KEY_KPENTER,
		KPD = KEY_BACKSPACE,
		KPE = KEY_KPPLUS,
		KPF = KEY_KPMINUS,
		KPG = KEY_KPASTERISK,
		KPH = KEY_KPSLASH,
		KPI = -1,
		KPX = KEY_NUMLOCK
	};

private:
	DepthCamera* camera = NULL;
	struct libinput *linput = NULL;
	struct pollfd fds;
	struct libinput_event_keyboard* kEvent  = NULL;
	struct libinput_event *evt  = NULL;
	struct sigaction act;
	vector<pair<uint32_t,double>> kplist;
	uint32_t keyCode = 0;
	std::map<int,string> keymap;
	bool keyedit=false;
	uint32_t editKeyCode = 0;
	int editableKeys[10] = {KEY_KP0,KEY_KP1,KEY_KP2,KEY_KP3,KEY_KP4,KEY_KP5,KEY_KP6,KEY_KP7,KEY_KP8,KEY_KP9};
	uint32_t kpNumericKeys[10] = {KEY_KP0,KEY_KP1,KEY_KP2,KEY_KP3,KEY_KP4,KEY_KP5,KEY_KP6,KEY_KP7,KEY_KP8,KEY_KP9};
	int userSetupKeys[5] = {KEY_KPASTERISK,KEY_KP7,KEY_KP8,KEY_KPPLUS,KEY_KPMINUS};
	std::string idkeypad = "1a2c:2124";
	UserSetup activesetup = UserSetup::NOTCHOSEN;
	//FeedbackMethod fmt;
	struct TempUserSetup tempsetup;

private:
	void processKeyDown(libinput_event_keyboard*);
	void processKeyUp(libinput_event_keyboard*);
	int keyPressedIdx(vector<pair<uint32_t,double>>,uint32_t);
	void processKey(uint32_t,double);
	void processKeyFunc(int,bool,uint32_t);
	void setKeyboardKeys();
	int getNumericFromKeycode(uint32_t);
};


class ParamHandler
{
public:
	ParamHandler(){};
	~ParamHandler(){};

	bool exists(const string&);

	template<typename T> void set(const std::string& key, const T& value)
	{
		params[key] = value;
	}
	template<typename T> const T& get(const std::string& key)const
	{
		if (!exists(key))
			throw "Can't find key";

		return boost::get<T>(params.find(key)->second);
	}
	bool exists(const std::string& key)const
	{
		if (params.find(key) == params.end())
			return false;

		return true;
	}
private:
	FuncParam params;
};


#endif /* SRC_PILAYER_H_ */
