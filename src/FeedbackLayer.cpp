/*
 * FeedbackLayer.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "FeedbackLayer.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

FeedbackLayer::FeedbackLayer(AudioFeedback *audioFeedback, Configuration *conf, VoiceFeedback *v)
{
	audio = audioFeedback;
	config = conf;
	voice = v;
}

FeedbackLayer::FeedbackLayer(AudioFeedback *audioFeedback, Configuration *conf, VoiceFeedback *v, TactileFeedback *t)
{
	audio = audioFeedback;
	config = conf;
	voice = v;
	tactile = t;
}

FeedbackLayer::~FeedbackLayer()
{
	delete cclass;
}

//**********************************
//  FEEDBACK DISPATCHERS
//**********************************

const char *FeedbackLayer::doObjectDetectionFeedback(DetectedObject *detectedObject)
{
	object = detectedObject;
	lastObject = *object;
	lobject = &lastObject;
	config->reloadConfig();
	// bool isVoice = config->isVoiceFeedback();
	float xPos = getLRAudioPosFromObj(detectedObject);
	std::thread audiot;
	std::thread tactilet;
	switch (config->getFeedbackMethod())
	{
	case FeedbackMethod::Acoustic:
		audiot = audio->_doToneFeedback(xPos);
		audiot.join();
		break;
	case FeedbackMethod::Tactile:
		tactilet = tactile->_doFeedback(getFeedbackDirection(xPos), FeedbackType::OBSTACLE);
		tactilet.join();
		break;
	case FeedbackMethod::Both:
		tactilet = tactile->_doFeedback(getFeedbackDirection(xPos), FeedbackType::OBSTACLE);
		tactilet.detach();
		audiot = audio->_doToneFeedback(xPos);
		audiot.join();
		break;
	}
	return getDirectionalString(xPos);
	/*if(isVoice){
		voiceObjectClassification();
	}
	else{
		audio->doToneFeedback(xPos);
	}*/
}

void FeedbackLayer::voiceObjectClassification()
{
	if (!checkRecentlyDetectedObj())
	{
		callVoiceFeedbackText("Nenhum.");
		return;
	}
	std::vector<Prediction> preds = cclass->Classify(object->rgbRegion, 3);
	if (preds.size() > 0)
	{
		Prediction pred = preds[0];
		float xPos = getLRAudioPosFromObj(object);
		string classe;
		if (pred.second * 100 >= 90)
		{
			// voice->doVoiceFeedback(pred.first.c_str(),cutil.randomString(5),xPos);
			callVoiceFeedbackText(pred.first.c_str());
			classe = pred.first;
		}
		else if (pred.second * 100 >= 60 && preds[1].second * 100 < 30)
		{
			const char *t1 = "Parece ser: ";
			const char *t2 = pred.first.c_str();
			char *texto = (char *)malloc(sizeof(strlen(t1)) + sizeof(strlen(t2)));
			strcpy(texto, t1);
			strcat(texto, t2);
			callVoiceFeedbackText(texto);
			// voice->doVoiceFeedback(texto,cutil.randomString(5),xPos);
			// voice->doVoiceFeedback(texto,cutil.randomString(5),xPos);
			classe = texto;
			BOOST_LOG_TRIVIAL(info) << "Confidence: " << pred.second * 100 << "%";
		}
		else
		{
			callVoiceFeedbackText("Desconhecido.");
			classe = "desconhecido";
		}
		if ((gvars.takeSnapshot || gvars.collectData) && gvars.snaps.snapshots != NULL)
		{
			cv::Mat imgRGB = object->rgbImage;
			rectangle(imgRGB, object->bounds, Scalar(0, 0, 255), 1, 8);
			cv::putText(imgRGB, classe, object->centroid, FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 2);
			gvars.snaps.snapshots->push_front(pair<string, cv::Mat>("classifiedObstacle", imgRGB));
		}
	}
}

void FeedbackLayer::callVoiceFeedbackText(const char *text)
{
	std::thread voicet = voice->_doVoiceFeedback(text);
	voicet.detach();
}

void FeedbackLayer::callTactileFeedback(Direction dir, int t, int d)
{
	std::thread tactilet = tactile->_doCustomFeedback(dir, t, d);
	tactilet.join();
}

void FeedbackLayer::callTactileFeedback(Direction dir, VibrationType vt)
{
	std::thread tactilet = tactile->_doCustomTypeFeedback(dir, vt);
	tactilet.join();
}

