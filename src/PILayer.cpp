/*
 * PILayer.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "PILayer.h"
#include <exception>

static void
log_handler(struct libinput *li,
			enum libinput_log_priority priority,
			const char *format,
			va_list args)
{
	vprintf(format, args);
}

static struct libinput *open_udev(const struct libinput_interface *interface,
								  void *userdata,
								  const char *seat,
								  int verbose)
{
	struct libinput *li;
	struct udev *udev = udev_new();

	if (!udev)
	{
		fprintf(stderr, "Failed to initialize udev\n");
		return NULL;
	}

	li = libinput_udev_create_context(interface, userdata, udev);
	if (!li)
	{
		fprintf(stderr, "Failed to initialize context from udev\n");
		goto out;
	}

	if (verbose)
	{
		libinput_log_set_handler(li, log_handler);
		libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_DEBUG);
	}

	if (libinput_udev_assign_seat(li, seat))
	{
		fprintf(stderr, "Failed to set seat\n");
		libinput_unref(li);
		li = NULL;
		goto out;
	}

out:
	udev_unref(udev);
	return li;
}

static struct libinput *
open_device(const struct libinput_interface *interface,
			void *userdata,
			const char *path,
			int verbose)
{
	struct libinput_device *device;
	struct libinput *li;

	li = libinput_path_create_context(interface, userdata);
	if (!li)
	{
		fprintf(stderr, "Failed to initialize context from %s\n", path);
		return NULL;
	}

	if (verbose)
	{
		libinput_log_set_handler(li, log_handler);
		libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_DEBUG);
	}

	device = libinput_path_add_device(li, path);
	if (!device)
	{
		fprintf(stderr, "Failed to initialized device %s\n", path);
		libinput_unref(li);
		li = NULL;
	}

	return li;
}

static volatile sig_atomic_t stop = 0;

static void
sighandler(int signal, siginfo_t *siginfo, void *userdata)
{
	stop = 1;
}

static int open_restricted(const char *path, int flags, void *user_data)
{
	int fd = open(path, flags);
	return fd < 0 ? -errno : fd;
}
static void close_restricted(int fd, void *user_data)
{
	close(fd);
}
const static struct libinput_interface interface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
};

static struct libinput *openDevice(const char *path, bool pathBackend = true)
{
	struct libinput_device *device;
	struct libinput *li;
	struct udev *udev = udev_new();

	/*li = libinput_path_create_context(&interface, NULL);
	if (!li) {
		fprintf(stderr, "Failed to initialize context from %s\n", path);
		return NULL;
	}*/

	if (pathBackend)
	{
		device = libinput_path_add_device(li, path);
		if (!device)
		{
			fprintf(stderr, "Failed to initialized device %s\n", path);
			libinput_unref(li);
			li = NULL;
		}
	}
	else
	{
		if (!udev)
		{
			fprintf(stderr, "Failed to initialize udev\n");
			return NULL;
		}

		li = libinput_udev_create_context(&interface, NULL, udev);
		if (!li)
		{
			fprintf(stderr, "Failed to initialize context from udev\n");
			goto out;
		}

		/*libinput_log_set_handler(li, log_handler);
		libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_DEBUG);*/

		if (libinput_udev_assign_seat(li, path))
		{
			fprintf(stderr, "Failed to set seat\n");
			libinput_unref(li);
			li = NULL;
			goto out;
		}

	out:
		udev_unref(udev);
		return li;

		/*udev = udev_new();
		if(!udev){
			fprintf(stderr, "Failed to initialized device %s\n", path);
			libinput_unref(li);
			li = NULL;
		}
		li = libinput_udev_create_context(&interface, NULL, udev);
		libinput_udev_assign_seat(li, "seat0");*/
	}

	return li;
}

