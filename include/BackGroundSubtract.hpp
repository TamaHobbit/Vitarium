#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
using namespace cv;

#include "values.hpp"
#include "utilities.hpp"
#include "CellularAutomaton.hpp"

namespace Gardeners {
	
	class BackGroundSubtract {
	public:
		
		BackGroundSubtract(Mat reference);
		
		void updateReference(Mat reference);
		
		void smoothGradients(Mat newRawRGB, Mat lastProcessedRGB, Mat newProcessedRGB);
		void operator()(Mat & input, Mat & output);
		
		template<class Func>
		void channelWise(Mat & frame, Mat & output, Func f){
			vector<Mat> frame_split = splitToChannels(frame);
			for(int channel = 0; channel < 3; channel++){
				absdiff(frame_split.at(channel), reference_split.at(channel), frame_split.at(channel));
				threshold(frame_split.at(channel), frame_split.at(channel), diff_threshold_value, 255, CV_THRESH_BINARY);
			}
			merge(frame_split, output);
		}

		int erode_size, erode_passes;
		int dilate_size, dilate_passes;
		int diff_threshold_value;
	private:
		Mat referenceImage;
		vector<Mat> reference_split;
		
		void diff_and_thresh_channels(Mat & frame, Mat & person_mask);
		
	};
	
}
	