void FeedbackLayer::callAutoFeedback(FeedbackMethod fm, int n, ...)
{
	va_list args;
	va_start(args, n);
	switch (fm)
	{
	case FeedbackMethod::Acoustic:
		callVoiceFeedbackText(va_arg(args, const char *));
		break;
	case FeedbackMethod::Tactile:
		va_arg(args, const char *);
		if (n > 3)
		{
			callTactileFeedback(static_cast<Direction>(va_arg(args, int)), va_arg(args, int), va_arg(args, int));
		}
		else
		{
			callTactileFeedback(static_cast<Direction>(va_arg(args, int)), static_cast<VibrationType>(va_arg(args, int)));
		}
		break;
	case FeedbackMethod::Both:
		callVoiceFeedbackText(va_arg(args, const char *));
		if (n > 3)
		{
			callTactileFeedback(static_cast<Direction>(va_arg(args, int)), va_arg(args, int), va_arg(args, int));
		}
		else
		{
			callTactileFeedback(static_cast<Direction>(va_arg(args, int)), static_cast<VibrationType>(va_arg(args, int)));
		}
		break;
	}
}

void FeedbackLayer::callAutoFeedback(const char *text, Direction dir, VibrationType vt)
{

	switch (config->getFeedbackMethod())
	{
	case FeedbackMethod::Acoustic:
		callVoiceFeedbackText(text);
		break;
	case FeedbackMethod::Tactile:
		callTactileFeedback(dir, vt);
		break;
	case FeedbackMethod::Both:
		callVoiceFeedbackText(text);
		callTactileFeedback(dir, vt);
		break;
	}
}

void FeedbackLayer::callAutoFeedback(const char *text, Direction dir, int t, int d)
{

	switch (config->getFeedbackMethod())
	{
	case FeedbackMethod::Acoustic:
		callVoiceFeedbackText(text);
		break;
	case FeedbackMethod::Tactile:
		callTactileFeedback(dir, t, d);
		break;
	case FeedbackMethod::Both:
		callVoiceFeedbackText(text);
		callTactileFeedback(dir, t, d);
		break;
	}
}

const char *FeedbackLayer::doPathNavigationFeedback(pair<int, int> path, int imgwidth)
{
	float xPos = getLRAudioPosFromXCoord(path.first, imgwidth);
	lastFoundPath = xPos;
	std::thread voicet;
	std::thread audiot;
	std::thread tactilet;
	// std::thread tactilet = tactile->_doFeedback(getFeedbackDirection(xPos),FeedbackType::SAFEPATH);
	switch (config->getFeedbackMethod())
	{
	case FeedbackMethod::Acoustic:
		if (config->isPositionalFeedback())
		{
			voicet = voice->_doVoiceFeedback(getDirectionalString(xPos), cutil.randomString(5));
			voicet.join();
		}
		else
		{
			audiot = audio->_doPathFeedback(xPos);
			audiot.detach();
		}
		break;
	case FeedbackMethod::Tactile:
		tactilet = tactile->_doFeedback(getFeedbackDirection(xPos), FeedbackType::SAFEPATH);
		tactilet.join();
		break;
	case FeedbackMethod::Both:
		tactilet = tactile->_doFeedback(getFeedbackDirection(xPos), FeedbackType::SAFEPATH);
		tactilet.detach();
		if (config->isPositionalFeedback())
		{
			voicet = voice->_doVoiceFeedback(getDirectionalString(xPos), cutil.randomString(5));
			voicet.join();
		}
		else
		{
			audiot = audio->_doPathFeedback(xPos);
			audiot.join();
		}
		break;
	}
	return getDirectionalString(xPos);
}

list<string> FeedbackLayer::doPathNavigationFeedback(list<pair<int, int>> caminhos, int IMGWIDTH)
{

	list<string> dirs;
	int ssize = 0;
	const char *t1 = "Caminhos: ";
	char *text;
	for (pair<int, int> c : caminhos)
		ssize += strlen(getDirectionalString(getLRAudioPosFromXCoord(c.first, IMGWIDTH))) + 1;

	// text = (char*) malloc(sizeof(strlen(t1))+sizeof(ssize));
	text = (char *)malloc(strlen(t1) + ssize);
	strcpy(text, t1);

	for (pair<int, int> c : caminhos)
	{
		dirs.push_back(getDirectionalString(getLRAudioPosFromXCoord(c.first, IMGWIDTH)));
		strcat(text, getDirectionalString(getLRAudioPosFromXCoord(c.first, IMGWIDTH)));
		strcat(text, ",");
	}
	// voice->doVoiceFeedback(text,cutil.randomString(5));
	std::thread audiot;
	std::thread tactilet;
	std::thread voicet; /* = voice->_doVoiceFeedback(text,cutil.randomString(5));
							 voicet.join();*/

	switch (config->getFeedbackMethod())
	{
	case FeedbackMethod::Acoustic:
		voicet = voice->_doVoiceFeedback(text, cutil.randomString(5));
		voicet.join();
		break;
	case FeedbackMethod::Tactile:
		for (pair<int, int> c : caminhos)
		{
			tactilet = tactile->_doFeedback(getFeedbackDirection(getLRAudioPosFromXCoord(c.first, IMGWIDTH)), FeedbackType::SAFEPATH);
			tactilet.join();
		}
		break;
	case FeedbackMethod::Both:
		voicet = voice->_doVoiceFeedback(text, cutil.randomString(5));
		voicet.join();
		for (pair<int, int> c : caminhos)
		{
			tactilet = tactile->_doFeedback(getFeedbackDirection(getLRAudioPosFromXCoord(c.first, IMGWIDTH)), FeedbackType::SAFEPATH);
			tactilet.join();
		}
		break;
	}
	return dirs;
}

