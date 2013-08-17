#include "CellularAutomaton.hpp"

CellularAutomaton::CellularAutomaton(bitset<18> rule) : m_rule(rule){
}

CellularAutomaton::CellularAutomaton(const char * rule){
	unsigned long setbit = 1 << 17;
	m_rule = 0;
	for(int i = 0; i < 18; i++){
		if(rule[i] == '\0'){
			cout << "Bitstring too short. You must specify an 18 bit string." << endl;
			exit(1);
		}
		if( rule[i] == '1' ){
			m_rule |= setbit;
		}
		setbit = setbit >> 1;
	}
}

void CellularAutomaton::setMat(Mat & seed){
	m_data = seed;//not a real copy! Just point to seed
}

void CellularAutomaton::setRand(int width, int height, int millentage){
	m_data.create(height, width, CV_8U);
	if(millentage == 1000){ //because cv::threshold cannot accept all pixel values
		m_data = Mat::ones(width, height, m_data.type())*255;
	} else {
		millentage /= 4;
		//get a random grayscale randomly distributed in range (0, 100>
		randu(m_data, Scalar(0), Scalar(250));
		//anything higher than the threshold is live, so the higher the live_percentage, the
		//more pixels should pass this test, therefore the lower the threshold
		threshold(m_data, m_data, 249-millentage, 255, THRESH_BINARY);
	}
}

void CellularAutomaton::timestep(){
	threshold(m_data, m_data, 100, 1, THRESH_BINARY); //turn 255 -> 1
	
	//sum neighbours
	filter2D(m_data, m_data, ddepth, kernel, anchor, delta, BORDER_REPLICATE );//BORDER_CONSTANT );
	
	for(MatIterator_<uchar> it = m_data.begin<uchar>(); it != m_data.end<uchar>(); ++it){
		uchar & pixVal = *it;
		pixVal = m_rule[pixVal]*255;
	}
}

//returns true if something changed
bool CellularAutomaton::timestep_checkChange(){
	Mat copy = m_data.clone();
	timestep();
	return cv::norm(copy,m_data)!=0;
}

Mat CellularAutomaton::getMatrix(){
	return m_data;
}