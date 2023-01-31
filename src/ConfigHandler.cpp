/* ConfigHandler.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "ConfigHandler.h"

ConfigHandler::ConfigHandler() {

	struct stat buffer;
	if(stat(configfile,&buffer) != 0){
		BOOST_LOG_TRIVIAL(error) << "(ConfigHandler) System ConfigHandler file not found. Building a new one.";
		buildNewConfigFile();
	}

	try{
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Reading system ConfigHandler file...";
		cfg.readFile(configfile);
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Done!";
	}catch(exception err){
		BOOST_LOG_TRIVIAL(error) << "(ConfigHandler) Error while loading config file: " << err.what();
	}
	if(gvars.systemVolume)
		setSystemVolume(getSystemVolume());
}

float ConfigHandler::getMaximumDistance(){
	float dist;
	Setting &root = cfg.getRoot();
	/*if(!root.exists("detection")){
		root.add("detection",Setting::TypeGroup)
	}*/
	Setting &detection = root["detection"];
	if(!detection.lookupValue("detection_distance",dist)){
		dist = 1;
	}
	return dist;
}

void ConfigHandler::updateDetectionDistance(float dist){

	Setting &detection = cfg.getRoot()["detection"];
	detection.remove("detection_distance");
	detection.add("detection_distance",Setting::TypeFloat) = dist;
	cfg.writeFile(configfile);

}

void ConfigHandler::setSystemMinVol() {
	//cout << "Setting feedback volume to " << volstep << endl;
	if(!gvars.systemVolume)
		updateSystemVolume(volstep);
	else{
		setSystemVolume(minsvol);
	}
}

void ConfigHandler::setSystemDefVol() {
	//cout << "Setting feedback volume to default " << defvol << endl;
	if(!gvars.systemVolume)
		updateSystemVolume(defvol);
	else{
		setSystemVolume(defsvol);
	}
}

void ConfigHandler::setSystemHighVol() {
	//cout << "Setting feedback volume to " << defvol*2 << endl;
	if(!gvars.systemVolume)
		updateSystemVolume(defvol*2);
	else{
		setSystemVolume(maxsvol);
	}
}

void ConfigHandler::setToneFeedback() {
	Setting &feedback = cfg.getRoot()["feedback"];
	feedback.remove("feedback_type");
	feedback.add("feedback_type",Setting::TypeString) = "tone";
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback type changed to tone";
}

void ConfigHandler::setVoiceFeedback() {
	Setting &feedback = cfg.getRoot()["feedback"];
	feedback.remove("feedback_type");
	feedback.add("feedback_type",Setting::TypeString) = "voice";
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback type changed to voice";
}

void ConfigHandler::setAcousticFeedbackType(FeedbackAcousticType type) {
	Setting &feedback = cfg.getRoot()["feedback"];
	feedback.remove("feedback_type");
	feedback.add("feedback_type",Setting::TypeInt) = type;
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Acoustic feedback type changed to " << type;
}

bool ConfigHandler::isPathNavigation() {
	int type;
	Setting &root = cfg.getRoot();
	if(!root.exists("navigation")){
		Setting &nav = root.add("navigation",Setting::TypeGroup);
		nav.add("navigation_type",Setting::TypeInt) = NavigationType::Path;
		cfg.writeFile(configfile);
		return true;
	}
	Setting &nav = root["navigation"];
	if(!nav.lookupValue("navigation_type",type))
		return true;
	return (static_cast<NavigationType>(type) == NavigationType::Path || static_cast<NavigationType>(type) == NavigationType::BothNT);
}

NavigationType ConfigHandler::getNavigationType() {
	int type;
	Setting &root = cfg.getRoot();
	if(!root.exists("navigation")){
		Setting &nav = root.add("navigation",Setting::TypeGroup);
		nav.add("navigation_type",Setting::TypeInt) = NavigationType::Path;
		cfg.writeFile(configfile);
		return NavigationType::Path;
	}
	Setting &nav = root["navigation"];
	if(!nav.lookupValue("navigation_type",type))
		return NavigationType::Path;
	return static_cast<NavigationType>(type);
}

