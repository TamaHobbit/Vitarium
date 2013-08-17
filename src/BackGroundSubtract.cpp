#include "BackGroundSubtract.hpp"

namespace Gardeners {
	
	BackGroundSubtract::BackGroundSubtract(VideoCapture cap, const char * title) : window_title(title), videoStream(cap){
		updateReference();
		//lower = more crisp, higher = less noise. Anywhere between 40 and 70 is good
		//this is the most efficient way of reducing distractions (noise outside person)
		diff_threshold_value = 60;  

		//1, 1, 5, 2 thresh 60
		erode_size = 1; erode_passes = 1;
		dilate_size = 5; dilate_passes = 2;
	}
	
	void BackGroundSubtract::updateReference(){
		for(int i = 0; i < FRAMECOUNT; i++){
			videoStream >> frames[i];
			waitKey(30);
		}
	}
	
	void BackGroundSubtract::operator()(/*Mat & input, */Mat & output){
		Mat d1(frames[0].size(), frames[0].type());
		Mat d2(frames[0].size(), frames[0].type());
		absdiff(frames[2], frames[1], d1);
		absdiff(frames[1], frames[0], d2);
		bitwise_xor(d1, d2, output);
		//threshold(output, output, 140, 255, CV_THRESH_BINARY);
		
		//maskImage(input, output, output);
		
		frames[0] = frames[1].clone();
		frames[1] = frames[2].clone();
		videoStream >> frames[2];
		
		return;
		
// 		//from http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
// 		cvtColor( input, output, CV_BGR2GRAY );
// 		GaussianBlur( output, output, Size(5,5), 0, 0, BORDER_DEFAULT );
// 		
// 		int scale = 1;
// 		int delta = 100;
// 		int ddepth = CV_16S;
// 		
// 		/// Generate grad_x and grad_y
// 		Mat grad_x, grad_y;
// 		Mat abs_grad_x, abs_grad_y;
// 
// 		/// Gradient X
// 		//Scharr( output, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
// 		Sobel( output, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
// 		convertScaleAbs( grad_x, abs_grad_x );
// 
// 		/// Gradient Y
// 		//Scharr( output, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
// 		Sobel( output, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
// 		convertScaleAbs( grad_y, abs_grad_y );
// 
// 		/// Total Gradient (approximate)
// 		addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, output );
// 		
// 		//input = output.clone();
// 		return;
// 		
// 		//create a mask that is white where the people are
// 		Mat person_mask;
// 		diff_and_thresh_channels(input, person_mask);
// 		whiten_colours(person_mask, person_mask);
// 		
// 		//filter noise outside the person
// 		Mat brush = getStructuringElement( MORPH_RECT, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );
// 		for(int i = 0; i < erode_passes; i++){
// 			erode(person_mask, person_mask, brush);
// 		}
// 		
// 		//fill in gaps inside the person
// 		if( dilate_size != erode_size ){
// 			brush = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
// 		}
// 		for(int i = 0; i < dilate_passes; i++){
// 			dilate(person_mask, person_mask, brush);
// 		}
// 		
// 		//apply the mask
// 		maskImage(input, person_mask, output);
	}
	
	void BackGroundSubtract::diff_and_thresh_channels(Mat & frame, Mat & output){
// 		output.create(frame.rows, frame.cols, CV_8UC3);
// 		absdiff(frame, referenceImage, output);
// 		threshold(output, output, diff_threshold_value, 255, CV_THRESH_BINARY);
		
		vector<Mat> frame_split = splitToChannels(frame);
				
		for(int channel = 0; channel < 3; channel++){
			absdiff(frame_split.at(channel), reference_split.at(channel), frame_split.at(channel));
			threshold(frame_split.at(channel), frame_split.at(channel), diff_threshold_value, 255, CV_THRESH_BINARY);
		}
		
		output.create(frame.rows, frame.cols, CV_8UC3);
		merge(frame_split, output);
	}
}