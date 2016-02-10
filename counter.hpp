#pragma once
#include <tuple>
#include <opencv.hpp>

using namespace std;
using namespace cv;

class Counter
{
public:
	Counter(Point& x, Point& y){
		X = x;
		Y = y;
		a = (X.y - Y.y)/(X.x - Y.x);
		b = X.y - a*X.x;
		in = 0;
		out = 0;
		stay = 0;
	}
	void setInitials(const vector<Point>& i) {
		initials = i;
	}
	void setCurrents(const vector<Point>& c) {
		currents = c;
	}
	
	tuple<int,int,int, vector<Point> > getSituation() {
		for(uint i = 0; i < initials.size(); i++){
			Point p = currents[i];
			if(p.y >= min(X.y, Y.y) && p.y <= max(X.y, Y.y)) {
				if(!isInside(initials[i]) && isInside(p)) {
					in ++;
					initials[i] = p;
				}
				else if(isInside(initials[i]) && !isInside(p)) {
					out ++;
					initials[i] = p;
				}
				else 
					stay ++;
			}
		}
		return tuple<int,int,int,vector<Point> >(out, in, stay, initials);
	}

private:
	Point X;
	Point Y;
	vector<Point> initials;
	vector<Point> currents;
	float a;
	float b;
	int in, out, stay;
	bool isInside(const Point& c) const {
		if(a > 0)
			return (a*c.x -c.y + b > 0);
		else
			return (a*c.x -c.y + b < 0);
	}
};

