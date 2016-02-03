#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <opencv.hpp>

using namespace cv;
using namespace std;

class LineController
{
public:
	
	void addLigne(Ligne l) {
		lignes.push_back(l);
	}
	void addLigne(Point a, Point b) {
		lignes.push_back(Ligne(a, b, "toto"));
	}

	// void removeLine(Ligne l){
	// 	for(std::vector<Ligne>::iterator it = lignes.begin(); it != l; it++);
	// 	lignes.erase(it);
	// }

	void mouseEventCallBack(int event, int _x, int _y, int flags, void* userdata) {
		if  ( event == EVENT_LBUTTONDOWN ){
			if(click_nb%2 == 0) {
				X.x = _x;
				X.y = _y;
			} else {
				Y.y = _y;
				Y.x = _x;
			}
			click_nb++;
		}
		else if ( event == EVENT_MOUSEMOVE ) {
			frame_orig.copyTo(frame_copy);
			current_mouse_pos.x = _x;
			current_mouse_pos.y = _y;
		}
	}

	void drawOptFlowMap (int step, const Scalar& color) {  
		for(int y = 0; y < cflowmap.rows; y += step)  
			for(int x = 0; x < cflowmap.cols; x += step) {  
				const Point2f& fxy = flow.at< Point2f>(y, x);  
				line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), color);  
				circle(cflowmap, Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), 1, color, -1);  
			}  
	}

	VideoCapture cap;
	vector<Ligne> lignes(){
		return _lignes;
	}
	
private:
	int click_nb;
	vector<Ligne> _lignes;
	Point X;
	Point Y;
	Point current_mouse_pos;
	Mat frame_orig;
	Mat frame_copy;
	Mat flow;
	Mat cflowmap;
};
