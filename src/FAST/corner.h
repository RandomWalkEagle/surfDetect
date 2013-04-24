#pragma once
#include <cvaux.h>

class Corner
{
public:
enum matchStatus{
	Matched_No,
	Matched_X,
	Matched_Y,
	Matched_XY,
};

public:
	Corner(const IplImage* image, const CvPoint& pos);
	~Corner();

	void setCornerPixel(const IplImage* image, const CvPoint& pos);
	void setMatchingStatus(matchStatus status);
	matchStatus getMatchingStatus();
	CvPoint getCornerPos();
	CvScalar getCornerPixel();
	std::vector<CvScalar>& getAdjacentPixels(const IplImage* image,const int& Region);
	bool isMatched();

protected:

private:
	CvPoint	m_pos;
	CvScalar m_pixel;
	matchStatus	m_isMatched;
	std::vector<CvScalar> m_adjacentPixels;
};

class Corners{
public:
	Corners(IplImage* image);
	~Corners();

	void pushCorner( const CvPoint& cornerPos );
	void setSimilarRect( const CvRect& rect );
	CvRect getSimilarRect();
	Corner& getCorner(int pos);
	void reduceUselessCorners();
	int	getCornerSize() {return m_num;};

protected:

private:
	IplImage*  m_imagePtr;
	int				m_num;
	std::vector<Corner>	m_corners;
	CvRect		m_rect;
};