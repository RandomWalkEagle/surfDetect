#include "cvaux.h"
#include "highgui.h"
#include "match.h"

int main()
{
	IplImage *imgAsFirstFrame = cvLoadImage("../imgs/YY66.png");
	IplImage *imgAsSecondFrame = cvLoadImage("../imgs/YY77.png");
	/*
	1¡¢find the top and down rect area (ag0: middle (8)24-adjacent connection , now 61*61)
			and save that as two images  --- TopImage + DownImage.
	*/
	Ematch match(imgAsFirstFrame, imgAsSecondFrame);
	match.createTemplateImage();

	/*
	2¡¢match TopImage and OrignImage
	*/
	match.matchUp();

	/*
	3¡¢match DownImage and OrignImage
	*/
	match.matchDown();

	//match.showTemplateImage();

	cvReleaseImage(&imgAsFirstFrame);
	cvReleaseImage(&imgAsSecondFrame);
	return 0;
}