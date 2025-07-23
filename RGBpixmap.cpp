//----------------------------------------------------------------------
//	File:           RGBpixmap.cpp
//	Description:    Pixmap storage object
//	Programmer:	Dave Mount
//	Date:		March 2003
//	Source:		Adapted from the code from F. S. Hill's book,
//			"Computer Graphics Using OpenGL", Prentice Hall,
//			2000.
//
//	See the file RGBpixmap.h for information.
//
//	Beware: This procedure has not been carefully tested.  Use at
//	your own risk.  Report errors to Dave Mount (mount@cs.umd.edu).
//----------------------------------------------------------------------

#include <cstdlib>			// standard includes
#include <fstream>			// C++ file I/O
#include <iostream>			// C++ I/O

#include "RGBpixmap.h"			// pixel map definitions

ifstream bmpFile;			// .bmp file

//----------------------------------------------------------------------
//  Error utility
//----------------------------------------------------------------------
static void RGBerror(const string& msg, bool fatal)
{
    if (fatal) {
    	std::cerr << "Error: " << msg << std::endl;
	bmpFile.close();		// close .bmp file
    }
    else {
    	std::cerr << "Warning: " << msg << std::endl;
    }
}

//----------------------------------------------------------------------
//  round2Power2
//	Rounds the argument up to the next higher power of 2.  The
//	method is to divide n-1 by 2 until 0.  If it takes k divides to
//	get to zero, then the result returned in 2^k.
//----------------------------------------------------------------------

RGB_ulong round2Power2(RGB_ulong n)
{
    RGB_ulong res = 1;
    n -= 1;
    while (n > 0) {
	n >>= 1;  res <<= 1;
    }
    return res;
}

//----------------------------------------------------------------------
//  getShort, getLong
//	Read short (long) from file.  Note that BMP format stores values
//	in little-endian form (lowest byte first).
//----------------------------------------------------------------------

static RGB_ushort getShort()
{
    char c;
    RGB_ushort ip;
    bmpFile.get(c);  ip = RGB_ushort(c);	// store lower order byte
    bmpFile.get(c);  ip |= RGB_ushort(c) << 8;	// store higher order byte
    return ip;
}

static RGB_ulong getLong()
{
    char c;
    RGB_ulong ip;
    bmpFile.get(c);  ip = RGB_uchar(c);		// store lower order byte
    bmpFile.get(c);  ip |= RGB_uchar(c) << 8;
    bmpFile.get(c);  ip |= RGB_uchar(c) << 16;
    bmpFile.get(c);  ip |= RGB_uchar(c) << 24;	// store highest order byte
    return ip;
}

//----------------------------------------------------------------------
//  readBMPfile
//	Reads a .bmp file and stores in RGBpixmap.  Since OpenGL
//	requires that pixel arrays be a power of 2 in size, there is
//	an option which pads the size of the array up to the next higher
//	power of 2.
//----------------------------------------------------------------------

bool RGBpixmap::readBMPFile(const string& fname, bool glPad, bool verbose)
{
    						// open file for reading
    bmpFile.open(fname.c_str(), ios::in|ios::binary);
    if (!bmpFile) {
	std::cerr << "File: " << fname << std::endl;
    	RGBerror("Cannot open file", true);
	return false;
    }
    else if (verbose) {
	std::cerr << "Opening bmp file " << fname << std::endl;
    }
    char ch1, ch2;
    bmpFile.get(ch1);				// read .bmp magic number
    bmpFile.get(ch2);
    if (ch1 != 'B' || ch2 != 'M') {
	RGBerror("Illegal magic number.  May not be a .bmp file", false);
    }
    RGB_ulong  fileSize		= getLong();	// read file size
    RGB_ushort reserved1	= getShort();	// (ignore)
    RGB_ushort reserved2	= getShort();	// (ignore)
    RGB_ulong  offBits		= getLong();	// offset to image (unreliable)
    RGB_ulong  headerSize	= getLong();	// always 40
    RGB_ulong  numCols		= getLong();	// number of columns in image
    RGB_ulong  numRows		= getLong();	// number of rows in image
    RGB_ushort planes		= getShort();	// number of planes (1)
    RGB_ushort bitsPerPixel	= getShort();	// (assume 24)
    if (bitsPerPixel != 24) {			// check bits per pixel
    	RGBerror("We only support uncompressed 24-bit/pixel images", true);
	return false;
    }
    RGB_ulong  compression	= getLong();	// (assume 0)
    RGB_ulong  imageSize	= getLong();	// total bytes in image
    RGB_ulong  xPels		= getLong();	// (ignored)
    RGB_ulong  yPels		= getLong();	// (ignored)
    RGB_ulong  numLUTentry	= getLong();	// (ignored)
    RGB_ulong  impColors	= getLong();	// (ignored)
    //------------------------------------------------------------------
    //  Note: Compiler may complain about many unused variables.
    //------------------------------------------------------------------

    int nBytesInRow = ((3 * numCols + 3)/4) * 4;// round to mult of 4
    int numPadBytes = nBytesInRow - 3 * numCols;// unused bytes in row

    if (glPad) {				// pad to power of 2?
	nRows = round2Power2(numRows);		// set class's members
	nCols = round2Power2(numCols);
    }
    else {
	nRows = numRows;			// set class's members
	nCols = numCols;
    }
    if (verbose) {
        std::cerr << "BMP file information: \n"
	    << "  file-name: "	<< fname << "\n"
	    << "  file-size: "	<< fileSize << "\n"
	    << "  columns: "	<< numCols << "\n"
	    << "  rows: "	<< numRows << "\n"
	    << "  bits/pixel: "	<< bitsPerPixel << "\n";
	if (glPad) {
	    std::cerr << "  Rounding size to " << nRows << " x " << nCols << "\n";
	}
    }

    pixel = new RGBpixel[nRows * nCols];	// allocate array
    if (!pixel) {				// cannot allocate
    	RGBerror("Cannot allocate storage for image array", true);
	return false;
    }
    						// read pixel values
    for (RGB_ulong row = 0; row < numRows; row++) {
	RGB_ulong index = row * nCols;
    	for (RGB_ulong col = 0; col < numCols; col++) {
	    char r, g, b;
	    bmpFile.get(b);			// read RGB (reversed)
	    bmpFile.get(g);
	    bmpFile.get(r);
	    					// store in array 
	    pixel[index++].set(RGB_uchar(r), RGB_uchar(g), RGB_uchar(b));
	}
	for (int k = 0; k < numPadBytes; k++) {
	    char pad;
	    bmpFile.get(pad);			// skip padding bytes
	}
    }
    bmpFile.close();				// close the file
    return true;				// return good status
}