void ConfigHandler::setNavigationType(string type) {
	if(!cfg.getRoot().exists("navigation")){
		Setting &nav = cfg.getRoot().add("navigation",Setting::TypeGroup);
		nav.add("navigation_type",Setting::TypeString) = type;
	}else{
		Setting &nav = cfg.getRoot()["navigation"];
		if(nav.exists("navigation_type"))
			nav.remove("navigation_type");
		nav.add("navigation_type",Setting::TypeString) = type;
	}
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Navigation type changed to " << type;

}

void ConfigHandler::setNavigationType(NavigationType type) {
	if(!cfg.getRoot().exists("navigation")){
		Setting &nav = cfg.getRoot().add("navigation",Setting::TypeGroup);
		nav.add("navigation_type",Setting::TypeInt) = type;
	}else{
		Setting &nav = cfg.getRoot()["navigation"];
		if(nav.exists("navigation_type"))
			nav.remove("navigation_type");
		nav.add("navigation_type",Setting::TypeInt) = type;
	}
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Navigation type changed to " << type;
}

bool ConfigHandler::isPositionalFeedback() {
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.exists("feedback_positional")){
		feedback.add("feedback_positional",Setting::TypeInt) = FeedbackAcousticDirection::Positional;
		cfg.writeFile(configfile);
		return true;
	}
	int type;
	feedback.lookupValue("feedback_positional",type);
	return static_cast<FeedbackAcousticDirection>(type) == FeedbackAcousticDirection::Positional;
}

int ConfigHandler::getFeedbackSpeed() {
	int currSpeed = 5;
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.exists("feedback_speed")){
		feedback.add("feedback_speed",Setting::TypeInt) = currSpeed;
		cfg.writeFile(configfile);
		return currSpeed;
	}
	feedback.lookupValue("feedback_speed",currSpeed);
	return currSpeed;
}

void ConfigHandler::updateFeedbackSpeed(int speed) {
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Changing Feedback Speed to " << speed;
	Setting &feedb = cfg.getRoot()["feedback"];
	if(speed < 0)
		return;
	if(feedb.exists("feedback_speed"))
		feedb.remove("feedback_speed");
	feedb.add("feedback_speed",Setting::TypeInt) = speed;

	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Speed changed to " << speed;
}

void ConfigHandler::setSceneContFeedback(string type) {
	Setting &root = cfg.getRoot();
	Setting &feedb = root["feedback"];
	if(feedb.exists("feedback_contscene"))
		feedb.remove("feedback_contscene");
	feedb.add("feedback_contscene",Setting::TypeString) = type;
	cfg.writeFile(configfile);

	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback changed to " << type;
}

void ConfigHandler::setSceneContFeedback(FeedbackSceneMode m) {
	Setting &root = cfg.getRoot();
	Setting &feedb = root["feedback"];
	if(feedb.exists("feedback_contscene"))
		feedb.remove("feedback_contscene");
	feedb.add("feedback_contscene",Setting::TypeInt) = m;
	cfg.writeFile(configfile);

	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback changed to " << m;
}

bool ConfigHandler::isContinuousFeedback() {
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.exists("feedback_contscene")){
		feedback.add("feedback_contscene",Setting::TypeInt) = FeedbackSceneMode::Active;
		cfg.writeFile(configfile);
		return true;
	}
	int fd;
	feedback.lookupValue("feedback_contscene",fd);
	return static_cast<FeedbackSceneMode>(fd) == FeedbackSceneMode::Active;
}

float ConfigHandler::getNavigationDistance() {
	Setting &root = cfg.getRoot();
	if(!root.exists("navigation")){
		Setting &nav = root.add("navigation",Setting::TypeGroup);
		nav.add("navigation_distance",Setting::TypeFloat) = 100;
		cfg.writeFile(configfile);
		return 100;
	}
	Setting &nav = root["navigation"];
	if(!nav.exists("navigation_distance")){
		nav.add("navigation_distance",Setting::TypeFloat) = 100;
		cfg.writeFile(configfile);
		return 100;
	}
	float dist;
	nav.lookupValue("navigation_distance",dist);
	//BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Navig. Dist." << dist;
	return dist;
}

