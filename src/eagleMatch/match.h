#pragma once
#include "cvaux.h"
#include "highgui.h"

class Ematch{
public:
	Ematch(IplImage * imageFirst, IplImage * imageSecond);
	~Ematch();

	void createTemplateImage();
	void showTemplateImage();
	void createMatchImage();

	void matchUp();
	void matchDown();
	IplImage* match(IplImage* templateImage, IplImage* originImage, bool isTopImage);

private:
	IplImage* m_imageMatch;
	IplImage* m_imageAsFirstFrame;
	IplImage* m_imageAsSecondFrame;
	IplImage* m_topImage;
	IplImage* m_downImage;
};