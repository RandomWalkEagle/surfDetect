#include <opencv/cv.h>


void resize(frame &dst, frame &src)
{
	IplImage *srcImage = cvCreateImageHeader(cvSize(src.width, src.height), IPL_DEPTH_8U, 3);
	IplImage *dstImage = cvCreateImageHeader(cvSize(dst.width, dst.height), IPL_DEPTH_8U, 3);
	cvSetImageData(srcImage, src.data, src.width * 3);
	cvSetImageData(dstImage, dst.data, dst.width * 3);
	cvResize(srcImage, dstImage);
	cvReleaseImageHeader(&srcImage);
	cvReleaseImageHeader(&dstImage);

}