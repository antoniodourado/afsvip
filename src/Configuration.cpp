/*
 * Configuration.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "Configuration.h"

Configuration::Configuration(ConfigHandler *c)
{
	cfg = c;
	localConfig.detection_distance = c->getMaximumDistance();
	localConfig.feedback_tonetimes = 3;
	localConfig.feedback_pacevalue = c->getMPacesVal();
	localConfig.feedback_type = c->isVoiceFeedback() ? FeedbackAcousticType::Voice : FeedbackAcousticType::Tone;
	localConfig.feedback_contscene = c->isContinuousFeedback() ? FeedbackSceneMode::Active : FeedbackSceneMode::Passive;
	localConfig.feedback_svolume = c->getSystemVolume();
	localConfig.feedback_distancetype = c->isDistanceTypeMeters() ? DistanceType::Meters : DistanceType::Paces;
	localConfig.feedback_volume = c->getSystemAPIVolume();
	localConfig.feedback_positional = c->isPositionalFeedback() ? FeedbackAcousticDirection::Positional : FeedbackAcousticDirection::Stereo;
	localConfig.voice_speed = c->getVoiceSpeed();
	localConfig.voice_gap = c->getVoiceGap();
	localConfig.feedback_speed = c->getFeedbackSpeed();
	localConfig.feedback_method = c->getFeedbackMethod();
	localConfig.navigation_distance = c->getNavigationDistance();
	localConfig.navigation_type = c->getNavigationType();
	localConfig.keymap["K0"] = c->getKeypadFunction("K0");
	localConfig.keymap["K1"] = c->getKeypadFunction("K1");
	localConfig.keymap["K2"] = c->getKeypadFunction("K2");
	localConfig.keymap["K3"] = c->getKeypadFunction("K3");
	localConfig.keymap["K4"] = c->getKeypadFunction("K4");
	localConfig.keymap["K5"] = c->getKeypadFunction("K5");
	localConfig.keymap["K6"] = c->getKeypadFunction("K6");
	localConfig.keymap["K7"] = c->getKeypadFunction("K7");
	localConfig.keymap["K8"] = c->getKeypadFunction("K8");
	localConfig.keymap["K9"] = c->getKeypadFunction("K9");
	localConfig.keymap["KA"] = c->getKeypadFunction("KA");
	localConfig.keymap["KB"] = c->getKeypadFunction("KB");
	localConfig.keymap["KC"] = c->getKeypadFunction("KC");
	localConfig.keymap["KD"] = c->getKeypadFunction("KD");
	localConfig.keymap["KE"] = c->getKeypadFunction("KE");
	localConfig.keymap["KF"] = c->getKeypadFunction("KF");
	localConfig.keymap["KG"] = c->getKeypadFunction("KG");
	localConfig.keymap["KH"] = c->getKeypadFunction("KH");
	localConfig.keymap["KI"] = c->getKeypadFunction("KI");
	localConfig.keymap["NUMLOCK"] = c->getKeypadFunction("NUMLOCK");
	localConfig.height = c->getUserHeight();
	changedConfigs.clear();
}

Configuration::~Configuration()
{
	saveChangedConfigs();
}

void Configuration::reloadConfig()
{
}

float Configuration::getMaximumDistance()
{
	return localConfig.detection_distance;
}

float Configuration::getMinimumDistance()
{
	return localConfig.navigation_distance;
}

void Configuration::increaseMaximumDistance()
{
	// localConfig.detection_distance += 0.5;
	localConfig.detection_distance = localConfig.detection_distance + 0.5 > localConfig.navigation_distance ? localConfig.detection_distance : localConfig.detection_distance + 0.5;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::DETECTION_DISTANCE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::DETECTION_DISTANCE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Maximum distance set to " << localConfig.detection_distance;
}

void Configuration::decreaseMaximumDistance()
{
	// localConfig.detection_distance = localConfig.detection_distance - 0.5 < localConfig.navigation_distance ? localConfig.detection_distance : localConfig.detection_distance - 0.5;
	localConfig.detection_distance = localConfig.detection_distance - 0.5 < 1 ? localConfig.detection_distance : localConfig.detection_distance - 0.5;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::DETECTION_DISTANCE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::DETECTION_DISTANCE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Maximum distance set to " << localConfig.detection_distance;
}

void Configuration::increaseMinimumDistance()
{
	// localConfig.navigation_distance = localConfig.navigation_distance + 0.5 > localConfig.detection_distance ? localConfig.navigation_distance : localConfig.navigation_distance + 0.5;
	localConfig.navigation_distance += 0.5;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::NAVIGATION_DISTANCE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::NAVIGATION_DISTANCE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Minimum distance set to " << localConfig.navigation_distance;
}

void Configuration::decreaseMinimumDistance()
{
	// localConfig.navigation_distance = localConfig.navigation_distance - 0.5 < 1 ? localConfig.navigation_distance : localConfig.navigation_distance - 0.5;
	localConfig.navigation_distance = localConfig.navigation_distance - 0.5 < localConfig.detection_distance ? localConfig.navigation_distance : localConfig.navigation_distance - 0.5;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::NAVIGATION_DISTANCE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::NAVIGATION_DISTANCE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Minimum distance set to " << localConfig.navigation_distance;
}

void Configuration::increaseSystemVolume()
{
	if (!gvars.systemVolume)
	{
		float vol = getSystemAPIVolume();
		localConfig.feedback_volume = vol + volstep;
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_VOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_VOLUME_CFG);
		BOOST_LOG_TRIVIAL(info) << "(Configuration) Volume set to " << localConfig.feedback_volume;
	}
	else
	{
		int vol = getSystemVolume();
		setSystemVolume(vol + volpstep);
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_SVOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_SVOLUME_CFG);
	}
}

void Configuration::decreaseSystemVolume()
{
	if (!gvars.systemVolume)
	{
		float vol = getSystemAPIVolume();
		if ((vol - volstep) <= 0)
		{
			BOOST_LOG_TRIVIAL(info) << "(Configuration) Minimum volume reached";
			return;
		}
		localConfig.feedback_volume = vol - volstep;
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_VOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_VOLUME_CFG);
		BOOST_LOG_TRIVIAL(info) << "(Configuration) Volume set to " << localConfig.feedback_volume;
	}
	else
	{
		int vol = getSystemVolume();
		if (vol - volpstep >= minsvol)
		{
			setSystemVolume(vol - volpstep);
			if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_SVOLUME_CFG) == changedConfigs.end())
				changedConfigs.push_back(ConfigList::FEEDBACK_SVOLUME_CFG);
		}
	}
}

void Configuration::setSystemMinVol()
{
	if (!gvars.systemVolume)
	{
		localConfig.feedback_volume = volstep;
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_VOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_VOLUME_CFG);
		BOOST_LOG_TRIVIAL(info) << "(Configuration) Volume set to " << localConfig.feedback_volume;
	}
	else
	{
		setSystemVolume(minsvol);
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_SVOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_SVOLUME_CFG);
	}
}

void Configuration::setSystemDefVol()
{
	if (!gvars.systemVolume)
	{
		localConfig.feedback_volume = defvol;
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_VOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_VOLUME_CFG);
		BOOST_LOG_TRIVIAL(info) << "(Configuration) Volume set to " << localConfig.feedback_volume;
	}
	else
	{
		setSystemVolume(defsvol);
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_SVOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_SVOLUME_CFG);
	}
}

void Configuration::setSystemHighVol()
{
	if (!gvars.systemVolume)
	{
		localConfig.feedback_volume = defvol * 2;
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_VOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_VOLUME_CFG);
		BOOST_LOG_TRIVIAL(info) << "(Configuration) Volume set to " << localConfig.feedback_volume;
	}
	else
	{
		setSystemVolume(maxsvol);
		if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_SVOLUME_CFG) == changedConfigs.end())
			changedConfigs.push_back(ConfigList::FEEDBACK_SVOLUME_CFG);
	}
}

float Configuration::getSystemAPIVolume()
{
	return localConfig.feedback_volume;
}

bool Configuration::isVoiceFeedback()
{
	return localConfig.feedback_type == FeedbackAcousticType::Voice;
}

void Configuration::setToneFeedback()
{
}

void Configuration::setVoiceFeedback()
{
}

bool Configuration::isPathNavigation()
{
	return localConfig.navigation_type == NavigationType::Path || localConfig.navigation_type == NavigationType::BothNT;
}

void Configuration::setNavigationType(string allocator)
{
}

bool Configuration::isPositionalFeedback()
{
	return localConfig.feedback_positional == FeedbackAcousticDirection::Positional;
}

void Configuration::setPosStereoFeedback(string s)
{
}

void Configuration::setDistanceType(string d)
{
}

bool Configuration::isDistanceTypeMeters()
{
	return localConfig.feedback_distancetype == DistanceType::Meters;
}

float Configuration::getMPacesVal()
{
	return localConfig.feedback_pacevalue;
}

bool Configuration::isCommandPtBr()
{
	return false;
}

void Configuration::setCommandLang(string lang)
{
}

int Configuration::getFeedbackSpeed()
{
	return localConfig.feedback_speed;
}

void Configuration::updateFeedbackSpeed(int sp)
{
	localConfig.feedback_speed = sp;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_SPEED_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::FEEDBACK_SPEED_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Feedback speed set to " << localConfig.feedback_speed;
}

void Configuration::setSceneContFeedback(string sc)
{
}

void Configuration::setSceneContFeedback(
	FeedbackSceneMode fd)
{
	localConfig.feedback_contscene = fd;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_CONTSCENE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::FEEDBACK_CONTSCENE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Feedback scene set to " << getFeedbackSceneDesc(localConfig.feedback_contscene);
}

bool Configuration::isContinuousFeedback()
{
	return localConfig.feedback_contscene == FeedbackSceneMode::Active;
}

float Configuration::getNavigationDistance()
{
	return localConfig.navigation_distance;
}

void Configuration::setNavigationDistance(float d)
{
	localConfig.navigation_distance = d;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::NAVIGATION_DISTANCE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::NAVIGATION_DISTANCE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Navigation distance set to " << localConfig.navigation_distance;
}

NavigationType Configuration::getNavigationType()
{
	return localConfig.navigation_type;
}

void Configuration::setNavigationType(NavigationType nt)
{
	localConfig.navigation_type = nt;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::NAVIGATION_TYPE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::NAVIGATION_TYPE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Navigation type set to " << getNavigationTypeDesc(localConfig.navigation_type);
}

float Configuration::getDistance()
{
	return localConfig.navigation_distance;
}

void Configuration::setDistance(float d)
{
	localConfig.navigation_distance = d;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::NAVIGATION_DISTANCE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::NAVIGATION_DISTANCE_CFG);
}

void Configuration::setKeypadFunction(string k, int f)
{
	localConfig.keymap[k] = f;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::KEYMAP_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::KEYMAP_CFG);
}

int Configuration::getKeypadFunction(string k)
{
	return localConfig.keymap[k];
}

void Configuration::setSystemVolume(int vol)
{
	// return;
	long min, max;
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	const char *card = "default";
	const char *selem_name = "Master";

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, selem_name);
	snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_set_playback_volume_all(elem, vol * max / 100);

	snd_mixer_close(handle);
	localConfig.feedback_svolume = vol;
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Volume set to " << localConfig.feedback_svolume;
}

int Configuration::getSystemVolume()
{
	return localConfig.feedback_svolume;
}

FeedbackMethod Configuration::getFeedbackMethod()
{
	return localConfig.feedback_method;
}

void Configuration::setFeedbackMethod(FeedbackMethod fm)
{
	localConfig.feedback_method = fm;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_METHOD_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::FEEDBACK_METHOD_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Feedback method set to " << getFeedbackMethodDesc(localConfig.feedback_method);
}

int Configuration::getVoiceSpeed()
{
	return localConfig.voice_speed;
}

void Configuration::setVoiceSpeed(int speed)
{
	localConfig.voice_speed = speed;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::VOICE_SPEED_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::VOICE_SPEED_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Voice speed set to " << localConfig.voice_speed;
}

int Configuration::getVoiceGap()
{
	return localConfig.voice_gap;
}

void Configuration::setVoiceGap(int gap)
{
	localConfig.voice_gap = gap;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::VOICE_GAP_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::VOICE_GAP_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Voice gap set to " << localConfig.voice_gap;
}

int Configuration::getUserHeight()
{
	return localConfig.height;
}

FeedbackAcousticType Configuration::getAcousticFeedbackType()
{
	return localConfig.feedback_type;
}

void Configuration::setAcousticFeedbackType(
	FeedbackAcousticType ac)
{
	localConfig.feedback_type = ac;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_TYPE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::FEEDBACK_TYPE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Feedback acoustic type set to " << getFeedbackAcousticTypeDesc(localConfig.feedback_type);
}

void Configuration::setPosStereoFeedback(
	FeedbackAcousticDirection acfd)
{
	localConfig.feedback_positional = acfd;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_POSITIONAL_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::FEEDBACK_POSITIONAL_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Feedback acoustic direction set to " << getFeedbackDirectionDesc(localConfig.feedback_positional);
}

void Configuration::setDistanceType(DistanceType d)
{
	localConfig.feedback_distancetype = d;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::FEEDBACK_DISTANCETYPE_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::FEEDBACK_DISTANCETYPE_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) Feedback distance type set to " << getDistanceDesc(localConfig.feedback_distancetype);
}

void Configuration::setUserHeight(int h)
{
	localConfig.height = h;
	if (std::find(changedConfigs.begin(), changedConfigs.end(), ConfigList::HEIGHT_CFG) == changedConfigs.end())
		changedConfigs.push_back(ConfigList::HEIGHT_CFG);
	BOOST_LOG_TRIVIAL(info) << "(Configuration) User height set to " << localConfig.height;
}

void Configuration::saveChangedConfigs()
{
	if (changedConfigs.size() <= 0)
		return;
	for (ConfigList c : changedConfigs)
	{
		switch (c)
		{
		case DETECTION_DISTANCE_CFG:
			cfg->updateDetectionDistance(localConfig.detection_distance);
			break;
		case FEEDBACK_TONETIMES_CFG:
			// todo
			break;
		case FEEDBACK_PACEVALUE_CFG:
			// todo
			break;
		case FEEDBACK_TYPE_CFG:
			cfg->setAcousticFeedbackType(localConfig.feedback_type);
			break;
		case FEEDBACK_CONTSCENE_CFG:
			cfg->setSceneContFeedback(localConfig.feedback_contscene);
			break;
		case FEEDBACK_SVOLUME_CFG:
			cfg->setSystemVolume(localConfig.feedback_svolume);
			break;
		case FEEDBACK_DISTANCETYPE_CFG:
			cfg->setDistanceType(localConfig.feedback_distancetype);
			break;
		case FEEDBACK_VOLUME_CFG:
			cfg->updateSystemVolume(localConfig.feedback_volume);
			break;
		case FEEDBACK_POSITIONAL_CFG:
			cfg->setPosStereoFeedback(localConfig.feedback_positional);
			break;
		case VOICE_SPEED_CFG:
			cfg->setVoiceSpeed(localConfig.voice_speed);
			break;
		case VOICE_GAP_CFG:
			cfg->setVoiceGap(localConfig.voice_gap);
			break;
		case FEEDBACK_SPEED_CFG:
			cfg->updateFeedbackSpeed(localConfig.feedback_speed);
			break;
		case FEEDBACK_METHOD_CFG:
			cfg->setFeedbackMethod(localConfig.feedback_method);
			break;
		case NAVIGATION_DISTANCE_CFG:
			cfg->setNavigationDistance(localConfig.navigation_distance);
			break;
		case NAVIGATION_TYPE_CFG:
			cfg->setNavigationType(localConfig.navigation_type);
			break;
		case KEYMAP_CFG:
			// todo
			break;
		case HEIGHT_CFG:
			cfg->setUserHeight(localConfig.height);
			break;
		}
	}
}

string Configuration::getFeedbackSceneDesc(
	FeedbackSceneMode f)
{
	switch (f)
	{
	case FeedbackSceneMode::Active:
		return "Active";
		break;
	case FeedbackSceneMode::Passive:
		return "Passive";
		break;
	default:
		return "Not found";
		break;
	}
	return "Not found";
}

string Configuration::getFeedbackAcousticTypeDesc(
	FeedbackAcousticType f)
{
	switch (f)
	{
	case FeedbackAcousticType::Tone:
		return "Tone";
		break;
	case FeedbackAcousticType::Voice:
		return "Voice";
		break;
	default:
		return "Not found";
		break;
	}
}

string Configuration::getFeedbackDirectionDesc(
	FeedbackAcousticDirection f)
{
	switch (f)
	{
	case FeedbackAcousticDirection::Positional:
		return "Positional";
		break;
	case FeedbackAcousticDirection::Stereo:
		return "Stereo";
		break;
	default:
		return "Not found";
		break;
	}
}

string Configuration::getFeedbackMethodDesc(FeedbackMethod f)
{
	switch (f)
	{
	case FeedbackMethod::Acoustic:
		return "Acoustic";
		break;
	case FeedbackMethod::Tactile:
		return "Tactile";
		break;
	case FeedbackMethod::Both:
		return "Acoustic+Tactile";
		break;
	default:
		return "Not found";
		break;
	}
}

string Configuration::getDistanceDesc(DistanceType d)
{
	switch (d)
	{
	case DistanceType::Meters:
		return "Meters";
		break;
	case DistanceType::Paces:
		return "Paces";
		break;
	default:
		return "Not found";
		break;
	}
}

string Configuration::getNavigationTypeDesc(NavigationType nt)
{
	switch (nt)
	{
	case NavigationType::BothNT:
		return "Path+Obstacle";
		break;
	case NavigationType::Path:
		return "Path";
		break;
	case NavigationType::Obstacle:
		return "Obstacle";
		break;
	default:
		return "Not found";
		break;
	}
}

pair<string, int> Configuration::changeEditableKey(string key)
{
	int keycode = cfg->getKeypadFunction(key);
	keycode = ++keycode == cfg->commands.size() ? 0 : keycode;
	cfg->setKeypadFunction(key, keycode);
	return make_pair(cfg->commands[keycode], keycode);
}
