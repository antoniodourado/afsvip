/*
 * CaffeClassifier.h
 *
 *  Created on: 5 de out de 2019
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
