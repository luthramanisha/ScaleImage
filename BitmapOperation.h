#include "Bitmap.h"

/*
* Class to perform bitmap interpolation 
* default scaling algorithm : nearest neighbor interpolation
* performBilinear : implements 4 pixel bilinear interpolation for bitmap images
* also contains utility functions to help performing interpolation
*/

class BitmapOperation {
public:
	BitmapOperation();
	BitmapOperation(double);
	void perform(Bitmap & bitmap);
	void performBilinear(Bitmap & bmp);
	void vectorToData(size_t, size_t, Bitmap::PixelData&, unsigned char*);
	void copyOldToNewPixels(Bitmap::PixelData&, unsigned char*, size_t, size_t);
	void borderCols(Bitmap::PixelData&, int, int);
	void borderRows(Bitmap::PixelData&, int, int);
	double _factor;
};