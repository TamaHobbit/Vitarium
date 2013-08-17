#include <opencv2/opencv.hpp>
using namespace cv;

namespace Gardeners {
	
	double getPSNR(const Mat& I1, const Mat& I2);
	Scalar getMSSIM( const Mat& i1, const Mat& i2);
	
}