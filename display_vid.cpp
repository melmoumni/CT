#include <opencv2/video/background_segm.hpp>
#include "line.hpp"
#include "lineController.hpp"
#include "patch.hpp"

using namespace std;
using namespace cv;

const char* WINDOW_NAME = "Video window";
int click_nb = 0;
Point X;
Point Y;
Point current_mouse_pos;
LineController controller;

void on_change(int i, void *j){
	cout<<"ttoto"<<endl;
}



void mouseEventCallBack(int event, int _x, int _y, int flags, void* userd) {
	if  ( event == EVENT_LBUTTONDOWN ){
		if(click_nb%2 == 0) {
			X.x = _x;
			X.y = _y;
		} else {
			Y.y = _y;
			Y.x = _x;
			controller.addLine(X,Y);
		}
		click_nb++;
	}
	
}

void process(const char* videoname){
	Mat frame;
	Mat last_frame;
  bool first = true;
	vector<Line> lines;
  VideoCapture cap(videoname);
  if(!cap.isOpened())
    exit(EXIT_FAILURE);
	
	namedWindow(WINDOW_NAME, 1);
	setMouseCallback(WINDOW_NAME, mouseEventCallBack, NULL);

  while(cap.read(frame)) {
		lines = controller.getLines();
		for(int i = 0; i < lines.size(); ++i){
			cout<<"Button "<<lines[i].getId()<<endl;
			createButton(patch::to_string(lines[i].getId()), on_change, NULL, CV_RADIOBOX);
		}
    if(first) {
      frame.copyTo(last_frame);
      first = false;
    }
		controller.setBackground(frame);
		controller.draw();

    imshow(WINDOW_NAME, frame);
    while(waitKey(30) < 0);
  }
}

void usage(const char *s){
  cerr<<"Usage: "<<s<<" videoName"<<endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
  if(argc != 2)
    usage(argv[0]);
  process(argv[1]);

  return EXIT_SUCCESS;
}
