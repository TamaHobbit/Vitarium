#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
using namespace cv;

#include <thread>
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

#include "Gardeners.hpp"
#include "CellularAutomaton.hpp"

/* Making it run fast:
 * increasing the threshold removes distractions without slowing down at all
 * increasing the size of the brush (be it eroding or dilating) slows it down less than increasing the passes
 * the total amount of passes should be no more than 5
 */

const char * window_title = "Gardeners";

void saveImage(const Mat & frame);

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
	
	Gardeners artwork(capture, window_title);
	
	Mat referenceImage;
	do{
		capture >> referenceImage;
		imshow(window_title, referenceImage);
	}while( waitKey(30) != 32 ); //spacebar
	
	const int NUMSEQ = 5; 
	assert(NUMSEQ > 0 && NUMSEQ < 10);
	Mat * imageSequence[NUMSEQ];
	for(int i = 0; i < NUMSEQ-1; i++){
		imageSequence[i] = new Mat(referenceImage.size(), referenceImage.type());
	}
	
	//Life: 000001100000001000
	//Seeds: 000000000000000100
	//Whiteweb: 100001100111000000
	//Fader: 100100000111000000
	//Anthairfade: 000111000001110000
	//Anthillspread: 000111000001111000
	//Spaceships: 001000100110000100
	
	CellularAutomaton game("000001100000001000");
	imageSequence[NUMSEQ-1] = &game.m_data;
	game.setRand( referenceImage.cols, referenceImage.rows, 0 );
	const Mat * displayImage = imageSequence[NUMSEQ-1];
	
	while(true){
		capture >> *imageSequence[0]; // get a new frame from camera
		
		cvtColor( *imageSequence[0], *imageSequence[1], CV_BGR2GRAY );
		artwork.getMovement( *imageSequence[1], *imageSequence[2] );
		artwork.revolve( *imageSequence[1] ); //store new frame and shift others
		//cvtColor( *imageSequence[1], *imageSequence[2], CV_BGR2GRAY );
		threshold( *imageSequence[2], *imageSequence[3], artwork.movement_threshold, 255, CV_THRESH_BINARY);
		
		bitwise_or( *imageSequence[3], *imageSequence[4], *imageSequence[4]);
		game.timestep();
		
		imshow(window_title, *displayImage);

		int keyPressed = waitKey(30);
		//high_resolution_clock::time_point previousTime = high_resolution_clock::now();
		if(keyPressed != -1){
			if(keyPressed == 27){ //escape
				break;
			} else if(keyPressed == 32){ //spacebar
				artwork.updateReference();
				game.setRand( referenceImage.cols, referenceImage.rows, 0 );
			} else if(keyPressed == 115){ // 's'
				saveImage(*displayImage);
			} else if(keyPressed == 'q'){
				displayImage = imageSequence[0];
				cout << "Viewing input" << endl;
			} else if(keyPressed == 'w'){
				displayImage = imageSequence[NUMSEQ-1];
				cout << "Viewing output" << endl;
			} else if(keyPressed >= '1' && keyPressed <= '9'){
				const int index = keyPressed-'1';
				if(index >= NUMSEQ){
					cout << "Only the sequence from '1' to '" << (char)(NUMSEQ-1+'1') << "' is in use." << endl;
					continue;
				}
				displayImage = imageSequence[keyPressed-'1'];
				cout << "Viewing image " << (char)keyPressed << endl;
			}/* else if(keyPressed == 'e'){
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
			} */else {
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
	
// 	cout << "Erode size: " << filter.erode_size << endl
// 			<< "Erode passes: " << filter.erode_passes << endl
// 			<< "Dilate size: " << filter.dilate_size << endl
// 			<< "Dilate repeat: " << filter.dilate_passes << endl;
	
			
	for(int i = 0; i < NUMSEQ-1; i++){
		delete imageSequence[i];
	}
 	return 0;
}

void saveImage(const Mat & frame){
	string imageName;
	std::cin >> imageName;
	imageName = string("../images/") + imageName + string(".jpg");
	cout << "Writing frame to " + imageName << endl;
	imwrite( imageName, frame );
}