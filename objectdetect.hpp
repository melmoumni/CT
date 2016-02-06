#pragma once 
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <opencv.hpp>

class DetectAndDisplay {
public:
	DetectAndDisplay(String s): window_name(s) {}
	/** @function detectAndDisplay */
	vector<Rect>& detectAndMark( Mat frame, CascadeClassifier& logo_cascade, vector<Rect>& logos )
	{
		Mat frame_gray;
		Mat ROI;
		cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
		equalizeHist( frame_gray, frame_gray );	
		//-- Detect logos
		logo_cascade.detectMultiScale( frame_gray, logos, 1.05, 1, 0|CASCADE_SCALE_IMAGE);
		return logos;
	}
	void displayObject(vector<Rect> & logos, Mat &frame) {
		for(uint i = 0; i<logos.size(); i++){
			Point center( logos[i].x + logos[i].width/2, logos[i].y + logos[i].height/2 );
			circle(frame, center, 5, Scalar( 255, 0, 255 ), 4, 8, 0); 
		}
		imshow(window_name, frame);
	}

private:
	String window_name;
};