void ConfigHandler::setNavigationDistance(float dist) {
	if(!cfg.getRoot().exists("navigation")){
		Setting &nav = cfg.getRoot().add("navigation",Setting::TypeGroup);
		nav.add("navigation_distance",Setting::TypeFloat) = dist;
	}else{
		Setting &nav = cfg.getRoot()["navigation"];
		if(nav.exists("navigation_distance"))
			nav.remove("navigation_distance");
		nav.add("navigation_distance",Setting::TypeFloat) = dist;
	}
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Minimum distance changed to " << dist;
}

float ConfigHandler::getMinimumDistance() {
	Setting &root = cfg.getRoot();
	if(!root.exists("navigation")){
		Setting &nav = root.add("navigation",Setting::TypeGroup);
		nav.add("navigation_distance",Setting::TypeFloat) = 100;
		cfg.writeFile(configfile);
		return 100;
	}
	Setting &nav = root["navigation"];
	if(!nav.exists("navigation_distance")){
		nav.add("navigation_distance",Setting::TypeFloat) = 100;
		cfg.writeFile(configfile);
		return 100;
	}
	float dist;
	nav.lookupValue("navigation_distance",dist);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Navig. Dist." << dist;
	return dist;
}

void ConfigHandler::increaseMinimumDistance() {
	if(this->getMinimumDistance()+0.5 <= this->getMaximumDistance()){
		this->setNavigationDistance(this->getNavigationDistance()+0.5);
	}
	else
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Increasing minimum distance can't be higher than maximum.";
}

void ConfigHandler::decreaseMinimumDistance() {
	float currdist = this->getMinimumDistance();
	if(currdist > 1){
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Decreasing minimum distance by 0.5 meter";
		this->setNavigationDistance(currdist-0.5);
	}
	else
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Minimum distance of 1 meter reached";
}

void ConfigHandler::updateSystemVolume(float vol){

	Setting &feedback = cfg.getRoot()["feedback"];
	feedback.remove("feedback_volume");
	feedback.add("feedback_volume",Setting::TypeFloat) = vol;
	cfg.writeFile(configfile);
}

void ConfigHandler::reloadConfig() {
	cfg.readFile(configfile);
}

bool ConfigHandler::isVoiceFeedback() {
	int type;
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.lookupValue("feedback_type",type))
		return false;
	return static_cast<FeedbackAcousticType>(type) == FeedbackAcousticType::Voice;
}

float ConfigHandler::getSystemAPIVolume(){
	float vol;
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.lookupValue("feedback_volume",vol)){
		feedback.add("feedback_volume",Setting::TypeFloat) = vol;
		vol = 0.25;
	}
	return vol;
}

int ConfigHandler::getSystemVolume(){
	int vol;
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.lookupValue("feedback_svolume",vol)){
		feedback.add("feedback_svolume",Setting::TypeInt) = vol;
		vol = defsvol;
	}
	return vol;
}

void ConfigHandler::increaseSystemVolume() {
	if(!gvars.systemVolume){
		float vol = getSystemAPIVolume();
		//cout << "Increasing feedback volume to " << vol+volstep << endl;
		updateSystemVolume(vol+volstep);
	}
	else{
		int vol=getSystemVolume();
		setSystemVolume(vol+volpstep);
	}
}

void ConfigHandler::decreaseSystemVolume() {
	if(!gvars.systemVolume){
		float vol = getSystemAPIVolume();
		if((vol-volstep) <= 0){
			BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Minimum volume reached";
			return;
		}
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Decreasing feedback volume to " << vol-volstep;
		updateSystemVolume(vol-volstep);
	}
	else{
		int vol=getSystemVolume();
		if(vol-volpstep >= minsvol)
			setSystemVolume(vol-volpstep);
	}
}

