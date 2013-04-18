/*********************************************************** 
*  --- OpenSURF ---                                       *
*  This library is distributed under the GNU GPL. Please   *
*  use the contact form at http://www.chrisevansdev.com    *
*  for more information.                                   *
*                                                          *
*  C. Evans, Research Into Robust Visual Features,         *
*  MSc University of Bristol, 2008.                        *
*                                                          *
************************************************************/

#include <highgui.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include "utils.h"

using namespace std;

//-------------------------------------------------------

static const int NCOLOURS = 8;
static const CvScalar COLOURS [] = {cvScalar(255,0,0), cvScalar(0,255,0), 
                                    cvScalar(0,0,255), cvScalar(255,255,0),
                                    cvScalar(0,255,255), cvScalar(255,0,255),
                                    cvScalar(255,255,255), cvScalar(0,0,0)};

//-------------------------------------------------------

//! Display error message and terminate program
void error(const char *msg) 
{
  cout << "\nError: " << msg;
  getchar();
  exit(0);
}

//-------------------------------------------------------

//! Show the provided image and wait for keypress
void showImage(const IplImage *img)
{
  cvNamedWindow("Surf", CV_WINDOW_AUTOSIZE); 
  cvShowImage("Surf", img);  
  cvWaitKey(0);
}

//-------------------------------------------------------

//! Show the provided image in titled window and wait for keypress
void showImage(char *title,const IplImage *img)
{
  cvNamedWindow(title, CV_WINDOW_AUTOSIZE); 
  cvShowImage(title, img);  
  cvWaitKey(0);
}

//-------------------------------------------------------

// Convert image to single channel 32F
IplImage *getGray(const IplImage *img)
{
  // Check we have been supplied a non-null img pointer
  if (!img) error("Unable to create grayscale image.  No image supplied");

  IplImage* gray8, * gray32;

  gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );

  if( img->nChannels == 1 )
    gray8 = (IplImage *) cvClone( img );
  else {
    gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
    cvCvtColor( img, gray8, CV_BGR2GRAY );
  }

  cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

  cvReleaseImage( &gray8 );
  return gray32;
}

//-------------------------------------------------------

// Draw the FPS figure on the image (requires at least 2 calls)
void drawFPS(IplImage *img)
{
  static int counter = 0;
  static clock_t t;
  static float fps;
  char fps_text[20];
  CvFont font;
  cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1.0,1.0,0,2);

  // Add fps figure (every 10 frames)
  if (counter > 10)
  {
    fps = (10.0f/(clock()-t) * CLOCKS_PER_SEC);
    t=clock(); 
    counter = 0;
  }

  // Increment counter
  ++counter;

  // Get the figure as a string
  sprintf(fps_text,"FPS: %.2f",fps);

  // Draw the string on the image
  cvPutText (img,fps_text,cvPoint(10,25), &font, cvScalar(255,255,0));
}