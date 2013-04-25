#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <vector>

#include "cvfast.h"
#include "match.h"
#include "corner.h"
#include "drawCorner.h"

const float KSCALE = 0.125*2;//change
const int KFASTTHRESHHOLD = 180;//��Χ[0-250]
const int KNPIXELS = 11;
const char fileOne[] = "../imgs/YY102.png";
const char fileTwo[] = "../imgs/YY103.png";

IplImage *scaleImage(IplImage* image, float scale = KSCALE)
{
	//calTime cal(std::string(__FUNCTION__));
	CvSize size = cvGetSize( image );
	size.width = (int)(size.width * scale);
	size.height = (int)(size.height * scale);
	IplImage *imageAfterScale= cvCreateImage(size, image->depth, image->nChannels);
	cvResize(image, imageAfterScale, CV_INTER_LINEAR);
	return imageAfterScale;
}

void showImage(IplImage* image)
{
	cvNamedWindow("FAST", CV_WINDOW_AUTOSIZE); 
	cvShowImage("FAST", image);
}

void geneCorners(CvPoint* cornersPos, int numCorners, Corners& corners)
{
	calTime cal(std::string(__FUNCTION__));
	for (int i =0; i < numCorners; ++i)
	{
		cornersPos[i].x *= (int)(1.0/KSCALE);
		cornersPos[i].y *= (int)(1.0/KSCALE);
		corners.pushCorner(cornersPos[i]);
	}
}

void processImage(IplImage* I_inOrigin,Corners& corners)
{
	int inNonMaxSuppression  = 0;
	CvPoint* cornersPos;
	 int numCorners;

	IplImage *I_inScale = scaleImage(I_inOrigin);

	IplImage* IGray = cvCreateImage( cvGetSize( I_inScale ), IPL_DEPTH_8U, 1);
	cvCvtColor( I_inScale, IGray, CV_RGB2GRAY );

	cvCornerFast(IGray, KFASTTHRESHHOLD, KNPIXELS, inNonMaxSuppression, &numCorners, &cornersPos);
	std::cout << "------->number of corners found " << " are " << numCorners << "\n\n";

	geneCorners(cornersPos, numCorners, corners);

	cvReleaseImage(&IGray);
	cvReleaseImage(&I_inScale);
}

void mergeImage(IplImage* image1, IplImage* image2)
{
	cv::Mat leftImage(image1);
	cv::Mat rightImage(image2);
	int bias = 20;

	int row = leftImage.rows > rightImage.rows ? leftImage.rows : rightImage.rows;
	int col = leftImage.cols + rightImage.cols;
	col += bias;

	cv::Mat doubleImage(row, col, leftImage.type());

	leftImage.copyTo(doubleImage(cv::Rect(0,0, leftImage.cols, leftImage.rows)));
	rightImage.copyTo(doubleImage(cv::Rect(leftImage.cols + bias, 0, rightImage.cols, rightImage.rows)));

	cv::imshow("Result",doubleImage);
}
/*
�㷨������
		1����С�ֱ��ʣ�����FAST�㷨�������Сͼ���������;
		2���Ŵ�ֱ��ʣ�������λ��ӳ���Դͼ��;
		3����Դͼ��������ƥ���㷨��
				(1)����X��Yֵ������ƥ��;
				(2)ȥ����ƥ����������㣻
				(3)�Դ���ƥ�������з��ࣺ(�뾶Ϊ3ȡԪ��ֵ)
					A�ࣺX��Y��ͬ; (shift = 0, decription = 0)
					B�ࣺX��ͬ��Y��ͬ;
						���෽������Yֵƫ��ֵ���й��࣬ȡÿ�����ĵ�һ�����о�ȷƥ�䣺��5�������������ƥ�䣩
						B1�ࣺ���ƥ�䣬����ƥ�䣻(shift != 0, decription != 0)
						B2�ࣺ�����ƥ�䣬��������. (shift = 0, decription = 0)
					C�ࣺY��ͬ��X��ͬ��
						���෽������Xֵƫ��ֵ���й��࣬ȡÿ�����ĵ�һ�����о�ȷƥ�䣺��5�������������ƥ�䣩
						C1�ࣺ���ƥ�䣬����ƥ�䣻(shift != 0, decription != 0)
						C2�ࣺ�����ƥ�䣬��������. (shift != 0, decription != 0)
				(4) ͳ��ƥ��㣬��ƥ����������ͽ��о��μ���
*/
int main()
{
	//step(1)��step(2);
	IplImage* I_inOriginOne = cvLoadImage( fileOne);
	Corners cornersOne(I_inOriginOne);
	processImage(I_inOriginOne, cornersOne);

	IplImage* I_inOriginTwo = cvLoadImage( fileTwo);
	Corners cornersTwo(I_inOriginTwo);
	processImage(I_inOriginTwo, cornersTwo);

	//step(3)
	matchCorners(I_inOriginOne, cornersOne, I_inOriginTwo, cornersTwo);

	//step(4)//delete corners whose description == 0
	//cornersOne.reduceUselessCorners();
	//cornersTwo.reduceUselessCorners();

	//step(5)
	drawCorners(I_inOriginOne, cornersOne);// can delete
	drawCorners(I_inOriginTwo, cornersTwo);// can delete

	cornersOne.calculateSimilarRect();
	cornersTwo.calculateSimilarRect();
	drawRect(I_inOriginOne, cornersOne);
	drawRect(I_inOriginTwo, cornersTwo);

	//matchRect(I_inOriginOne, cornersOne, I_inOriginTwo, cornersTwo);

	mergeImage(I_inOriginOne, I_inOriginTwo);

	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&I_inOriginOne);
	cvReleaseImage(&I_inOriginTwo);

	return 0;
}
