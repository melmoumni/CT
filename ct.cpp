#include <opencv2/video/background_segm.hpp>
#include "line.hpp"
#include "lineController.hpp"
#include "patch.hpp"
#include "objectdetect.hpp"
#include "draw.hpp"

using namespace std;
using namespace cv;

/** Global variables **/
LineController controller;
//Draw on frame feature
int click_nb = 0;
Point X;
Point Y;
Point current_mouse_pos;
bool destroy_window = false;;
bool destroyed = false;;
Mat frame;
//link the button id with its property (state)
map<int, int> buttons_id;
int new_button = 0;
String window_name = "Capture - logo atos detection";
/** End global variables **/

void on_change(int , void *id) {
	controller.removeLine(*(int*)id);
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
	CascadeClassifier logo_cascade;
	String logo_cascade_name;
	VideoCapture capture;
	Mat frame;
	vector<Rect> logos;
	if(argc < 2) {
		usage(argv[0]);
		return 0;
	}
	namedWindow(window_name);
	DetectAndDisplay d(window_name);
	logo_cascade_name = argv[1];
  //-- 1. Load the cascades
	if( !logo_cascade.load( logo_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
	
	//-- Get video
	capture.open( -1 );
	if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }

	//process images
	while(capture.read(frame)) {
		if( frame.empty() ){
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		process(frame);
		//-- 3. Apply the classifier to the frame
		logos = d.detectAndMark(frame, logo_cascade, logos);
		cout<<"Nubmer of logos: "<<logos.size()<<endl;
		d.displayObject(logos, frame);
		waitKey(0);
	}
	return EXIT_SUCCESS;
}
