#include "DepthCameraInitializer.h"
#include "CVLayer.h"
#include "VILayer.h"
#include "TactileFeedback.h"
#include "AudioFeedback.h"
#include "VoiceFeedback.h"
#include "RealSense435i.h"
#include "PILayer.h"
#include "CodeUtil.h"
#include <pthread.h>
#include <thread>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/program_options.hpp>
#include <iterator>
#include <map>
#include <future>
#include "ConfigHandler.h"
#include "Configuration.h"

namespace po = boost::program_options;

bool parseArgs(int, char *[]);

int main(int ac, char *av[])
{

	if (!parseArgs(ac, av))
		return 0;

	init_logging();

	ConfigHandler cfgHndlr;
	Configuration config(&cfgHndlr);
	TactileFeedback tactile(&config);
	AudioFeedback audio(&config);
	VoiceFeedback voice(audio, &config);
	// FeedbackLayer* feedback = new FeedbackLayer(&audio,&config,&voice);
	FeedbackLayer feedback(&audio, &config, &voice, &tactile);

	DepthCameraInitializer dcam;
	PILayer *pi = new PILayer(&config, &feedback, dcam.getDepthCamera());
	std::thread startUpRoutine = pi->startListening();

	VILayer *voiceintlayer = new VILayer(&feedback, &config);

	CodeUtil cutil;

	//**********************************
	//  SYSTEM LOADER
	//**********************************
	std::thread listenThread = voiceintlayer->startListening();
	listenThread.detach();
	gvars.stopVoiceListener = true;
	feedback.callAutoFeedback("Sistema pronto. X para começar.", Direction::all, VibrationType::ACK);
	tic();
	while (!gvars.systemBoot)
	{
		if (startUpRoutine.joinable())
		{
			std::cout << "executando thread" << std::endl;
			startUpRoutine.detach();
		}
		if (gvars.systemState == sysState::USERSETUP)
		{
			if (tocX(gvars.userSetupTic) >= 30000)
			{
				feedback.callVoiceFeedbackText("Pressione G para sair do modo de customização.");
				gvars.userSetupTic = tic1();
			}
			continue;
		}

		if (toc1() >= 30000)
		{
			feedback.callAutoFeedback("Sistema pronto. X para começar.", Direction::all, VibrationType::ACK);
			tic();
		}
	}
	feedback.callAutoFeedback("Aguarde.", Direction::all, VibrationType::ACK);
	gvars.systemState = sysState::ON;

	delete pi;
	PILayer *pi2 = new PILayer(&config, &feedback);
	std::thread kbThread = pi2->startListening();
	kbThread.detach();
	CVLayer cvlayer(&feedback, dcam.getDepthCamera(), &config);
	// ThreadController* tc = new ThreadController(voiceintlayer,pi);
	// std::future<void> cfut = std::async(std::launch::async,&ThreadController::createAndRun,tc);
	// std::thread tt = tc->startThreads();
	// tt.detach();
	feedback.callAutoFeedback("Iniciando o sistema.", Direction::all, VibrationType::ACK);

	std::thread videoThread = cvlayer.startVideoProcess();
	videoThread.join();

	voice.doVoiceFeedback("Finalizando o sistema.");
	// voice.doVoiceFeedback("Até logo.");
	// pi->~PILayer();

	delete pi;
	delete voiceintlayer;

	if (gvars.shutdownComputer)
		voice.doVoiceFeedback("Desligar energia em 10 segundos.");

	std::system("numlockx on; stty sane");

	if (gvars.shutdownComputer)
		std::system("shutdown -P now");
	else
		std::cin.ignore();
	return 0;
}

bool parseArgs(int ac, char *av[])
{
	try
	{
		po::options_description desc("Allowed options: ");
		desc.add_options()("help", "TODO")("noautorun", po::bool_switch(), "prevents system to run on startup")("headless", po::bool_switch(), "set to headless mode")("shutdown", po::bool_switch(), "shutdown operating system once navigation system is closed")("collectdata", po::bool_switch(), "collect and save data for further analysis");

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << "\n";
			return false;
		}

		if (vm["noautorun"].as<bool>())
			return false;

		if (vm["shutdown"].as<bool>())
			gvars.shutdownComputer = true;

		if (vm["headless"].as<bool>())
			gvars.headless = true;

		if (vm["collectdata"].as<bool>())
		{
			gvars.collectData = true;
			pair<const char *, const char *> fps = CodeUtil::createSnapshotFilePath();
			gvars.snaps.fpath = fps.first;
			gvars.snaps.tid = fps.second;
		}
	}
	catch (std::exception &e)
	{
		cerr << "error: " << e.what() << "\n";
		return false;
	}
	catch (...)
	{
		cerr << "Exception of unknown type!\n";
		return false;
	}
	return true;
}