void ConfigHandler::buildNewConfigFile(){

	Config newcfg;

	Setting &root = newcfg.getRoot();

	//User settings
	Setting &usr = root.add("user",Setting::TypeGroup);
	usr.add("height",Setting::TypeInt) = 165;

	//Navigation settings
	Setting &nav = root.add("navigation",Setting::TypeGroup);
	nav.add("navigation_type",Setting::TypeInt) = 0; //obstacle or path
	nav.add("navigation_distance",Setting::TypeFloat) = 1.5;

	//Detection settings
	Setting &detection = root.add("detection",Setting::TypeGroup);
	detection.add("detection_distance",Setting::TypeFloat) = 1.0;

	//Feedback Settings
	Setting &feedback = root.add("feedback",Setting::TypeGroup);
	feedback.add("feedback_type",Setting::TypeInt) = FeedbackAcousticType::Tone; //tone or voice
	feedback.add("feedback_tonetimes", Setting::TypeInt) = 3;
	feedback.add("feedback_volume", Setting::TypeFloat) = 0.05;
	feedback.add("feedback_svolume", Setting::TypeInt) = defsvol;
	feedback.add("feedback_contscene", Setting::TypeInt) = 0;
	feedback.add("feedback_positional", Setting::TypeInt) = 0; //if positional, instead 3D audio, voice will indicate. stereo otherwise
	// left, right, center
	feedback.add("feedback_distancetype", Setting::TypeInt) = DistanceType::Meters; //meters or paces
	feedback.add("feedback_pacevalue", Setting::TypeFloat) = 1.31;
	feedback.add("feedback_speed", Setting::TypeInt) = 5; //velocidade de feedback contínuo
	feedback.add("feedback_method",Setting::TypeInt) = 0; //acustico 0, tatil 1 e ambos 2

	Setting &keypad = root.add("keypad",Setting::TypeGroup);
	keypad.add("0", Setting::TypeInt) = 0;
	keypad.add("1", Setting::TypeInt) = 9;
	keypad.add("2", Setting::TypeInt) = 10;
	keypad.add("3", Setting::TypeInt) = 34;
	keypad.add("4", Setting::TypeInt) = 25;
	keypad.add("5", Setting::TypeInt) = 26;
	keypad.add("6", Setting::TypeInt) = 35;
	keypad.add("7", Setting::TypeInt) = 21;
	keypad.add("8", Setting::TypeInt) = 36;
	keypad.add("9", Setting::TypeInt) = 37;
	keypad.add("B", Setting::TypeInt) = 1;
	keypad.add("C", Setting::TypeInt) = 19;
	keypad.add("D", Setting::TypeInt) = 14;
	keypad.add("E", Setting::TypeInt) = 23;
	keypad.add("F", Setting::TypeInt) = 39;
	keypad.add("G", Setting::TypeInt) = 38;
	keypad.add("H", Setting::TypeInt) = 33;
	keypad.add("I", Setting::TypeInt) = 29;
	keypad.add("NUMLOCK", Setting::TypeInt) = 3;

	newcfg.writeFile(configfile);
}
ConfigHandler::~ConfigHandler() {
	// TODO Auto-generated destructor stub
}

void ConfigHandler::increaseMaximumDistance() {
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Increasing maximum distance by 0.5 meter";
	this->updateDetectionDistance(this->getMaximumDistance()+0.5);
}

void ConfigHandler::decreaseMaximumDistance() {
	if(this->getMaximumDistance()-0.5 >= this->getMinimumDistance()){
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Decreasing maximum distance by 0.5 meter";
		this->updateDetectionDistance(this->getMaximumDistance()-0.5);
	}
	else{
		BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Maximum distance can't be lower than minimum.";
	}

}

void ConfigHandler::setPosStereoFeedback(string type) {
	Setting &feedb = cfg.getRoot()["feedback"];
	if(feedb.exists("feedback_positional"))
		feedb.remove("feedback_positional");
	feedb.add("feedback_positional",Setting::TypeString) = type;

	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback type changed to " << type;
}

void ConfigHandler::setPosStereoFeedback(FeedbackAcousticDirection type) {
	Setting &feedb = cfg.getRoot()["feedback"];
	if(feedb.exists("feedback_positional"))
		feedb.remove("feedback_positional");
	feedb.add("feedback_positional",Setting::TypeInt) = type;

	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback type changed to " << type;
}

void ConfigHandler::setDistanceType(string type) {
	Setting &feedb = cfg.getRoot()["feedback"];
	if(feedb.exists("feedback_distancetype"))
		feedb.remove("feedback_distancetype");
	feedb.add("feedback_distancetype",Setting::TypeString) = type;

	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Distance type changed to " << type;
}

void ConfigHandler::setDistanceType(DistanceType type) {
	Setting &feedb = cfg.getRoot()["feedback"];
	if(feedb.exists("feedback_distancetype"))
		feedb.remove("feedback_distancetype");
	feedb.add("feedback_distancetype",Setting::TypeInt) = type;

	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Distance type changed to " << type;
}

