/*
 * RawToWaveConverter.cpp
 *
 *  Created on: 5 de out de 2019
 *      Author: dourado
 */

#include "RawToWavConverter.h"

RawToWaveConverter::RawToWaveConverter() {
	// TODO Auto-generated constructor stub

}

void RawToWaveConverter::convert(vector<short> data, int srate, int nsamp, int espeakerevt, bool isFname, const char *fname){

	if(espeakerevt != 2 && espeakerevt != 8)
		return;




	ofstream f(fname, ios::binary);



	// Write the file headers
	f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
	write_word( f,     16, 4 );  // no extension data
	write_word( f,      1, 2 );  // PCM - integer samples
	write_word( f,      1, 2 );  // two channels (stereo file)
	write_word( f,  nsamp, 4 );  // samples per second (Hz)
	write_word( f, nsamp*1*16/8, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
	write_word( f,      2, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
	write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

	// Write the data chunk header
	size_t data_chunk_pos = f.tellp();
	f << "data----";  // (chunk size to be filled in later)




	int N = data.size();  // total number of samples
	for (long n = 0; n < N; n++)
	{
		//cout << n << endl;
		write_word( f, data[n], 2 );
	}

	// (We'll need the final file size to fix the chunk sizes above)
	size_t file_length = f.tellp();

	// Fix the data chunk header to contain the data size
	f.seekp( data_chunk_pos + 4 );
	write_word( f, file_length - data_chunk_pos - 20);

	//cout << "Ae: " << file_length - data_chunk_pos + 8 << endl;

	// Fix the file header to contain the proper RIFF chunk size, which is (file size + 20) bytes
	f.seekp( 0 + 4 );
	write_word( f, file_length + 20, 4 );

	f.flush();
	f.sync_with_stdio(true);
	f.close();

}

RawToWaveConverter::~RawToWaveConverter() {
	// TODO Auto-generated destructor stub
}

