/*

 * __afsvipGLOBALS.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */
#include "__afsvipGLOBALS.h"

struct globalvars gvars;
std::mutex configmutex;
bool soundInUse = false;
// bool SYSDEBUGMODE = true;
// bool stopSystemFeedback = false;
// bool stopVoiceListener = false;
// bool stopKeyListener = false;
// bool isFreenectDriver = false;
// bool takeSnapshot = false;
// bool setup = false;
TPointVar timeTic, timeToc;
const string SYSLOGDATETIME(getYMDTime());

char *randomString(int len)
{
	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char *out = (char *)malloc(len + 1);

	srand(time(NULL));

	for (int i = 0; i <= len; ++i)
	{
		out[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return out;
}

int randomInt()
{
	srand(time(NULL));
	return rand() % 1000;
}

std::string trim(std::string allocator)
{
	return boost::algorithm::trim_copy(allocator);
}

std::vector<std::string> splitString(std::string allocator, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(allocator);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

TPointVar getCurrTimePoint()
{
	return std::chrono::high_resolution_clock::now();
}

double compareTPoint(TPointVar t1, TPointVar t2)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
}

void tic()
{
	timeTic = getCurrTimePoint();
}

TPointVar tic1()
{
	return getCurrTimePoint();
}

void toc(std::string msg)
{
	std::cout << msg << compareTPoint(timeTic, getCurrTimePoint()) << "ms." << std::endl;
}

double toc1()
{
	return compareTPoint(timeTic, getCurrTimePoint());
}

double tocX(TPointVar t1)
{
	return compareTPoint(t1, getCurrTimePoint());
}

char *getYMDTime()
{
	time_t now;
	time(&now);
	char *tbuff = (char *)malloc(15);
	strftime(tbuff, 15, "%Y%m%d%H%M%S", localtime(&now));
	return tbuff;
}

void init_logging()
{

	logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

	logging::add_console_log(std::cout, boost::log::keywords::format = "\r[%TimeStamp%] [%ThreadID%] [%Severity%] [%ProcessID%] [%LineID%] %Message% \r");

	/*logging::add_file_log(
		keywords::file_name = "logs/"+SYSLOGDATETIME+"/"+SYSLOGDATETIME+".log",
		keywords::auto_flush = true,
		keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] [%ProcessID%] [%LineID%] %Message%"
	);*/

	logging::core::get()->set_filter(
		logging::trivial::severity >= logging::trivial::info);

	/*logging::core::get()->add_global_attribute("MyAttr", attrs::constant<int>(42));
	logging::core::get()->add_global_attribute("CountDown", attrs::counter<int>(100, -1));*/

	logging::add_common_attributes();
}

double degree2rad(int d)
{
	return (d * _PI) / 180;
}

int rad2degree(double r)
{
	return round((r * 180) / _PI);
}

int calcRecCameraAngle(int cang, int cvfov, int alt, int mind)
{
	alt -= 30;
	double hip = sqrt(pow(alt, 2) + pow(mind, 2));
	double nang = asin(mind / hip);
	nang = 90 - (rad2degree(nang) + cvfov / 2);
	cang -= 90;
	// std::cout << "Altura: " << alt << " | Dist: " << mind << " | Hip: " << hip << " | Cam: " << cang << " | Rec: " << nang << std::endl;
	return cang - nang;
}
