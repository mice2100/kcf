#ifndef __VIDEOPLAYER_H__
#define __VIDEOPLAYER_H__

#include <opencv2/core/core.hpp>       // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp> // Gaussian Blur
#include <opencv2/highgui/highgui.hpp> // OpenCV window I/O

using namespace std;
using namespace cv;

class VideoPlayer
{
public:
	VideoPlayer() {};
	~VideoPlayer() {Close(); };
	
public:
	bool	LoadFile(string videofile) { return m_VC.open(videofile); };
	void	Close() { return m_VC.release(); };
	bool	IsOpened() { return m_VC.isOpened(); };
	int		GetWidth() { return (int)m_VC.get(CV_CAP_PROP_FRAME_WIDTH); };
	int		GetHeight() { return (int)m_VC.get(CV_CAP_PROP_FRAME_HEIGHT); };
	int		GetFrameCount() { return (int)m_VC.get(CV_CAP_PROP_FRAME_COUNT); };
	int		GetCurrentFrame() { return (int)m_VC.get(CV_CAP_PROP_POS_FRAMES)-1; };
	int		GetFPS() { return (int)m_VC.get(CV_CAP_PROP_FPS); };
	
	bool	MoveToFrame(int frame);
	int		MoveToNextFrame();
	Mat*	GetFramePtr(){ return &m_image; };
	
private:
	VideoCapture	m_VC;
	Mat				m_image;
};

#endif
