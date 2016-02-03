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
	
	void addLine(Point a, Point b) {
		if(lines.size() > 0)
			lines.push_back(Line(a, b, lines[lines.size() - 1].getId() + 1));
		else
			lines.push_back(Line(a, b, 1));
	}

	void removeLine(int id){
		for(uint i = 0; i< lines.size(); i++)
			if(lines[i].getId() == id) {
				lines.erase(lines.begin()+i);
				break;
			}
	}

	void draw(){
		for(uint i= 0; i < lines.size(); i++) {
			int a = 255 - i * (255 / lines.size());
			drawLine(lines[i], Scalar(a,a/2,a/4));
		}
	}

	void setBackground (const Mat & m){
		currentBackground = m;
	}

	vector<Line>& getLines(void) {
		return lines;
	}
		
private:
	void drawLine(const Line &l, Scalar color){
		line(currentBackground, l.getX(), l.getY(), color, 2, 8 );
	}
	int click_nb;	
	vector<Line> lines;
	Mat currentBackground;
	Mat frame_orig;
	Mat frame_copy;
	Mat flow;
	Mat cflowmap;
};