struct libinput *
openDevice2(bool device,
			const char *path,
			void *userdata,
			const struct libinput_interface *interface)
{
	struct libinput *li = NULL;

	if (!device)
	{
		li = open_udev(interface, userdata, path, 1);
	}
	else
	{
		li = open_udev(interface, userdata, path, 1);
	}

	return li;
}

PILayer::PILayer(Configuration *c, FeedbackLayer *f)
{
	config = c;
	feedback = f;
	setKeyboardKeys();
}

PILayer::PILayer(Configuration *c, FeedbackLayer *f, DepthCamera *cam)
{
	config = c;
	feedback = f;
	camera = cam;
	setKeyboardKeys();
}

void PILayer::setKeyboardKeys()
{
	keymap[KEY_KP1] = "K1";
	keymap[KEY_KP2] = "K2";
	keymap[KEY_KP3] = "K3";
	keymap[KEY_KP4] = "K4";
	keymap[KEY_KP5] = "K5";
	keymap[KEY_KP6] = "K6";
	keymap[KEY_KP7] = "K7";
	keymap[KEY_KP8] = "K8";
	keymap[KEY_KP9] = "K9";
	keymap[KEY_KP0] = "K0";
	// Teclado maxprint
	/*keymap[KEY_BACKSPACE]="KB";
	keymap[KEY_KPDOT]="KC";
	keymap[KEY_KPENTER]="KD";
	keymap[KEY_KPCOMMA]="KE";
	keymap[KEY_KPPLUS]="KF";
	keymap[KEY_KPMINUS]="KG";
	keymap[KEY_KPASTERISK]="KH";
	keymap[KEY_KPSLASH]="KI";*/
	// teclado knup kp2003a
	keymap[KEY_KPDOT] = "KB";
	keymap[KEY_KPENTER] = "KC";
	keymap[KEY_BACKSPACE] = "KD";
	keymap[KEY_KPPLUS] = "KE";
	keymap[KEY_KPMINUS] = "KF";
	keymap[KEY_KPASTERISK] = "KG";
	keymap[KEY_KPSLASH] = "KH";
	keymap[KEY_NUMLOCK] = "NUMLOCK";
}

PILayer::~PILayer()
{
}

std::thread PILayer::startListening()
{
	initscr();
	return std::thread([=]
					   { startKeyboardListener(); });
}

void PILayer::startKeyboardListener()
{
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sighandler;
	act.sa_flags = SA_SIGINFO;

	if (sigaction(SIGINT, &act, NULL) == -1)
	{
		fprintf(stderr, "Failed to set up signal handling (%s)\n",
				strerror(errno));
		return;
	}
	linput = openDevice("seat0", false);
	fds.fd = libinput_get_fd(linput);
	fds.events = POLLIN;
	fds.revents = 0;
	// while(!gvars.haltSystem) {
	while (!gvars.haltSystem && poll(&fds, 1, -1) > -1)
	{
		BOOST_LOG_TRIVIAL(trace) << "CHK_PIL1";
		std::system("numlockx on");
		noecho();
		cbreak();
		nodelay(stdscr, TRUE);
		scrollok(stdscr, TRUE);
		BOOST_LOG_TRIVIAL(trace) << "CHK_PIL2";
		// while(!gvars.haltSystem && poll(&fds, 1, -1) > -1){
		BOOST_LOG_TRIVIAL(trace) << "CHK_PIL3";
		libinput_dispatch(linput);
		BOOST_LOG_TRIVIAL(trace) << "CHK_PIL4";
		while ((evt = libinput_get_event(linput)))
		{
			BOOST_LOG_TRIVIAL(trace) << "CHK_PIL5";
			/*std::string resV;
				std::string resP;
				std::stringstream ssV;
				std::stringstream ssP;
				ssV << std::hex << std::setw(4) << std::setfill('0') << libinput_device_get_id_vendor(libinput_event_get_device(evt));
				ssP << std::hex << std::setw(4) << std::setfill('0') << libinput_device_get_id_product(libinput_event_get_device(evt));
				ssV >> resV;
				ssP >> resP;
				resV.append(":");
				resV.append(resP);*/
			BOOST_LOG_TRIVIAL(trace) << "CHK_PIL6";
			if (libinput_event_get_type(evt) == LIBINPUT_EVENT_KEYBOARD_KEY)
			{
				BOOST_LOG_TRIVIAL(trace) << "CHK_PIL7";
				kEvent = libinput_event_get_keyboard_event(evt);
				if (libinput_event_keyboard_get_key(kEvent) >= 0)
				{
					switch (libinput_event_keyboard_get_key_state(kEvent))
					{
					case LIBINPUT_KEY_STATE_PRESSED:
						BOOST_LOG_TRIVIAL(trace) << "CHK_PIL8";
						processKeyDown(kEvent);
						BOOST_LOG_TRIVIAL(trace) << "CHK_PIL8.1";
						break;
					case LIBINPUT_KEY_STATE_RELEASED:
						BOOST_LOG_TRIVIAL(trace) << "CHK_PIL9";
						processKeyUp(kEvent);
						BOOST_LOG_TRIVIAL(trace) << "CHK_PIL9.1";
						break;
					}
				}
			}
			//}
		}
		BOOST_LOG_TRIVIAL(trace) << "CHK_PIL10\r";
		if (gvars.systemBoot && gvars.systemState == sysState::OFF)
			break;
		BOOST_LOG_TRIVIAL(trace) << "CHK_PIL11\r";
		usleep(300);
	}
	if (!gvars.systemBoot)
	{
		std::cin.ignore();
		endwin();
	}
	// libinput_unref(linput);
}

