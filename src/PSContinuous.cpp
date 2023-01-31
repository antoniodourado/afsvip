/* * PSContinuous.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#include "PSContinuous.h"

/*PSContinuous::PSContinuous() {
	cout << "Configuring sphinx...";
	try{
		static const char *hmmpath = "Assets/SphinxModel/en-us/";
		static const char *lmfile = "Assets/SphinxModel/afsvip.lm";
		static const char *dicfile = "Assets/SphinxModel/afsvip.dic";
		config = cmd_ln_init(NULL,ps_args(),TRUE,
				"-hmm", hmmpath,
				"-lm", lmfile,
				"-dict", dicfile,
				"-logfn", "/dev/null",
				"-debug","0",
				NULL);
		ps_default_search_args(config);
		ps = ps_init(config);
		cout << "Done!" << endl;
		processor = new VoiceHypotesisProcessor();
	}
	catch(exception err){
		cout << "Erro: " << err.what() << endl;
	}

}*/

PSContinuous::~PSContinuous() {
	ps_free(ps);
	cmd_ln_free_r(config);
	free(processor);
}

void PSContinuous::listen() {
	ad_rec_t *ad;
	int16 adbuf[2048];
	uint8 utt_started, in_speech;
	int32 k;
	char const *hyp;

	if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
			(int) cmd_ln_float32_r(config,
					"-samprate"))) == NULL)
		E_FATAL("Failed to open audio device\n");
	if (ad_start_rec(ad) < 0)
		E_FATAL("Failed to start recording\n");

	if (ps_start_utt(ps) < 0)
		E_FATAL("Failed to start utterance\n");
	utt_started = FALSE;
	E_INFO("Ready....\n");

	bool uStopped=false;
	while(!gvars.haltSystem) {
		if(gvars.stopVoiceListener){
			if(!uStopped){
				utt_started=TRUE;
				ps_end_utt(ps);
				ad_stop_rec(ad);
				ad_close(ad);
				uStopped=true;
			}
			sleep_msec(100);
			continue;
		}
		else if(uStopped){
			if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
					(int) cmd_ln_float32_r(config,
							"-samprate"))) == NULL)
				E_FATAL("Failed to open audio device\n");
			if (ad_start_rec(ad) < 0)
				E_FATAL("Failed to start recording\n");

			if (ps_start_utt(ps) < 0)
				E_FATAL("Failed to start utterance\n");
			utt_started = FALSE;
			uStopped=false;
		}
		if ((k = ad_read(ad, adbuf, 2048)) < 0)
			E_FATAL("Failed to read audio\n");
		ps_process_raw(ps, adbuf, k, FALSE, FALSE);
		in_speech = ps_get_in_speech(ps);
		if (in_speech && !utt_started) {
			utt_started = TRUE;
			E_INFO("Listening...\n");
		}
		if (!in_speech && utt_started) {
			/* speech -> silence transition, time to start new utterance  */
			ps_end_utt(ps);
			hyp = ps_get_hyp(ps, NULL );
			if (hyp != NULL && strlen(hyp) > 0) {
				printf("%s\n", hyp);
				//processor->doProcess(hyp);
				string varg = hyp;
				std::thread hypT = processor->doProcessString(varg);
				hypT.detach();
				fflush(stdout);
			}

			if (ps_start_utt(ps) < 0)
				E_FATAL("Failed to start utterance\n");
			utt_started = FALSE;
			E_INFO("Ready....\n");
		}
		sleep_msec(100);
	}
	ps_end_utt(ps);
	ad_stop_rec(ad);
	ad_close(ad);
}

void* PSContinuous::listenT(void *threadid) {
	ad_rec_t *ad;
	int16 adbuf[2048];
	uint8 utt_started, in_speech;
	int32 k;
	char const *hyp;
	VoiceHypotesisProcessor* sprocessor = new VoiceHypotesisProcessor;

	if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
			(int) cmd_ln_float32_r(config,
					"-samprate"))) == NULL)
		E_FATAL("Failed to open audio device\n");
	if (ad_start_rec(ad) < 0)
		E_FATAL("Failed to start recording\n");

	if (ps_start_utt(ps) < 0)
		E_FATAL("Failed to start utterance\n");
	utt_started = FALSE;
	E_INFO("Ready....\n");

	for (;;) {
		if ((k = ad_read(ad, adbuf, 2048)) < 0)
			E_FATAL("Failed to read audio\n");
		ps_process_raw(ps, adbuf, k, FALSE, FALSE);
		in_speech = ps_get_in_speech(ps);
		if (in_speech && !utt_started) {
			utt_started = TRUE;
			E_INFO("Listening...\n");
		}
		if (!in_speech && utt_started) {
			/* speech -> silence transition, time to start new utterance  */
			ps_end_utt(ps);
			hyp = ps_get_hyp(ps, NULL );
			/*if (hyp != NULL) {
				printf("%s\n", hyp);
				sprocessor->doProcess(hyp);
				fflush(stdout);
			}*/
			if (hyp != NULL && strlen(hyp) > 0) {
				printf("%s\n", hyp);
				//processor->doProcess(hyp);
				string varg = hyp;
				sprocessor->doProcess(hyp);
				//hypT.detach();
				fflush(stdout);
			}

			if (ps_start_utt(ps) < 0)
				E_FATAL("Failed to start utterance\n");
			utt_started = FALSE;
			E_INFO("Ready....\n");
		}
		sleep_msec(100);
	}
	ad_close(ad);
}



