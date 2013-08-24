#include "Vitarium.hpp"

Vitarium::Vitarium(VideoCapture cap, const char * title) : window_title(title), videoStream(cap){
	movement_threshold = 15;
	updateReference();
}

void Vitarium::updateReference(){
	for(int i = 0; i < FRAMECOUNT; i++){
		videoStream >> frames[i];
		cvtColor( frames[i], frames[i], CV_BGR2GRAY );
		waitKey(30);
	}
}

void Vitarium::revolve(Mat & input){
	frames[0] = frames[1].clone();
	frames[1] = frames[2].clone();
	frames[2] = input.clone();
}

void Vitarium::getMovement(Mat & input, Mat & output){
	Mat d1(frames[0].size(), frames[0].type());
	Mat d2(frames[0].size(), frames[0].type());
	absdiff(frames[2], frames[1], d1);
	absdiff(frames[1], frames[0], d2);
	bitwise_xor(d1, d2, output);
// 	absdiff(frames[2], input, output);
}