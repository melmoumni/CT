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
	LineController() {
		next_id = 1;
	}

	void addLine(Point a, Point b) {
		lines.push_back(Line(a, b, next_id));
		next_id++;
	}
	
	void removeLine(int id){
		int i;
		if((i = getLinePositionById(id)) != -1){
			lines.erase(lines.begin()+i);
			cout<<"Removed line "<<id<<" at position "<<i<<endl;
		} else
			cout<<"Line "<<id<<" not in lines collection"<<endl;
	}
	
	//as lines is sorted we can do a log search TODO
	int getLinePositionById(int id) {
		for(uint i = 0; i< lines.size(); i++)
			if(lines[i].getId() == id)
				return i;
		return -1;
	}
	
	void draw(){
		for(uint i = 0; i < lines.size(); i++) {
			int a = 255 - i * (255 / lines.size());
			drawLine(lines[i], Scalar(a,a,0));
		}
	}

	void setBackground (const Mat & m){
		currentBackground = m;
	}

	vector<Line>& getLines(void) {
		return lines;
	}
	
	Mat& frame() {
		return currentBackground;
	}
	
	int getNextId() {
		return next_id;
	}

private:
	void drawLine(const Line &l, Scalar color){
		line(currentBackground, l.getX(), l.getY(), color, 2, 8 );
	}
	int next_id;
	int click_nb;	
	vector<Line> lines;
	Mat currentBackground;
};
