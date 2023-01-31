/*
 * ThreadController.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC_THREADCONTROLLER_H_
#define SRC_THREADCONTROLLER_H_

#include "VILayer.h"
#include "PILayer.h"
#include <thread>
#include <pthread.h>
#include <future>
#include "__afsvipGLOBALS.h"

class ThreadController {
public:
	ThreadController(VILayer*,PILayer*);
	std::thread startThreads();
	virtual ~ThreadController();
	void createAndRun();
private:
	VILayer* vi;
	PILayer* pi;

};

#endif /* SRC_THREADCONTROLLER_H_ */
