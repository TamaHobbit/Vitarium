#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
using namespace cv;

#include <thread>
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

#include "values.hpp"
#include "utilities.hpp"
#include "BackGroundSubtract.hpp"
using namespace Gardeners;

/* Making it run fast:
 * increasing the threshold removes distractions without slowing down at all
 * increasing the size of the brush (be it eroding or dilating) slows it down less than increasing the passes
 * the total amount of passes should be no more than 5
 */

const char * window_title = "Gardeners";

//check the camera, then calibrate the reference image (upon "spacebar" press)
int main ( int argc, const char* argv[] ){
	VideoCapture capture(0);
	//cvCaptureFromCAM(int);
	if(!capture.isOpened()){  // check if we succeeded
		cerr << "Could not capture from camera." << endl;
		return -1;
	}
	
	namedWindow(window_title,1); //0 required for FULLSCREEN, 1 is normal (autosize)
	//setWindowProperty(window_title, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	
	Mat referenceImage;
	do{
		capture >> referenceImage;
		imshow(window_title, referenceImage);
	}while( waitKey(30) != 32 ); //spacebar
	
	BackGroundSubtract filter(capture, window_title);
	
	Mat newRawRGB(referenceImage.size(), referenceImage.type());
	Mat output(referenceImage.size(), referenceImage.type());
 	const Mat * displayImage = &output;
	
	CellularAutomaton game("000001100000001000");
	
	while(true)	{
		capture >> newRawRGB; // get a new frame from camera
		
		filter(newRawRGB, output);
		cvtColor( output, output, CV_BGR2GRAY );
		
		imshow(window_title, *displayImage);

		int keyPressed = waitKey(30);
		//high_resolution_clock::time_point previousTime = high_resolution_clock::now();
		if(keyPressed != -1){
			if(keyPressed == 27){ //escape
				break;
			} else if(keyPressed == 32){ //spacebar
				filter.updateReference();
			} else if(keyPressed == 115){ // 's'
				saveImage(*displayImage);
			} else if(keyPressed == 'q'){
				displayImage = &newRawRGB;
				cout << "Viewing input" << endl;
			} else if(keyPressed == 'w'){
				displayImage = &output;
				cout << "Viewing output" << endl;
			} else if(keyPressed == 'e'){
				filter.erode_size++;
				cout << "Erode size: " << filter.erode_size << endl;
			} else if(keyPressed == 'r' && filter.erode_size > 0){
				filter.erode_size--;
				cout << "Erode size: " << filter.erode_size << endl;
			} else if(keyPressed == 't'){
				filter.erode_passes++;
				cout << "Erode passes: " << filter.erode_passes << endl;
			} else if(keyPressed == 'y'){
				filter.erode_passes--;
				cout << "Erode passes: " << filter.erode_passes << endl;
			} else if(keyPressed == 'd'){
				filter.dilate_size++;
				cout << "Dilate size: " << filter.dilate_size << endl;
			} else if(keyPressed == 'f' && filter.dilate_size > 0){
				filter.dilate_size--;
				cout << "Dilate size: " << filter.dilate_size << endl;
			} else if(keyPressed == 'g'){
				filter.dilate_passes++;
				cout << "Dilate passes: " << filter.dilate_passes << endl;
			} else if(keyPressed == 'h'){
				filter.dilate_passes--;
				cout << "Dilate passes: " << filter.dilate_passes << endl;
			} else if(keyPressed == 44){// '<'
				filter.diff_threshold_value-=5;
				cout << "Threshold: " << filter.diff_threshold_value << endl;
			} else if(keyPressed == 46){// '>'
				filter.diff_threshold_value+=5;
				cout << "Threshold: " << filter.diff_threshold_value << endl;
			} else {
				cout << "No key binding for: " << keyPressed << endl;
			}
		}
		
// 		high_resolution_clock::time_point  now = high_resolution_clock::now();
// 		microseconds frame_ms = std::chrono::duration_cast<microseconds>(now - previousTime);
// 		//grab a new time point, so that only time outside of the above sleep is counted
// 		previousTime = high_resolution_clock::now();
// 		
// 		cout << "Delta t: "<< frame_ms.count() << endl;
		
	}
	
	cout << "Erode size: " << filter.erode_size << endl
			<< "Erode passes: " << filter.erode_passes << endl
			<< "Dilate size: " << filter.dilate_size << endl
			<< "Dilate repeat: " << filter.dilate_passes << endl;
	
 	return 0;
}