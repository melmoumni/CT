#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <opencv.hpp>

enum endpoint_t {START, END};
class Line
{
private:
	cv::Point X;
	cv::Point Y;
	char* _name;
	int id;

public:
	Line(cv::Point a, cv::Point b, char *n, int id_): X(a), Y(b), id(id_){
		strcpy(_name, n);
	}

	Line(cv::Point a, cv::Point b, int id_): X(a), Y(b), id(id_){
		//_name = "eaze";
	}

	cv::Point getX(void) const{
		return X;
	}
	cv::Point getY(void) const{
		return Y;
	}
	char* getName(void) const {
		return _name;
	}
	
	int getId(void) const{
		return id;
	}

	void setX(int _x, int _y){
		X.x = _x; X.y = _y;
	}
	void setY(int _x, int _y){
		Y.x = _x; Y.y = _y;
	}
	void setName(const char *name) {
		strcpy(_name, name);
	}
};