void PILayer::processKeyDown(libinput_event_keyboard *k)
{
	uint32_t key = libinput_event_keyboard_get_key(k);
	if (keyPressedIdx(kplist, key) < 0)
	{
		kplist.push_back(pair<uint32_t, double>(key, libinput_event_keyboard_get_time(k)));
	}
}

void PILayer::processKeyUp(libinput_event_keyboard *k)
{
	uint32_t key = libinput_event_keyboard_get_key(k);
	/*if(gvars.systemState == sysState::OFF && key != KEY_KPASTERISK){
		gvars.systemBoot = key == KEY_NUMLOCK;
		return;
	}*/

	int kidx = keyPressedIdx(kplist, key);
	if (kidx >= 0)
	{
		processKey(key, libinput_event_keyboard_get_time(k) - kplist[kidx].second);
		kplist.erase(kplist.begin() + kidx);
	}
}

int PILayer::keyPressedIdx(vector<pair<uint32_t, double>> v,
						   uint32_t k)
{
	for (uint i = 0; i < v.size(); i++)
	{
		if (v[i].first == k)
			return i;
	}
	return -1;
}

void PILayer::processKey(uint32_t key, double t)
{
	if (keymap.find(key) == keymap.end())
		return;
	bool longpress = t >= kpresstime;
	if (!longpress)
	{
		if (!keyedit)
		{
			processKeyFunc(config->getKeypadFunction(keymap[key]), longpress, key);
		}
		else if (editKeyCode == key)
		{
			feedback->callVoiceFeedbackText(config->changeEditableKey(keymap[key]).first.c_str());
		}
		else
		{
			feedback->callVoiceFeedbackText("Cancelado.");
			keyedit = false;
			editKeyCode = 0;
		}
	}
	else
	{
		switch (key)
		{
		case KPX:
			if (gvars.systemState == sysState::ON)
				gvars.haltSystem = true;
			return;
			break;
		case KPH: // sempre vai ter o mesmo comportamento.
		{
			FeedbackMethod currentFM = config->getFeedbackMethod();
			currentFM = currentFM == FeedbackMethod::Last ? FeedbackMethod::First : static_cast<FeedbackMethod>(static_cast<int>(currentFM) + 1);
			config->setFeedbackMethod(currentFM);
			feedback->callAutoFeedback("Ok.", Direction::all, VibrationType::ACK);
			return;
		}
		break;
		case KPE: // Diminuir gap de voz
		case KPF: // Aumentar gap de voz
		case KPB:
			processKeyFunc(config->getKeypadFunction(keymap[key]), longpress, key);
			return;
			break;
		}
		if (gvars.systemState == sysState::USERSETUP)
		{
			if (keyedit)
			{
				feedback->callVoiceFeedbackText("Salvo.");
			}
			else
			{
				keyedit = std::find(std::begin(editableKeys), std::end(editableKeys), key) != std::end(editableKeys);
				if (keyedit)
				{
					feedback->callVoiceFeedbackText("Editando.");
					editKeyCode = key;
				}
			}
		}
	}
}