float FeedbackLayer::getLRAudioPosFromObj(DetectedObject *detectedObject)
{
	float xPos = detectedObject->centroid.x;
	return ((xPos / detectedObject->rgbImage.cols) * 2 - 1);
}

float FeedbackLayer::getLRAudioPosFromXCoord(int x, int width)
{
	return (((float)x / (float)width) * 2 - 1);
}

char *FeedbackLayer::getDirectionalString(float pos)
{
	if (abs(pos) <= 0.2)
		return (char *)"frente";

	// std::cout << pos << std::endl;
	const char *tdir = (pos < 0) ? "esquerda" : "direita";
	const char *tcdir = (abs(pos) <= 0.6 && !gvars.reducedMotors) ? "pouco a " : "";
	char *texto = (char *)malloc(sizeof(strlen(tdir)) + sizeof(strlen(tcdir)) + 1);
	strcpy(texto, tcdir);
	strcat(texto, tdir);
	return texto;
}

void FeedbackLayer::doPathNotFoundFeedback()
{
	lastFoundPath = -2;
	config->reloadConfig();
	callAutoFeedback("Nenhum caminho.", Direction::back, 2, 400000);

	/*std::thread audiot;
	std::thread tactilet;
	std::thread voicet; // = voice->_doVoiceFeedback(text,cutil.randomString(5));
						//	voicet.join();

	switch(config->getFeedbackMethod()){
	case FeedbackMethod::Acoustic:
		if(config->isPositionalFeedback()){
			voicet = voice->_doVoiceFeedback("Nenhum caminho.",cutil.randomString(5));
			voicet.detach();
		}
		else{
			audiot = audio->_doPathFeedback(-2);
			audiot.detach();
		}
		break;
	case FeedbackMethod::Tactile:
		tactilet = tactile->_doCustomFeedback(Direction::back,2,400000);
		tactilet.join();
		break;
	case FeedbackMethod::Both:
		if(config->isPositionalFeedback()){
			voicet = voice->_doVoiceFeedback("Nenhum caminho.",cutil.randomString(5));
			voicet.detach();
		}
		else{
			audiot = audio->_doPathFeedback(-2);
			audiot.detach();
		}
		tactilet = tactile->_doCustomFeedback(Direction::all,2,400000);
		tactilet.join();
		break;
	}*/
}

void FeedbackLayer::repeatLastFoundPath()
{
	config->reloadConfig();
	if (!config->isPathNavigation())
		return;

	if (lastFoundPath >= -1 && lastFoundPath <= 1)
		if (config->isPositionalFeedback())
			voice->doVoiceFeedback(getDirectionalString(lastFoundPath), cutil.randomString(5));
		else
		{
			std::thread audiot = audio->_doPathFeedback(lastFoundPath);
			audiot.detach();
		}
	else if (lastFoundPath == -2)
		doPathNotFoundFeedback();
}

