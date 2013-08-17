#pragma once

/* This file is for data and functions are to be used individually, 
 * i.e. global data and standalone functions.
 */

#include <chrono>
#include <thread>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <opencv2/opencv.hpp>
using namespace cv;

namespace Gardeners {
	
	void GetCameraProperties(VideoCapture capture);
	vector<Mat> splitToChannels(Mat & input);
	void normaliseImage(Mat & input, Mat & output);
	void maskImage(Mat & frame, Mat & mask_bw, Mat & output);
	//void smoothGradients(Mat newRawRGB, Mat lastProcessedRGB, Mat newProcessedRGB, Mat output);
	void invertMask(Mat & input, Mat & output);
	void whiten_colours(Mat & input, Mat & output);
	void saveImage(const Mat & frame);
	
}