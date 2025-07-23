//	File:           RGBpixmap.h
//	Description:    Pixmap storage object
//	Programmer:	Dave Mount
//	Date:		March 2003
//	Source:		Adapted from the code from F. S. Hill's book,
//			"Computer Graphics Using OpenGL", Prentice Hall,
//			2000.
//----------------------------------------------------------------------
//	See file "ReadMe" for more information.
//
//	The object RGBpixmap is a stripped down version of the one
//	appearing Hill's book.  There is one significant enhancement.
//	The method readBMPFile() now as three arguments.
//	
//	bool readBMPFile(		// read a .bmp file
//		const string& fname,	// name of the file
//		bool glPad,		// pad size up to a power of 2
//		bool verbose);		// output summary
//
//	If the second parameter is true, then the image array is padded
//	up to the next higher power of 2 in size.  This is done because
//	OpenGL expects texture maps whose dimensions are exact powers of
//	2.  These additional entries are not set to any value.  (This
//	should be fixed.) Otherwise, the image size is not altered.  If
//	verbose argument is true, summary information is written to
//	cerr.
//
//	Beware: This procedure has not been carefully tested.  Use at
//	your own risk.  Report errors to Dave Mount (mount@cs.umd.edu).
//----------------------------------------------------------------------

#ifndef RGB_PIXMAP
#define RGB_PIXMAP

#include <cstdlib>			// standard includes
#include <string>			// STL strings
#include <GL/glut.h>			// glut/OpenGL includes

typedef unsigned char	RGB_uchar;	// unsigned type aliases
typedef unsigned short	RGB_ushort;
typedef unsigned long	RGB_ulong;

using namespace std;			// make std:: accessible

//----------------------------------------------------------------------
//  RGBpixel
//	Stores a single RGB value
//----------------------------------------------------------------------

class RGBpixel {
public:
    RGB_uchar	r, g, b;
//
    RGBpixel() { r = g = b = 0;  }			// default constructor

    RGBpixel(const RGBpixel& p)			// copy constructor
    { r = p.r; g = p.b; b = p.b;  }

    						// construct from components
    RGBpixel(RGB_uchar rr, RGB_uchar gg, RGB_uchar bb)
    { r = rr; g = gg; b = bb; }

    						// set values
    void set(RGB_uchar rr, RGB_uchar gg, RGB_uchar bb)
    { r = rr; g = gg; b = bb; }
};

//----------------------------------------------------------------------
//  RGBpixmap
//	RGB pixel array
//----------------------------------------------------------------------

class RGBpixmap {
public:
    RGBpixel*	pixel;				// array of pixels
    int		nRows, nCols;			// dimensions
//
    RGBpixmap()					// default constructor
    {  nRows = nCols = 0;  pixel = NULL;  }

    RGBpixmap(int rows, int cols)		// constructor
    {
    	nRows = rows;  nCols = cols;
	pixel = new RGBpixel[rows*cols];
    }
						// read from file
    bool readBMPFile(const string& fname, bool glPad, bool verbose=false);

    void freeIt()				// deallocate
        {  delete [] pixel;  nRows = nCols = 0;  }

    // ~RGBpixmap() { freeIt(); }		// destructor (disabled)

    void draw()					// draw pixmap to raster pos
    {
    	if (nRows == 0 || nCols == 0) return;	// nothing there
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// align to single byte
	glDrawPixels(nCols, nRows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    }
    						// set pixel color
    void setPixel(int x, int y, const RGBpixel& color)
    {
    	if (x >= 0 && x < nCols && y >= 0 && y < nRows)
	    pixel[x + nCols*y] = color;
    }

    RGBpixel getPixel(int x, int y)			// return pixel color
        { return pixel[x  + nCols*y]; }
};

#endif