int32 PSContinuous::ad_file_read(ad_rec_t* ad, int16* buf, int32 max) {
	size_t nread;
	nread = fread(buf, sizeof(int16), max, rawfd);
	return (nread > 0 ? nread : -1);
}

void PSContinuous::print_word_times(int32 start) {
	ps_seg_t *iter = ps_seg_iter(ps);
	while (iter != NULL) {
		int32 sf, ef, pprob;
		float conf;

		ps_seg_frames (iter, &sf, &ef);
		pprob = ps_seg_prob (iter, NULL, NULL, NULL);
		conf = logmath_exp(ps_get_logmath(ps), pprob);
		printf ("%s %f %f %f\n", ps_seg_word (iter), (sf + start) / 100.0, (ef + start) / 100.0, conf);
		iter = ps_seg_next (iter);
	}
}

void PSContinuous::sleep_msec(int32 ms) {
	struct timeval tmo;

	tmo.tv_sec = 0;
	tmo.tv_usec = ms * 1000;

	select(0, NULL, NULL, NULL, &tmo);
}

PSContinuous::PSContinuous(FeedbackLayer* f, Configuration* c) {
	BOOST_LOG_TRIVIAL(info) << "Configuring sphinx...";
	try{

		string shmmpath = "Assets/SphinxModel/";
		string slmfile = "Assets/SphinxModel/";
		string sdicfile = "Assets/SphinxModel/";
		if(c->isCommandPtBr()){
			shmmpath.append("pt-br/");
			slmfile.append("pt-br/afsvip.lm");
			sdicfile.append("pt-br/afsvip.dic");
		}
		else{
			shmmpath.append("en-us/");
			slmfile.append("en-us/afsvip.lm");
			sdicfile.append("en-us/afsvip.dic");
		}
		/*const char *hmmpath = shmmpath.c_str();
		const char *lmfile = slmfile.c_str();
		const char *dicfile = sdicfile.c_str();*/
		config = cmd_ln_init(NULL,ps_args(),TRUE,
				"-hmm", shmmpath.c_str(),
				"-lm", slmfile.c_str(),
				"-dict", sdicfile.c_str(),
				"-logfn", "/dev/null",
				"-debug","0",
				NULL);
		ps_default_search_args(config);
		ps = ps_init(config);
		BOOST_LOG_TRIVIAL(info) << "Done!";
		processor = new VoiceHypotesisProcessor(f,c);
		feedback = f;
		cfg=c;
	}
	catch(exception err){
		BOOST_LOG_TRIVIAL(error) << "Erro: " << err.what();
	}
}

void PSContinuous::sighandler(int signo) {
	longjmp(jbuf, 1);
}

std::thread PSContinuous::listenThread() {
	return std::thread([=] {listen();});
	//return std::thread(&PSContinuous::listen,this);
}

PSContinuous::PSContinuous(FeedbackLayer* f) {
	BOOST_LOG_TRIVIAL(info) << "Configuring sphinx...";
	try{
		static const char *hmmpath = "Assets/SphinxModel/en-us/";
		static const char *lmfile = "Assets/SphinxModel/afsvip.lm";
		static const char *dicfile = "Assets/SphinxModel/afsvip.dic";
		config = cmd_ln_init(NULL,ps_args(),TRUE,
				"-hmm", hmmpath,
				"-lm", lmfile,
				"-dict", dicfile,
				"-logfn", "/dev/null",
				"-debug","0",
				NULL);
		ps_default_search_args(config);
		ps = ps_init(config);
		cout << "Done!" << endl;
		processor = new VoiceHypotesisProcessor(f);
		feedback = f;
	}
	catch (exception &err) {
		cout << "Erro: " << err.what() << endl;
	}
}
