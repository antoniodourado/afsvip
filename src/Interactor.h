/*
 * Interactor.h
 *
 *  Created on: 5 de out de 2019
 *      Author: antonio
 */

#ifndef SRC_INTERACTOR_H_
#define SRC_INTERACTOR_H_
#include <thread>

class Interactor
{
public:
	Interactor();
	virtual ~Interactor();
	virtual std::thread startListening() = 0;
};

#endif /* SRC_INTERACTOR_H_ */
