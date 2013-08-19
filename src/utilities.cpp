#include "utilities.hpp"

namespace Gardeners {
	
	void GetCameraProperties(VideoCapture capture){
		int width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

		cout << "Camera properties\n";
		cout << "width = " << width << endl <<"height = "<< height << endl;
	}
	
	vector<Mat> splitToChannels(Mat & input){
		vector<Mat> output;
		output.resize(input.channels());
		split(input, output);
		return output;
	}

	//set luminescence to 128
	void normaliseImage(Mat & input, Mat & output){
		cvtColor(input, output, CV_BGR2HSV);
		const int from_to[] = {0,2};
		//make a single set channel of the right size
		Mat empty = Mat::ones(output.rows, output.cols, input.type()) * 128;
		//copy said channel into frame according to "from_to"
		mixChannels(&empty,1,&output,1,from_to,1);
		cvtColor(output, output, CV_HSV2BGR);
	}
	
	//split the frame, mask each channel, merge back
	void maskImage(Mat & frame, Mat & mask_bw, Mat & output){
		vector<Mat> frame_split = splitToChannels(frame);
		for(int channel = 0; channel < frame.channels(); channel++){
			bitwise_and(frame_split.at(channel), mask_bw, frame_split.at(channel));
		}
		
		output.create(frame.rows, frame.cols, CV_8UC3);
		merge(frame_split, output);
	}
	
	void invertMask(Mat & input, Mat & output){
		Mat sub_mat = Mat::ones(input.size(), input.type())*255;
		subtract(sub_mat, input, output);
	}
	
// 	void smoothGradients(Mat newRawRGB, Mat lastProcessedRGB, Mat newProcessedRGB, Mat output){
// 		for(int x = 0; x < newRawRGB.cols; x++){
// 			for(int y = 0; y < newRawRGB.rows; y++){
// 				cv::Vec3b & newRawRGB_pixel = newRawRGB.at<cv::Vec3b>(y,x);
// 				cv::Vec3b & lastProcessedRGB_pixel = lastProcessedRGB.at<cv::Vec3b>(y,x);
// 				cv::Vec3b & newProcessedRGB_pixel = newProcessedRGB.at<cv::Vec3b>(y,x);
// 				cv::Vec3b & output_pixel = output.at<cv::Vec3b>(y,x);
// 				//for every pixel:
// 				unsigned long diff = 0;
// 				for(int i = 0; i < 3; i++){
// 					diff += newRawRGB_pixel[i]>lastProcessedRGB_pixel[i]?newRawRGB_pixel[i]-lastProcessedRGB_pixel[i]:\
// 																																					lastProcessedRGB_pixel[i]-newRawRGB_pixel[i];
// 				}
// 				
// 				if( diff > diff_threshold_value ){
// 					newProcessedRGB_pixel = newRawRGB_pixel;
// 					output_pixel = newProcessedRGB_pixel;
// 				} else {
// 					newProcessedRGB_pixel = lastProcessedRGB_pixel/2 + newRawRGB_pixel/2;
// 					output_pixel = cv::Vec3b(0, 0, 0);
// 				}
// 			}
// 		}
// 		lastProcessedRGB = newProcessedRGB.clone();
// 	}
	
	//desaturates an image in HSV format
	void desaturate(Mat & image){
		const int from_to[] = {0,1};
		//make a single set channel of the right size
		Mat constant_val = Mat::ones(image.size(), image.type())*0;
		//copy said channel into frame according to "from_to"
		mixChannels(&constant_val,1,&image,1,from_to,1);
	}
	
	void whiten_colours(Mat & input, Mat & output){
		cvtColor(input, output, CV_BGR2GRAY);
		threshold(output, output, 20, 255, THRESH_BINARY);
	}
}