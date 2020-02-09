/*Multimodal, Customizable and Interactive Navigation System for Visually Impaired People 
Copyright (C) 2020 Antonio Miguel Batista Dourado

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/
/*
 * CaffeClassifier.h
 *
 *  Created on: 5 de out de 2016
 *      Author: antonio
 */



/* NOTA IMPORTANTE
 * Fazer o link simbólico de ~/caffe/include/caffe em /usr/local/include
 *
 * Fazer o link simbólido de ~/caffe/build/lib/\* em /usr/local/lib
 *
 * Ir no diretório de instalação do caffe e executar:
 * protoc src/caffe/proto/caffe.proto --cpp_out=.
 *	mkdir include/caffe/proto
 *	mv src/caffe/proto/caffe.pb.h include/caffe/proto
 *
 *	Include boost_system e caffe nas libs
 */

#ifndef SRC_CAFFECLASSIFIER_H_
#define SRC_CAFFECLASSIFIER_H_
#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace caffe;
using std::string;

typedef std::pair<string, float> Prediction;

class CaffeClassifier {
public:
	CaffeClassifier();
	//virtual ~CaffeClassifier();
	std::vector<Prediction> Classify(const cv::Mat& img, int N = 5);

private:
	void SetMean(const string& mean_file);
	std::vector<float> Predict(const cv::Mat& img);
	void WrapInputLayer(std::vector<cv::Mat>* input_channels);
	void Preprocess(const cv::Mat& img,std::vector<cv::Mat>* input_channels);
	boost::shared_ptr<Net> net_;
	cv::Size input_geometry_;
	int num_channels_;
	cv::Mat mean_;
	std::vector<string> labels_;
};

#endif /* SRC_CAFFECLASSIFIER_H_ */