bool ConfigHandler::isDistanceTypeMeters() {
	Setting &root = cfg.getRoot();
	Setting &feedback = root["feedback"];
	if(!feedback.exists("feedback_distancetype")){
		feedback.add("feedback_distancetype",Setting::TypeInt) = DistanceType::Meters;
		cfg.writeFile(configfile);
		return true;
	}
	int type;
	feedback.lookupValue("feedback_distancetype",type);
	return static_cast<DistanceType>(type) == DistanceType::Meters;
}

float ConfigHandler::getMPacesVal() {
	float val;
	if(!cfg.getRoot()["feedback"].lookupValue("feedback_pacevalue",val)){
		val = 0.82;
		cfg.getRoot()["feedback"].add("feedback_pacevalue",Setting::TypeFloat) = val;
		cfg.writeFile(configfile);
	}
	return val;
}

bool ConfigHandler::isCommandPtBr() {
	Setting &root = cfg.getRoot();
	if(!root.exists("internationalization")){
		Setting &inter = cfg.getRoot().add("internationalization",Setting::TypeGroup);
		inter.add("command_language",Setting::TypeString) = "pt-br";
		cfg.writeFile(configfile);
		return true;
	}
	else if(!cfg.getRoot()["internationalization"].exists("command_language")){
		cfg.getRoot()["internationalization"].add("command_language",Setting::TypeString) = "pt-br";
		cfg.writeFile(configfile);
		return true;
	}
	const char* lang;
	cfg.getRoot()["internationalization"].lookupValue("command_language",lang);
	return strcmp("pt-br",lang) == 0;
}

void ConfigHandler::setCommandLang(string lang) {
	Setting &root = cfg.getRoot();
	if(!root.exists("internationalization")){
		Setting &inter = cfg.getRoot().add("internationalization",Setting::TypeGroup);
		inter.add("command_language",Setting::TypeString) = "pt-br";
		cfg.writeFile(configfile);
	}
	else if(!cfg.getRoot()["internationalization"].exists("command_language")){
		cfg.getRoot()["internationalization"].add("command_language",Setting::TypeString) = "pt-br";
		cfg.writeFile(configfile);
	}
	else{
		cfg.getRoot()["internationalization"].remove("command_language");
		cfg.getRoot()["internationalization"].add("command_language",Setting::TypeString) = lang;
		cfg.writeFile(configfile);
	}
}

float ConfigHandler::getDistance() {
	float dist;
	if(isPathNavigation())
		dist = getNavigationDistance();
	else
		dist = getMaximumDistance();

	return dist;
}

void ConfigHandler::setDistance(float d) {
	if(isPathNavigation())
		setNavigationDistance(d);
}

void ConfigHandler::setKeypadFunction(string key, int val) {
	if(!cfg.getRoot().exists("keypad")){
		Setting &kp = cfg.getRoot().add("keypad",Setting::TypeGroup);
		kp.add(key,Setting::TypeInt) = val;
		cfg.writeFile(configfile);
		return;
	}
	Setting &kp = cfg.getRoot()["keypad"];
	if(kp.exists(key))
		kp.remove(key);
	kp.add(key,Setting::TypeInt) = val;
	cfg.writeFile(configfile);
    BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Keypad " << key << " changed to " << this->commands[val];
}

int ConfigHandler::getKeypadFunction(string key) {
	if(!cfg.getRoot().exists("keypad")){
		Setting &kp = cfg.getRoot().add("keypad",Setting::TypeGroup);
		kp.add(key,Setting::TypeInt) = 0;
		cfg.writeFile(configfile);
		return 0;
	}
	Setting &kp = cfg.getRoot()["keypad"];
	if(!kp.exists(key)){
		kp.add(key,Setting::TypeInt) = 0;
		cfg.writeFile(configfile);
	}
	int kpf;
	kp.lookupValue(key,kpf);
	return kpf;
}

void ConfigHandler::setSystemVolume(int vol) {
	return;
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
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_set_playback_volume_all(elem, vol * max / 100);

	snd_mixer_close(handle);

	Setting &feedback = cfg.getRoot()["feedback"];
	if(feedback.exists("feedback_svolume"))
		feedback.remove("feedback_svolume");
	feedback.add("feedback_svolume",Setting::TypeInt) = vol;
	cfg.writeFile(configfile);
}

