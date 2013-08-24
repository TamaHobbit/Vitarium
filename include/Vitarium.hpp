#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
using namespace cv;

class Vitarium {
public:
	Vitarium(VideoCapture cap, const char * title);
	void updateReference();
	void revolve(Mat & input);
	void getMovement(Mat & input, Mat & output);
	
	int movement_threshold;
	const static int FRAMECOUNT = 3;
	Mat frames[FRAMECOUNT];
	
	const char * const window_title;
	VideoCapture videoStream;
};