void FeedbackLayer::feedbackObstacleDistance()
{
	// Caso não tenha sido detectado nenhum objeto ou o último objeto tenha sido detectado há mais de 10 segundos...
	if (!checkRecentlyDetectedObj())
	{
		std::thread voicet = voice->_doVoiceFeedback("Nenhum detectado");
		voicet.detach();
	}
	else
	{
		config->reloadConfig();
		int dist = object->centroidDistance;
		object->detectionTime = time(NULL); // renova o tempo de detecção;
		float dmeters = (float)dist / (float)1000;
		string sval;
		char *texto;
		const char *word;
		if (config->isDistanceTypeMeters())
		{
			string sval = float2string(dmeters);
			sval.replace(sval.find("."), 1, ",");
			const char *val = sval.c_str();

			if (dmeters < 1)
				word = " centímetros";
			else
				word = " metros";
			texto = (char *)malloc(strlen(val) + strlen(word));
			strcpy(texto, val);
			strcat(texto, word);
			std::thread voicet = voice->_doVoiceFeedback(texto, cutil.randomString(5));
			voicet.detach();
		}
		else
		{
			float fvalue = config->getMPacesVal();
			float dpaces = dmeters / fvalue;
			double frac, inte;
			frac = modf(dpaces, &inte);
			// string sval = float2string(inte);
			string sval = to_string((int)inte);
			// sval.replace(sval.find("."),1,",");
			const char *val = sval.c_str();
			if (frac < 0.5)
			{
				if (((int)inte) <= 1)
					word = " passo";
				else
					word = " passos";
			}
			else if (((int)inte) <= 1)
				word = " passo e meio";
			else
				word = " passos e meio";
			texto = (char *)malloc(sizeof(strlen(val)) + sizeof(strlen(word)) + 1);
			strcpy(texto, val);
			strcat(texto, word);
			// voice->doVoiceFeedback(texto,cutil.randomString(5));
			std::thread voicet = voice->_doVoiceFeedback(texto, cutil.randomString(5));
			voicet.detach();
		}
	}
}

void FeedbackLayer::helpCommands(vector<string> commands)
{
	voice->updateLanguage("en");
	char *text;

	for (string c : commands)
	{
		text = (char *)malloc(c.length() + 2);
		strcpy(text, c.c_str());
		strcat(text, "\n");
		vthread = voice->_doVoiceFeedback(text, cutil.randomString(5), sstate);
		vthread.detach();
	}
}

void FeedbackLayer::stopHelp()
{
}

string FeedbackLayer::float2string(float val)
{
	std::ostringstream oss;

	oss << std::fixed << std::setfill('0') << std::setprecision(2) << val;

	return oss.str();
}

char *FeedbackLayer::getDistanceString(float dmeters)
{
	string sval;
	char *texto;
	const char *word;
	if (config->isDistanceTypeMeters())
	{
		string sval = float2string(dmeters);
		sval.replace(sval.find("."), 1, ",");
		const char *val = sval.c_str();

		if (dmeters < 1)
			word = " centímetros";
		else
			word = " metros";
		texto = (char *)malloc(sizeof(strlen(val)) + sizeof(strlen(word)) + 1);
		strcpy(texto, val);
		strcat(texto, word);
		// voice->doVoiceFeedback(texto,cutil.randomString(5));
	}
	else
	{
		float fvalue = config->getMPacesVal();
		float dpaces = dmeters / fvalue;
		double frac, inte;
		frac = modf(dpaces, &inte);
		// string sval = float2string(inte);
		string sval = to_string((int)inte);
		// sval.replace(sval.find("."),1,",");
		const char *val = sval.c_str();
		if (frac < 0.5)
		{
			if (((int)inte) <= 1)
				word = " passo";
			else
				word = " passos";
		}
		else if (((int)inte) <= 1)
			word = " passo e meio";
		else
			word = " passos e meio";
		texto = (char *)malloc(sizeof(strlen(val)) + sizeof(strlen(word)) + 1);
		strcpy(texto, val);
		strcat(texto, word);
		// voice->doVoiceFeedback(texto,cutil.randomString(5));
	}
	return texto;
}

const char *FeedbackLayer::getMeasurementString(float value, bool meters)
{
	string sval;
	// char* texto;
	// char* word;
	char *buff = (char *)malloc(25 * sizeof(char));
	if (meters)
	{
		if (value > 2.5)
			value /= 100;

		int m = truncf(value);
		int dec = fmodf(value, m) * 100;

		if (value < 1)
			sprintf(buff, "%d centímetros.", dec);
		else
			sprintf(buff, "%d %s e %d", m, (m < 2) ? "metro" : "metros", dec);
	}
	else
	{
		if (value < 2.5)
			value *= 100;

		sprintf(buff, "%d centímetros.", (int)value);
	}
	return buff;
}

Direction FeedbackLayer::getFeedbackDirection(float pos)
{
	if (abs(pos) <= 0.2)
		return Direction::front;

	if (abs(pos) <= 0.6 && !gvars.reducedMotors)
		if (pos < 0)
			return Direction::s_left;
		else
			return Direction::s_right;
	else if (pos < 0)
		return Direction::left;
	else
		return Direction::right;

	return Direction::none;
}

bool FeedbackLayer::checkRecentlyDetectedObj()
{
	return !(lobject == NULL || (lobject != NULL && abs(difftime(lobject->detectionTime, time(NULL))) > 10));
}
