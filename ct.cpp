#include <opencv2/video/background_segm.hpp>
#include <tuple>
#include "line.hpp"
#include "lineController.hpp"
#include "patch.hpp"
#include "objectdetect.hpp"
#include "counter.hpp"
#include "CppMT/CMT.cpp"

 
using namespace std;
using namespace cv;
using namespace cmt;

/** Global variables **/
LineController controller;
//Draw on frame feature
vector<Counter> counters;
int click_nb = 0;
Point X;
Point Y;
Point current_mouse_pos;
bool destroy_window = false;
bool destroyed = false;
Mat frame;
//link the button id with its property (state)
map<int, int> buttons_id;
int new_button = 0;
String window_name = "Capture - logo atos detection";
/** End global variables **/

int display(Mat im, CMT & cmt) {
	//Visualize the output. It is ok to draw on im itself, as CMT only uses the grayscale image
	for(size_t i = 0; i < cmt.points_active.size(); i++)
		circle(im, cmt.points_active[i], 2, Scalar(255,0,0));

	Point2f vertices[4];
	cmt.bb_rot.points(vertices);
	for (int i = 0; i < 4; i++)
		line(im, vertices[i], vertices[(i+1)%4], Scalar(255,0,0));
	imshow(window_name, im);
	return waitKey(5);
}

void on_change(int , void *id) {
	int position = controller.getLinePositionById(*(int*)id);
	if(position < 0 || position >= (int)counters.size())
		asm("int3");
	controller.removeLine(*(int*)id);
	counters.erase(counters.begin()+position);
	destroy_window = true;
}
	
void mouseEventCallBack(int event, int _x, int _y, int , void* ) {
	if(event == EVENT_LBUTTONDOWN ){
		if(click_nb%2 == 0) {
			X.x = _x;
			X.y = _y;
		} else {
			Y.y = _y;
			Y.x = _x;
			controller.addLine(X,Y);
			counters.push_back(Counter(X,Y));
			controller.draw();
			imshow(window_name, controller.frame());
			int this_id = controller.getNextId() - 1;
			buttons_id[new_button] = this_id;
			createButton(patch::to_string(this_id), on_change, &buttons_id[new_button++], CV_CHECKBOX);
		}
		click_nb++;
	}
}

void process(Mat& frame){
	vector<Line> lines;
	bool destroyed = false;
	setMouseCallback(window_name, mouseEventCallBack, NULL);
		
	if(destroy_window) {
		destroy_window = false;
		destroyed = true;
		destroyWindow(window_name);
	}
		
	controller.setBackground(frame);
	controller.draw();
	imshow(window_name, frame);
		
	if(destroyed) {
		lines = controller.getLines();
		for(uint i = 0; i < lines.size(); ++i){
			int this_id = lines[i].getId();
			buttons_id[new_button] = this_id;
			createButton(patch::to_string(this_id), on_change, &buttons_id[new_button++], CV_CHECKBOX);
		}
		setMouseCallback(window_name, mouseEventCallBack, NULL);
		destroyed = false;
			
		while(waitKey(30) < 0);
	}
}


