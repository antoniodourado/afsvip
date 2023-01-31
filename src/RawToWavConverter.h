/*
 * RawToWaveConverter.h
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#ifndef SRC_RAWTOWAVCONVERTER_H_
#define SRC_RAWTOWAVCONVERTER_H_
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <stdlib.h>
#include "CodeUtil.h"
#include <string.h>

namespace little_endian_io {
template<typename Word>
std::ostream& write_word(std::ostream& outs, Word value, unsigned size =
		sizeof(Word)) {
	for (; size; --size, value >>= 8)
		outs.put(static_cast<char>(value & 0xFF));
	return outs;
}
}
using namespace little_endian_io;

using namespace std;

class RawToWaveConverter {
public:
	RawToWaveConverter();
	virtual ~RawToWaveConverter();
	void convert(vector<short>, int, int, int, bool, const char*);
};

#endif /* SRC_RAWTOWAVCONVERTER_H_ */
