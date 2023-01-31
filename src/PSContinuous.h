/*
 * Based on original pocketsphinx continuous.c of Carnegie Mellon on:
 * https://github.com/cmusphinx/pocketsphinx/blob/master/src/programs/continuous.c
 *
 * Copyright (c) 1999-2010 Carnegie Mellon University.  All rights
 * reserved.
 *
 * PSContinuous.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */


/* EM CASO DE ERRO, CORRIGIR OS HEADERS EM /usr/local/include DO SPHINXBASE E POCKETSPHINX
 * ADICIONAR "sphinxbase/" OU "pocketsphinx/" PARA CORRIGIR OS BUGS.
 */


#ifndef SRC_PSCONTINUOUS_H_
#define SRC_PSCONTINUOUS_H_


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sys/select.h>
#include <thread>
#include <sys/types.h>
#include <sys/time.h>
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <signal.h>
#include <setjmp.h>
#include <pocketsphinx/pocketsphinx.h>
#include <pocketsphinx/cmdln_macro.h>
#include "VoiceHypotesisProcessor.h"
#include "FeedbackLayer.h"
#include "__afsvipGLOBALS.h"
using namespace std;

static ps_decoder_t *ps;
static cmd_ln_t *config;
static FILE* rawfd;
static jmp_buf jbuf;

/*static const arg_t cont_args_def[] = {
    POCKETSPHINX_OPTIONS,
    {"-argfile",
     ARG_STRING,
     NULL,
     "Argument file giving extra arguments."},
    {"-adcdev",
     ARG_STRING,
     NULL,
     "Name of audio device to use for input."},
    {"-infile",
     ARG_STRING,
     NULL,
     "Audio file to transcribe."},
    {"-inmic",
     ARG_BOOLEAN,
     "no",
     "Transcribe audio from microphone."},
    {"-time",
     ARG_BOOLEAN,
     "no",
     "Print word times in file transcription."},
    CMDLN_EMPTY_OPTION
};*/

class PSContinuous {
public:
	//PSContinuous();
	PSContinuous(FeedbackLayer*);
	PSContinuous(FeedbackLayer*,Configuration*);
	virtual ~PSContinuous();
	void listen();
	std::thread listenThread();
	static void* listenT(void*);

private:
	FeedbackLayer* feedback;
	Configuration* cfg;
	static int32 ad_file_read(ad_rec_t* ad,int16* buf,int32 max);
	static void print_word_times(int32 start);
	static void sleep_msec(int32 ms);
	static void sighandler(int signo);
	VoiceHypotesisProcessor* processor;
};

#endif /* SRC_PSCONTINUOUS_H_ */