void PILayer::processKeyFunc(int func, bool lpress, uint32_t kps)
{

	if (!lpress && activesetup == UserSetup::USERHEIGHT)
		switch (kps)
		{
		case KP0:
		case KP1:
		case KP2:
		case KP3:
		case KP4:
		case KP5:
		case KP6:
		case KP7:
		case KP8:
		case KP9:
			int kcNum = getNumericFromKeycode(kps);
			Direction cdirnum;
			if (tempsetup.heightdigit < 1)
			{
				tempsetup.height = kcNum * 100;
				// cdirnum=Direction::s_left;
				cdirnum = Direction::left;
			}
			else if (tempsetup.heightdigit < 2)
			{
				tempsetup.height += kcNum * 10;
				// cdirnum=Direction::s_right;
				cdirnum = Direction::front;
			}
			else if (tempsetup.heightdigit < 3)
			{
				tempsetup.height += kcNum;
				cdirnum = Direction::right;
			}
			tempsetup.heightdigit++;
			feedback->callAutoFeedback(to_string(kcNum).c_str(), cdirnum, kcNum, kcNum == 0 ? 500000 : 200000);
			if (tempsetup.heightdigit == 3)
			{
				config->setUserHeight(tempsetup.height);
				feedback->callAutoFeedback(FeedbackLayer::getMeasurementString(tempsetup.height, true), Direction::all, VibrationType::SAVED);
				activesetup = UserSetup::NOTCHOSEN;
			}
			return;
			break;
		}

	switch (gvars.systemState)
	{
	case sysState::OFF:
		switch (kps)
		{
		case KPX:
			gvars.systemBoot = true;
			break;
		case KPG:
		{
			feedback->callAutoFeedback("Modo de customização ligado.", Direction::all, VibrationType::OPT_IN);
			gvars.systemState = sysState::USERSETUP;
			gvars.userSetupTic = tic1();
			keyedit = false;
			editKeyCode = 0;
		}
		break;
		}
		break;
	case sysState::USERSETUP:
		switch (kps)
		{
		case KPG:
		{
			feedback->callAutoFeedback("Modo de customização desligado.", Direction::all, VibrationType::OPT_OUT);
			gvars.systemState = sysState::OFF;
			keyedit = false;
			editKeyCode = 0;
		}
		break;
		case KP8:
		{
			bool cok = false;
			if (!camera->isIMUAvailable())
				return;
			TPointVar t1 = tic1();
			while (!cok || tocX(t1) <= 3000)
			{
				gvars.userSetupTic = tic1();
				int dif = calcRecCameraAngle(camera->getVerticalInclination(), camera->getVerticalFOV(), config->getUserHeight(), 150);
				if (dif < -1 && tocX(t1) >= 3000)
				{
					feedback->callAutoFeedback("Baixo.", Direction::right, VibrationType::CAM_LOWER);
					t1 = tic1();
					cok = false;
				}
				else if (dif > 1 && tocX(t1) >= 3000)
				{
					feedback->callAutoFeedback("Cima.", Direction::left, VibrationType::CAM_RAISE);
					t1 = tic1();
					cok = false;
				}
				else if (!cok)
				{
					cok = true;
					t1 = tic1();
				}
			}
			gvars.userSetupTic = tic1();
			feedback->callAutoFeedback("Ok.", Direction::front, VibrationType::SAVED);
		}
		break;
		case KP7:
		{
			if (lpress)
			{
				gvars.userSetupTic = tic1();
				if (activesetup == UserSetup::USERHEIGHT)
				{
					activesetup = UserSetup::NOTCHOSEN;
					if (tempsetup.height != config->getUserHeight())
					{
						// config->setUserHeight(tempsetup.height);
						if (tempsetup.heightdigit < 3)
							feedback->callAutoFeedback("Altura mantida.", Direction::all, VibrationType::SAVED);
						else
						{
							config->setUserHeight(tempsetup.height);
							feedback->callAutoFeedback("Salvo.", Direction::all, VibrationType::SAVED);
						}
					}
				}
				else
				{
					activesetup = UserSetup::USERHEIGHT;
					tempsetup.height = config->getUserHeight();
					tempsetup.heightdigit = 0;
					feedback->callAutoFeedback(FeedbackLayer::getMeasurementString(tempsetup.height, true), Direction::front, VibrationType::UHEIGHT);
					// feedback->callVoiceFeedbackText(FeedbackLayer::getMeasurementString(tempsetup.height,true));
				}
			}
		}
		break;
		case KPE:
		{
			gvars.userSetupTic = tic1();
			switch (activesetup)
			{
			case UserSetup::USERHEIGHT:
				if (tempsetup.heightdigit > 0)
				{
					tempsetup.height = config->getUserHeight();
					tempsetup.heightdigit = 0;
				}
				if (lpress)
					tempsetup.height += 10;
				else
					tempsetup.height++;
				feedback->callVoiceFeedbackText(FeedbackLayer::getMeasurementString(tempsetup.height, true));
				break;
			}
		}
		break;
		case KPF:
		{
			gvars.userSetupTic = tic1();
			switch (activesetup)
			{
			case UserSetup::USERHEIGHT:
				if (tempsetup.heightdigit > 0)
				{
					tempsetup.height = config->getUserHeight();
					tempsetup.heightdigit = 0;
				}
				if (lpress)
					tempsetup.height -= 10;
				else
					tempsetup.height--;
				feedback->callVoiceFeedbackText(FeedbackLayer::getMeasurementString(tempsetup.height, true));
				break;
			}
		}
		break;
		}
		break;
	case sysState::ON:
	{
		switch (func)
		{
		case KBCommands::SYSTEM_CLASSIFY:
			feedback->voiceObjectClassification();
			break;
		case KBCommands::SYSTEM_SETUP_ENABLE:
			if (!gvars.setup)
				feedback->callVoiceFeedbackText("Modo de customização habilitado.");
			else
				feedback->callVoiceFeedbackText("Modo de customização desabilitado.");
			gvars.setup = !gvars.setup;
			break;
		case KBCommands::SYSTEM_OBSTACLE_DISTANCE:
			feedback->feedbackObstacleDistance();
			break;
		case KBCommands::SYSTEM_SHOW_PATH:
			feedback->repeatLastFoundPath();
			break;
		case KBCommands::SYSTEM_FEEDBACK_STOP:
			gvars.stopSystemFeedback = !gvars.stopSystemFeedback;
			if (gvars.stopSystemFeedback)
				feedback->callVoiceFeedbackText("Resposta do Sistema em Espera.");
			else
				feedback->callVoiceFeedbackText("Retomando Resposta do Sistema.");
			break;
		case KBCommands::SYSTEM_VOICELISTENER_TOGGLE:
			if (!lpress)
			{
				gvars.stopVoiceListener = !gvars.stopVoiceListener;
				if (gvars.stopVoiceListener)
					feedback->callVoiceFeedbackText("Sem Voz.");
				else
					feedback->callVoiceFeedbackText("Voz.");
			}
			else
			{
				FeedbackMethod currentFM = config->getFeedbackMethod();
				currentFM = currentFM == FeedbackMethod::Last ? FeedbackMethod::First : static_cast<FeedbackMethod>(static_cast<int>(currentFM) + 1);
				config->setFeedbackMethod(currentFM);
			}
			break;
		case KBCommands::SYSTEM_TURN_OFF:
			if (lpress)
				gvars.haltSystem = true;
			break;
		case KBCommands::SYSTEM_HELP_COMMANDS: // TODO
			// feedback->helpCommands(commands);
			break;
		case KBCommands::SYSTEM_HELP_STOP: // TODO
			// feedback->stopHelp();
			break;
		case KBCommands::DEBUG_SNAPSHOT:
			if (gvars.SYSDEBUGMODE)
				gvars.takeSnapshot = true;
			break;
		case KBCommands::SYSTEM_DISTANCE_INCREASE:
			config->increaseMaximumDistance();
			break;
		case KBCommands::SYSTEM_DISTANCE_DECREASE:
			config->decreaseMaximumDistance();
			break;
		case KBCommands::SYSTEM_FEEDBACK_TONE:
			config->setAcousticFeedbackType(FeedbackAcousticType::Tone);
			break;
		case KBCommands::SYSTEM_FEEDBACK_VOICE:
			config->setAcousticFeedbackType(FeedbackAcousticType::Voice);
			break;
		case KBCommands::SYSTEM_VOLUME_HIGHER:
			if (!lpress)
				config->increaseSystemVolume();
			else
				config->setSystemHighVol();
			// feedback->callVoiceFeedbackText("Ok.");
			break;
		case KBCommands::SYSTEM_VOLUME_LOWER:
			if (!lpress)
				config->decreaseSystemVolume();
			else
				config->setSystemMinVol();
			// feedback->callVoiceFeedbackText("Ok.");
			break;
		case KBCommands::SYSTEM_VOLUME_MINIMUM:
			config->setSystemMinVol();
			// feedback->callVoiceFeedbackText("Ok.");
			break;
		case KBCommands::SYSTEM_VOLUME_LOUD:
			config->setSystemHighVol();
			// feedback->callVoiceFeedbackText("Ok.");
			break;
		case KBCommands::SYSTEM_VOLUME_DEFAULT:
			config->setSystemDefVol();
			// feedback->callVoiceFeedbackText("Ok.");
			break;
		case KBCommands::SYSTEM_NAVIGATION_TYPE_PATH:
		{
			NavigationType navType = config->getNavigationType();
			navType = navType == NavigationType::LastNT ? NavigationType::FirstNT : static_cast<NavigationType>(static_cast<int>(navType) + 1);
			config->setNavigationType(navType);
			/*if(config->isPathNavigation())
				config->setNavigationType("obstacle");
			else
				config->setNavigationType("path");*/
		}
		break;
		case KBCommands::SYSTEM_NAVIGATION_DISTANCE_MINIMUM:
			config->setNavigationDistance(70);
			break;
		case KBCommands::SYSTEM_NAVIGATION_DISTANCE_DEFAULT:
			config->setNavigationDistance(100);
			break;
		case KBCommands::SYSTEM_NAVIGATION_DISTANCE_MAXIMUM:
			config->setNavigationDistance(130);
			break;
		case KBCommands::SYSTEM_FEEDBACK_TYPE_POSITIONAL:
			if (config->isPositionalFeedback())
				config->setPosStereoFeedback(FeedbackAcousticDirection::Positional);
			else
				config->setPosStereoFeedback(FeedbackAcousticDirection::Stereo);
			break;
		case KBCommands::SYSTEM_DISTANCE_TYPE_METERS:
			if (config->isDistanceTypeMeters())
			{
				config->setDistanceType(DistanceType::Paces);
				feedback->callVoiceFeedbackText("Passos.");
			}
			else
			{
				config->setDistanceType(DistanceType::Meters);
				feedback->callVoiceFeedbackText("Metros.");
			}
			break;
		case KBCommands::SYSTEM_FEEDBACK_CONTINUOUS:
			if (config->isContinuousFeedback())
			{
				config->setSceneContFeedback(FeedbackSceneMode::Passive);
				feedback->callVoiceFeedbackText("Passivo.");
			}
			else
			{
				config->setSceneContFeedback(FeedbackSceneMode::Active);
				feedback->callVoiceFeedbackText("Ativo.");
			}
			break;
		case KBCommands::SYSTEM_FEEDBACK_SPEED_INCREASE:
			if (config->getFeedbackSpeed() > 1)
				config->updateFeedbackSpeed(config->getFeedbackSpeed() - 1); // diminui um pois é o multiplicador do sleep na camada de visão, ou seja, o sleep é menor.
			// feedback->callVoiceFeedbackText(to_string(config->getFeedbackSpeed()).c_str());
			break;
		case KBCommands::SYSTEM_FEEDBACK_SPEED_DECREASE:
			config->updateFeedbackSpeed(config->getFeedbackSpeed() + 1);
			// feedback->callVoiceFeedbackText(to_string(config->getFeedbackSpeed()).c_str());
			break;
		case KBCommands::SYSTEM_INCREASE_MAXIMUM_DISTANCE:
			config->increaseMaximumDistance();
			feedback->callVoiceFeedbackText(feedback->getDistanceString(config->getMaximumDistance()));
			break;
		case KBCommands::SYSTEM_DECREASE_MAXIMUM_DISTANCE:
			config->decreaseMaximumDistance();
			feedback->callVoiceFeedbackText(feedback->getDistanceString(config->getMaximumDistance()));
			break;
		case KBCommands::SYSTEM_INCREASE_MINIMUM_DISTANCE:
			config->increaseMinimumDistance();
			feedback->callVoiceFeedbackText(feedback->getDistanceString(config->getMinimumDistance()));
			break;
		case KBCommands::SYSTEM_DECREASE_MINIMUM_DISTANCE:
			config->decreaseMinimumDistance();
			feedback->callVoiceFeedbackText(feedback->getDistanceString(config->getMinimumDistance()));
			break;
		case KBCommands::SYSTEM_VOICE_SPEED_INCREASE: // VOICE GAP SE PRESSIONADO
			if (lpress)
			{
				int vgap = cv::min(config->getVoiceGap() + 1, 1000); // Limite de 1s de gap
				config->setVoiceGap(vgap);
				VoiceFeedback::updateVoiceGap(vgap);
			}
			else
			{
				int vspd = cv::min(config->getVoiceSpeed() + 10, 450); // Limite do eSpeak = 450
				config->setVoiceSpeed(vspd);
				VoiceFeedback::updateVoiceSpeed(vspd);
			}
			break;
		case KBCommands::SYSTEM_VOICE_SPEED_DECREASE: // VOICE GAP SE PRESSIONADO
			if (lpress)
			{
				int vgp = cv::max(config->getVoiceGap() - 1, 0); // Limite de 0 ms de gap
				config->setVoiceGap(vgp);
				VoiceFeedback::updateVoiceGap(vgp);
			}
			else
			{
				int vsp = cv::max(config->getVoiceSpeed() - 10, 80); // Limite do eSpeak = 80
				config->setVoiceSpeed(vsp);
				VoiceFeedback::updateVoiceSpeed(vsp);
			}
			break;
		}
		break;
	}
	}
}

int PILayer::getNumericFromKeycode(uint32_t k)
{
	auto itr = std::find(kpNumericKeys, kpNumericKeys + sizeof(kpNumericKeys) / sizeof(kpNumericKeys[0]), k);
	if (itr == std::end(kpNumericKeys))
		return -1;
	return std::distance(kpNumericKeys, itr);
}
