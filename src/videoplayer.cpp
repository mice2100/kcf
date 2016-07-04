#include "videoplayer.h"
#include <iostream>

bool	VideoPlayer::MoveToFrame(int frame)
{
	m_VC.set(CV_CAP_PROP_POS_FRAMES, frame);
	m_VC >> m_image;
//	cout<<"Frame: "<<GetCurrentFrame()<<endl;
	return !m_image.empty();
}

int		VideoPlayer::MoveToNextFrame()
{
	m_VC >> m_image;
//	cout<<"Frame: "<<GetCurrentFrame()<<endl;
	return GetCurrentFrame();
}