void usage(const char *s){
  cerr<<"Usage: "<<s<<" xml"<<endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	/**
	 * int : lifetime of tracker (number of frames)
	 * Point : initial position of the CMT
	 **/
 	vector< tuple<CMT*,int, Point> > cmt_list;
	CMT* cmt;
	CascadeClassifier logo_cascade;
	String logo_cascade_name;
	VideoCapture capture;
	Mat frame;
	const int nb_frame_threshold = 10;

	if(argc < 2) {
		usage(argv[0]);
		return 0;
	}
	namedWindow(window_name);
	FILELog::ReportingLevel() = logINFO;
	DetectAndDisplay d(window_name);
	logo_cascade_name = argv[1];

  //-- 1. Load the cascades
	if( !logo_cascade.load( logo_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
	
	//-- Get video
	capture.open( -1 );
	if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }
	// capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	// capture.set(CV_CAP_PROP_FRAME_HEIGHT, 320);

	//process images
	while(capture.read(frame)) {
		if( frame.empty() ){
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		frame.copyTo(frame);
		Mat im_gray;
		if (frame.channels() > 1) {
			cvtColor(frame, im_gray, CV_BGR2GRAY);
		} else {
			im_gray = frame;
		}
		vector<Rect> logos;
		//-- 3. Apply the classifier to the frame
		d.detectAndMark(frame, logo_cascade, logos);

		//detect and track new objects
		for(uint i= 0; i<logos.size(); i++){ 
			Point2f a(logos[i].x,logos[i].y);
			bool match = true;
			for(uint j = 0; j < cmt_list.size(); j++){
				Point2f b(std::get<0>(cmt_list[j])->bb_rot.center);
				double res = cv::norm(cv::Mat(a),cv::Mat(b));
				double sizee = sqrt(logos[i].width*logos[i].width + logos[i].height*logos[i].height);
				if(res < sizee){
					std::get<1>(cmt_list[j]) = nb_frame_threshold;
					match = false;
					break;
				} 
			}
			if(match || cmt_list.size() == 0) {
				cmt = new CMT();
				cmt->initialize(im_gray, logos[i]);
				cmt_list.push_back(tuple<CMT*,int, Point>(cmt,nb_frame_threshold,cmt->bb_rot.center));
			}
		}
				
		//dont track an object that has not been detected for a long time
		for(uint i = 0; i<cmt_list.size(); i++){
			Point2f b(std::get<0>(cmt_list[i])->bb_rot.center);
			for(uint j = 0; j<logos.size(); j++) {
				Point2f a(logos[j].x,logos[j].y);
				RotatedRect r = std::get<0>(cmt_list[i])->bb_rot;
				double res = cv::norm(cv::Mat(a),cv::Mat(b));
				double sizee = sqrt(r.size.width * r.size.width + r.size.height * r.size.height);
				if(res<sizee){
					std::get<1>(cmt_list[i])++;
					break;
				}
			}
			std::get<1>(cmt_list[i])--;
			if(std::get<1>(cmt_list[i]) <= 0) {
				cmt_list.erase(cmt_list.begin()+i);
				if(i>0)
					--i;
			}
			for(uint j = 0; j < cmt_list.size() && j!=i; j++){
				Point2f a(std::get<0>(cmt_list[j])->bb_rot.center);
				RotatedRect r = std::get<0>(cmt_list[j])->bb_rot;
				double res = cv::norm(cv::Mat(a),cv::Mat(b));
				double sizee = sqrt(r.size.width * r.size.width + r.size.height * r.size.height);
				if(res<sizee){
					cmt_list.erase(cmt_list.begin()+j);
					break;
				}
			}
		}
		
		d.displayObject(logos, frame);
		for(uint i = 0; i<cmt_list.size(); i++) {
			std::get<0>(cmt_list[i])->processFrame(im_gray);
			char key = display(frame, *(std::get<0>(cmt_list[i])));
			if(key == 'q') break;
		}
		process(frame);

		vector<Point> initials;
		vector<Point> currents;
		for(uint i = 0; i < cmt_list.size(); i++) {
			initials.push_back(std::get<2>(cmt_list[i]));
			currents.push_back(std::get<0>(cmt_list[i])->bb_rot.center);
		}
		
		for(uint i = 0; i < counters.size(); i++) {
			counters[i].setInitials(initials);
			counters[i].setCurrents(currents);
		}
		
		for(uint i = 0; i < counters.size(); i++) {
			tuple<int,int,int, vector<Point> > tmp = counters[i].getSituation();
			putText(frame, to_string(std::get<0>(tmp)) + to_string(std::get<1>(tmp)) + to_string(std::get<2>(tmp)), Point(5,15*i+15), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,0));
			
			imshow(window_name, frame);
		}
		waitKey(0);
	}
	return EXIT_SUCCESS;
}
