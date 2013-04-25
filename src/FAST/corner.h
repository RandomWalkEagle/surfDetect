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

	void				setCornerPixel(const IplImage* image, const CvPoint& pos);
	void				setMatchingStatus(matchStatus status);
	void				setShift(int shift);
	void				setDescription(int description);

	matchStatus	getMatchingStatus();
	int					getShift();
	int					getDescription();
	CvPoint			getCornerPos();
	CvScalar			getCornerPixel();
	bool				isMatched();

	std::vector<CvScalar>& getAdjacentPixels(const IplImage* image,const int& Region);
	std::vector<CvScalar>& getAdjacentCirclePixels(const IplImage* image);

private:
	CvPoint	m_pos;
	CvScalar m_pixel;
	int			m_shift;
	int			m_description;
	matchStatus	m_isMatched;
	std::vector<CvScalar> m_adjacentPixels;
	std::vector<CvScalar> m_adjacentCirclePixels;
};

class Corners{
public:
	enum shiftType{
		Shift_X,
		Shift_Y,
	};

public:
	Corners(IplImage* image);
	~Corners();

	void		 pushCorner( const CvPoint& cornerPos );
	void		 pushShift( const shiftType& type , const Corner& shiftCorner );
	void		 addShiftCounts( const shiftType& type , const int& shift );
	void		 calculateSimilarRect();
	void		 calculateSimilarRectBasedOnDescription(const int& description);

	CvRect	 getSimilarRect();
	Corner& getCorner(int pos);
	int			 getCornerSize() {return m_num;};
	int			 findDescription( const shiftType& type , const int& shift );

	void		 reduceUselessCorners();

protected:

private:
	IplImage*  m_imagePtr;
	int				m_num;
	int				m_bestShift;
	CvRect		m_rect;

	std::vector<Corner>	m_corners;
	std::vector<std::pair<Corner, int>> m_shiftX;
	std::vector<std::pair<Corner, int>> m_shiftY;
};