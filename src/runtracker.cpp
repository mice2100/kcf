#include <iomanip> // for controlling float print precision
#include <sstream> // string to number conversion
#include <chrono>
#include <fstream>

#include <opencv2/core/core.hpp>       // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp> // Gaussian Blur
#include <opencv2/highgui/highgui.hpp> // OpenCV window I/O

#include "kcftracker.hpp"
#include "config.h"
#include "videoplayer.h"

using namespace std;
using namespace cv;
using namespace std::chrono;

int gInitialize = -1;
config cfg;

static void onMouse(int event, int x, int y, int, void* data)
{
    if(event != EVENT_LBUTTONDOWN)
	return;
    Rect* prc = (Rect*)data;

	prc->width = cfg.GetConfigInt("width");
	prc->height = cfg.GetConfigInt("height");
    prc->x = x - prc->width / 2;
    prc->y = y - prc->height / 2;

    gInitialize = 1;
}

float calcSpeed(bool startOver, Rect* rcLast, Rect* rcNow, float interval, float ratio, float* pDist=0)
{
	#define SPEEDCNT 3
	static float fdistance[SPEEDCNT];
	static float finterval[SPEEDCNT];
	static int indx=0;
	static bool full = false;
	if(startOver){
		for(int i=0; i<SPEEDCNT; i++){
			fdistance[i]=0; finterval[i]=0;
		}
		indx = 0;
		full = false;
		return 0;
	}
	
	float x1, y1, x2, y2;
	x1 = rcNow->x + rcNow->width/2.0;
	y1 = rcNow->y + rcNow->height/2.0;
	
	x2 = rcLast->x + rcLast->width/2.0;
	y2 = rcLast->y + rcLast->height/2.0;
	
	float dist = 0;
	dist = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))*ratio;
	if(pDist) *pDist = dist;
	
	fdistance[indx] = dist;
	finterval[indx] = interval;
	
	indx++;
	if(indx==SPEEDCNT){
		indx = 0;
		if(!full)
			full = true;
	}
	
	if( full ){
		dist = 0;
		float intv = 0;
		for(int i=0; i<SPEEDCNT; i++){
			dist += fdistance[i];
			intv += finterval[i];
		}
		
		return float(dist/intv)*ratio;
	}
	else
		return 0.0;
}

void OnTrackChange(int pos,void* data)
{
	VideoPlayer* pVP = (VideoPlayer*)data;
	if(pVP->GetCurrentFrame()!=pos)
		pVP->MoveToFrame(pos);
}

int main(int argc, char* argv[])
{
    stringstream conv;
    cfg.Init();

    int delay = cfg.GetConfigInt("normal");
    float ratio = cfg.GetConfigFlt("pixel2mm");
//    cout << "ratio:" << ratio << endl;
    int play = false;

    bool blog = cfg.GetConfigInt("log") == 1;
    string logfile = cfg.GetConfigStr("logfile");
    ofstream log;
    if(blog) {
		log.open(logfile);
		if(!log.is_open()) {
			cout << "Can't open log file: " << logfile << endl;
			return -1;
		}
		log << "x,y,dist,itvl" << endl;
    }

    //    VideoCapture captRefrnc(cfg.GetConfigInt("device"));
    //    VideoCapture captRefrnc(-1);
	VideoPlayer vp;

    if(!vp.LoadFile(cfg.GetConfigStr("videofile"))) {
		cout << "Could not open video file!" << cfg.GetConfigStr("videofile")<< endl;
		return -1;
    }

    Size refS = Size(vp.GetWidth(), vp.GetHeight());
    int frameNum = vp.GetFrameCount();
    int fps = vp.GetFPS();
    float timePerFrame = (float)1000 / (float)fps;
    int frame = 0;
    const char* WIN_RF = "VIDEO";
    const char* TRACK = "frame";

    Rect rc(10, 10, cfg.GetConfigInt("width"), cfg.GetConfigInt("height"));
    //	milliseconds frameTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    // Windows
    namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
    cvMoveWindow(WIN_RF, 0, 0);
    setMouseCallback(WIN_RF, onMouse, &rc);

	createTrackbar(TRACK, WIN_RF, &frame, frameNum, OnTrackChange, &vp);

	bool HOG = true;
    bool FIXEDWINDOW = false;
    bool MULTISCALE = true;
    bool SILENT = true;
    bool LAB = true;
    KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

    cout << "Resolution: Width=" << refS.width << "  Height=" << refS.height << " Frame Num: " << frameNum
         << " FPS: " << fps << endl;

    Mat* frameImage = vp.GetFramePtr();

    double maxSpeed = 0;
    //	captRefrnc.set(CV_CAP_PROP_POS_FRAMES, frame);
	if(!vp.MoveToFrame(frame)){
		cout<<"Can't read frame!"<<endl;
		return -1;
	}

	frame = vp.GetCurrentFrame();
	setTrackbarPos(TRACK, WIN_RF, frame);
	
    for(;;) // Show the image captured in the window and repeat
    {
		if(gInitialize == 1 && !frameImage->empty()) {
			tracker.init(rc, *frameImage);
			rectangle(*frameImage, rc, Scalar(0, 255, 255));

			gInitialize = 0;
			maxSpeed = 0;
			//				frameTime = duration_cast< milliseconds
			//>(system_clock::now().time_since_epoch());
			calcSpeed(true, NULL, NULL, 0, 0);
			if(blog) {
				log << rc.x << "," << rc.y << ",0,0" << endl;
			}
		} else if(gInitialize == 0 && play&& !frameImage->empty()) {
			Rect result = tracker.update(*frameImage);
			rectangle(*frameImage, result, Scalar(0, 255, 255));
			//				milliseconds frameTime2 = duration_cast< milliseconds
			//>(system_clock::now().time_since_epoch());
			//				milliseconds tt = frameTime2 - frameTime;
//			int xx = result.x - rc.x;
//			int yy = result.y - rc.y;
//			double dist = sqrt(xx * xx + yy * yy);
			float dist;
			char buf[200];
			float speed = calcSpeed(false, &result, &rc, timePerFrame, ratio, &dist);
			if(blog)
				log << rc.x << "," << rc.y << "," << dist<<","<<timePerFrame << endl;
//			double speed = ratio * dist / timePerFrame;
			//				cout<<"xx:"<<xx<<" yy:"<<yy<<" dist:"<<dist<<" ratio:"<<ratio<<endl;
			sprintf(buf, "Speed: %.3f, Max: %.3f", speed, maxSpeed);
			string strSpeed = buf;

			putText(*frameImage, strSpeed, Point(1, 50), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, Scalar::all(255), 1, 1);
			rc = result;
			//				frameTime = frameTime2;
			if(speed > maxSpeed)
				maxSpeed = speed;
		}
		
		if(!frameImage->empty())
			imshow(WIN_RF, *frameImage);
		if(play) {
			frame = vp.MoveToNextFrame();
			if(frameImage->empty()) {
				play = false;
				goto idle;
			}
			setTrackbarPos(TRACK, WIN_RF, frame);
		}
		
		idle:
		char c = (char)cvWaitKey(delay);
		if(c == 27)
			break;
		switch(c) {
		case 'n':
			delay = cfg.GetConfigInt("normal");
			break;
		case 's':
			delay = cfg.GetConfigInt("slow");
			break;
		case 'p':
			play = !play;
			break;
		}
    }

    if(log.is_open())
		log.close();

    return 0;
}
