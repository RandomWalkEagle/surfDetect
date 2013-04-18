#include "match.h"
#include "utils.h"
#include "../calTime.h"

const int  KTEMPLATESize =41;
static int  KTEMPLATEBIASX =0;
const int  KTEMPLATEBIASY =100;

#define TEST_FIRSTFRAME false

Ematch::Ematch(IplImage * imageFirst, IplImage * imageSecond)
{
	m_imageAsFirstFrame = imageFirst;
	m_imageAsSecondFrame = imageSecond;
	KTEMPLATEBIASX = m_imageAsFirstFrame->width/2;
}

Ematch::~Ematch()
{
	cvReleaseImage(&m_topImage);
	cvReleaseImage(&m_downImage);
	cvDestroyAllWindows();
}

void Ematch::createTemplateImage()
{
	calTime cal(std::string(__FUNCTION__));
	//create TopImage
	CvSize size= cvSize(KTEMPLATESize,KTEMPLATESize);
	cvSetImageROI(m_imageAsFirstFrame,cvRect(KTEMPLATEBIASX,KTEMPLATEBIASY,size.width, size.height));//设置源图像ROI
	m_topImage = cvCreateImage(size,m_imageAsFirstFrame->depth,m_imageAsFirstFrame->nChannels);
	cvCopy(m_imageAsFirstFrame,m_topImage); //复制图像
	cvResetImageROI(m_imageAsFirstFrame);//源图像用完后，清空ROI
	//cvSaveImage("top.jpg",m_topImage);//保存目标图像

	//create DownImage
	cvSetImageROI(m_imageAsFirstFrame,cvRect(KTEMPLATEBIASX,m_imageAsFirstFrame->height-KTEMPLATEBIASY,size.width, size.height));//设置源图像ROI
	m_downImage = cvCreateImage(size,m_imageAsFirstFrame->depth,m_imageAsFirstFrame->nChannels);
	cvCopy(m_imageAsFirstFrame,m_downImage);
	cvResetImageROI(m_imageAsFirstFrame);

	//createMatchImage
	createMatchImage();
}

void Ematch::showTemplateImage()
{
	cv::Mat leftImage(m_topImage);
	cv::Mat rightImage(m_downImage);
	cv::Mat orignImage;
	if (TEST_FIRSTFRAME)
	{
		orignImage = cv::Mat(m_imageAsFirstFrame);
	}
	else
	{
		orignImage = cv::Mat(m_imageAsSecondFrame);
	}

	int row = leftImage.rows > rightImage.rows ? leftImage.rows : rightImage.rows;
	row += 20;
	row += orignImage.rows;
	int col = leftImage.cols > rightImage.cols ? leftImage.cols : rightImage.cols;
	col = col > orignImage.cols ? col : orignImage.cols;

	cv::Mat doubleImage(row, col, orignImage.type());

	leftImage.copyTo(doubleImage(cv::Rect(0,0, leftImage.cols, leftImage.rows)));
	rightImage.copyTo(doubleImage(cv::Rect(leftImage.cols + 20, 0, rightImage.cols, rightImage.rows)));
	orignImage.copyTo(doubleImage(cv::Rect(0, leftImage.rows + 20, orignImage.cols, orignImage.rows)));

	//cv::namedWindow("double");
	cv::imshow("double",doubleImage);
	cv::waitKey(0);
}

void Ematch::createMatchImage()
{
	if (TEST_FIRSTFRAME)
	{
		CvSize size= cvSize(KTEMPLATESize,m_imageAsFirstFrame->height);
		cvSetImageROI(m_imageAsFirstFrame,cvRect(KTEMPLATEBIASX,0,size.width, size.height));//设置源图像ROI
		m_imageMatch = cvCreateImage(size,m_imageAsFirstFrame->depth,m_imageAsFirstFrame->nChannels);
		cvCopy(m_imageAsFirstFrame,m_imageMatch); //复制图像
		cvResetImageROI(m_imageAsFirstFrame);//源图像用完后，清空ROI
	}else
	{
		CvSize size= cvSize(KTEMPLATESize,m_imageAsSecondFrame->height);
		cvSetImageROI(m_imageAsSecondFrame,cvRect(KTEMPLATEBIASX,0,size.width, size.height));//设置源图像ROI
		m_imageMatch = cvCreateImage(size,m_imageAsSecondFrame->depth,m_imageAsSecondFrame->nChannels);
		cvCopy(m_imageAsSecondFrame,m_imageMatch); //复制图像
		cvResetImageROI(m_imageAsSecondFrame);//源图像用完后，清空ROI
	}
}


void Ematch::matchUp()
{
	//match TopImage and matchImage
	IplImage* res = match(m_topImage, m_imageMatch, true);

	cvReleaseImage(&res);
}

void Ematch::matchDown()
{
	//match DownImage and matchImage
	IplImage* res = match(m_downImage, m_imageMatch, false);

	showTemplateImage();
	cvReleaseImage(&res);
}

IplImage* Ematch::match( IplImage* templateImage, IplImage* originImage, bool isTopImage )
{
	calTime cal(std::string(__FUNCTION__));
	//get image's properties
	int img_width = originImage->width;
	int img_height = originImage->height;
	int tpl_width = templateImage->width;
	int tpl_height = templateImage->height;
	int res_width = img_width - tpl_width + 1;
	int res_height = img_height - tpl_height + 1;

	IplImage* res = cvCreateImage(cvSize(res_width,res_height), IPL_DEPTH_32F, 1);
	cvMatchTemplate(originImage, templateImage, res, CV_TM_CCORR_NORMED);//0.29s
	//cvMatchTemplate(originImage, templateImage, res, CV_TM_CCOEFF_NORMED); //0.299s

	cvNormalize(res,res,1,0,CV_MINMAX);
	CvPoint maxloc;
	cvMinMaxLoc( res, NULL, NULL, NULL, &maxloc, 0 );
	CvRect rect = cvRect(maxloc.x, maxloc.y, tpl_width, tpl_height);

	if (TEST_FIRSTFRAME)
	{
		CvPoint pt1, pt2;
		pt1 = cvPoint(rect.x + KTEMPLATEBIASX, rect.y);
		pt2 = cvPoint(rect.x + rect.width +KTEMPLATEBIASX, rect.y + rect.height);
		CvScalar scalar =isTopImage ? cvScalar(0,0,255) : cvScalar(255,0,255);
		cvRectangle( m_imageAsFirstFrame, pt1, pt2, scalar, 1, 8, 0);
	}
	else
	{
		CvPoint pt1, pt2;
		pt1 = cvPoint(rect.x + KTEMPLATEBIASX, rect.y);
		pt2 = cvPoint(rect.x + rect.width +KTEMPLATEBIASX, rect.y + rect.height);
		CvScalar scalar =isTopImage ? cvScalar(0,0,255) : cvScalar(255,130,0);
		cvRectangle( m_imageAsSecondFrame, pt1, pt2, scalar, 1, 8, 0);
	}

	return res;
}