FeedbackMethod ConfigHandler::getFeedbackMethod() {
	int method;
	if(!cfg.getRoot()["feedback"].lookupValue("feedback_method",method)){
		method = 0;
		cfg.getRoot()["feedback"].add("feedback_method",Setting::TypeInt) = method;
		cfg.writeFile(configfile);
	}
	return static_cast<FeedbackMethod>(method);
}

void ConfigHandler::setFeedbackMethod(FeedbackMethod method) {
	Setting &feedb = cfg.getRoot()["feedback"];
	if(feedb.exists("feedback_method"))
		feedb.remove("feedback_method");
	feedb.add("feedback_method",Setting::TypeInt) = method;

	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Feedback method changed to " << method;
}

int ConfigHandler::getVoiceSpeed() {
	Setting &root = cfg.getRoot();
	if(!root.exists("feedback")){
		Setting &fdb = root.add("feedback",Setting::TypeGroup);
		fdb.add("voice_speed",Setting::TypeInt) = 175;
		cfg.writeFile(configfile);
		return 175;
	}
	Setting &fdb = root["feedback"];
	if(!fdb.exists("voice_speed")){
		fdb.add("voice_speed",Setting::TypeInt) = 175;
		cfg.writeFile(configfile);
		return 175;
	}
	int speed;
	fdb.lookupValue("voice_speed",speed);
	return speed;
}

void ConfigHandler::setVoiceSpeed(int speed) {
	if(!cfg.getRoot().exists("feedback")){
		Setting &fdb = cfg.getRoot().add("feedback",Setting::TypeGroup);
		fdb.add("voice_speed",Setting::TypeInt) = speed;
	}else{
		Setting &fdb = cfg.getRoot()["feedback"];
		if(fdb.exists("voice_speed"))
			fdb.remove("voice_speed");
		fdb.add("voice_speed",Setting::TypeInt) = speed;
	}
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Voice Speed changed to " << speed;
}

int ConfigHandler::getVoiceGap(){
	Setting &root = cfg.getRoot();
	if(!root.exists("feedback")){
		Setting &fdb = root.add("feedback",Setting::TypeGroup);
		fdb.add("voice_gap",Setting::TypeInt) = 10;
		cfg.writeFile(configfile);
		return 10;
	}
	Setting &fdb = root["feedback"];
	if(!fdb.exists("voice_gap")){
		fdb.add("voice_gap",Setting::TypeInt) = 10;
		cfg.writeFile(configfile);
		return 10;
	}
	int gap;
	fdb.lookupValue("voice_gap",gap);
	return gap;
}

void ConfigHandler::setVoiceGap(int gap) {
	if(!cfg.getRoot().exists("feedback")){
		Setting &fdb = cfg.getRoot().add("feedback",Setting::TypeGroup);
		fdb.add("voice_gap",Setting::TypeInt) = gap;
	}else{
		Setting &fdb = cfg.getRoot()["feedback"];
		if(fdb.exists("voice_gap"))
			fdb.remove("voice_gap");
		fdb.add("voice_gap",Setting::TypeInt) = gap;
	}
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) Voice gap changed to " << gap;
}


int ConfigHandler::getUserHeight(){
	Setting &root = cfg.getRoot();
	if(!root.exists("user")){
		Setting &usr = root.add("user",Setting::TypeGroup);
		usr.add("height",Setting::TypeInt) = 165;
		cfg.writeFile(configfile);
		return 165;
	}
	Setting &usr = root["user"];
	if(!usr.exists("height")){
		usr.add("height",Setting::TypeInt) = 165;
		cfg.writeFile(configfile);
		return 165;
	}
	int height;
	usr.lookupValue("height",height);
	return height;
}

void ConfigHandler::setUserHeight(int height) {
	getUserHeight();
	Setting &usr = cfg.getRoot()["user"];
	if(usr.exists("height"))
		usr.remove("height");
	usr.add("height",Setting::TypeInt) = height;
	cfg.writeFile(configfile);
	BOOST_LOG_TRIVIAL(info) << "(ConfigHandler) User height changed to " << height;
}
