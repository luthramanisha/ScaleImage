#include "BitmapOperation.h"
#include <list>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <climits>
#include <omp.h>

using namespace std;

BitmapOperation::BitmapOperation(double factor) : _factor(factor) {
	//const
}
/*
* @function to extract color data from pixel data
* @param oldW, oldH, _oldPixel, _oldData
*/
void BitmapOperation::vectorToData(size_t oldW, size_t oldH, Bitmap::PixelData & _oldPixel, unsigned char * _oldData)
{
	int offset = 0;
	for (int i = 0; i < oldH; ++i) {
		for (int j = 0; j < oldW; ++j) {
			_oldData[offset] = _oldPixel[i][j].r;
			offset++;
			_oldData[offset] = _oldPixel[i][j].g;
			offset++;
			_oldData[offset] = _oldPixel[i][j].b;
			offset++;
		}
	}
}

/**
* padding new colors and interpolating new pixels on the border
* @param oldData, oldWidth, oldHeight, newPixelData
*/
void BitmapOperation::copyOldToNewPixels(Bitmap::PixelData &newPixelData, unsigned char* oldData, size_t oldWidth, size_t oldHeight) {

	#pragma omp parallel for 
	for (int i = 0; i < oldHeight; ++i) {
		for (int j = 0; j < oldWidth; ++j) {
			int ij = j * 2 + 1;  //new is same as src width
			int ii = i * 2 + 1;  //new is same as src height
			//index @ char data
			int index = (i * oldWidth + j) * sizeof(RGBPIXEL);

			//Ptr to index 
			unsigned char* color = &oldData[index];

			//red
			newPixelData[ii][ij].r = color[0];
			//green
			newPixelData[ii][ij].g = color[1];
			//blue
			newPixelData[ii][ij].b = color[2];

			newPixelData[ii][ij + 1] = newPixelData[ii][ij];
			newPixelData[ii + 1][ij] = newPixelData[ii][ij];
			newPixelData[ii + 1][ij + 1] = newPixelData[ii][ij];
		}
	}
	//enlarged size for interpolation of pixels
	int srcWidth = 2 * (oldWidth + 1);
	int srcHeight = 2 * (oldHeight + 1);

	borderRows(newPixelData, srcHeight - 1, srcHeight - 2);
	borderRows(newPixelData, 0, 1);
	borderCols(newPixelData, srcWidth - 1, srcWidth - 2);
	borderCols(newPixelData, 0, 1);
}

/*
* Interpolating at initial and end row
* @param newPixelData, newY, oldY
*/
void BitmapOperation::borderRows(Bitmap::PixelData & newPixelData, int newY, int oldY) {
	int width = newPixelData[0].size();
	int height = newPixelData.size();
//#pragma omp parallel for 
	for (int i = 0; i < width; ++i) {
		newPixelData[newY][i] = newPixelData[oldY][i];
	}
}

/*
* Interpolating at initial and end column
* @param newPixelData, newY, oldY
*/
void BitmapOperation::borderCols(Bitmap::PixelData & newPixelData, int newX, int oldX) {
	int height = newPixelData.size();
//#pragma omp parallel for 
	for (int i = 0; i < height; ++i) {
		newPixelData[i][newX] = newPixelData[i][oldX];
	}
}

/*
* Four point bilinear interpolation algorithm.
* Source: P.R. Smith, "BILINEAR INTERPOLATION OF DIGITAL IMAGES" ,
* https://en.wikipedia.org/wiki/Bilinear_interpolation
* 1. First we enlarge the image
* 2. Then we interpolate
*/
void BitmapOperation::performBilinear(Bitmap & bmp)
{
	cout << "Performing New Bilinear interpolation \n";

	// small old image data
	Bitmap::PixelData oldPixelData = bmp.getPixelData();
	const size_t oldWidth = oldPixelData[0].size(), oldHeight = oldPixelData.size();
	unsigned char *oldData = new unsigned char[oldWidth*oldHeight*sizeof(RGBPIXEL)]; //pixel data
	memset(oldData, 0, oldWidth*oldHeight*sizeof(RGBPIXEL));
	vectorToData(oldWidth, oldHeight, oldPixelData, oldData);

	// new scaled image data and variables
	const size_t newWidth = _factor *(oldWidth + 1), newHeight = _factor *(oldHeight + 1);
	unsigned char *newData = new unsigned char[newWidth*newHeight*sizeof(RGBPIXEL)]; //pixel data
	memset(newData, 0, newWidth*newHeight*sizeof(RGBPIXEL));
	Bitmap::PixelData newPixel;
	int srcWidth, srcHeight;

	//enlarged size for interpolation of pixels
	srcWidth = 2 * (oldWidth + 1);
	srcHeight = 2 * (oldHeight + 1);

	//resizing the vector
	newPixel.resize(srcHeight);

	//#pragma omp parallel for 
	for (int i = 0; i < srcHeight; ++i) {
		newPixel[i].resize(srcWidth);
	}

	//padding new colors and copying new pixels on the border 
	//and starting pixels
	copyOldToNewPixels(newPixel, oldData, oldWidth, oldHeight);
	delete[] oldData;

	//vector to save scaled image pixel data
	Bitmap::PixelData newPixelData;

	//#pragma omp parallel for 
	for (int i = 0; i < newHeight; ++i) {
		std::vector<RGBPIXEL> newRow(newWidth);
		for (int j = 0; j < newWidth; ++j) {

			//cout << "\n width:" << row << ", height:" << col;
			//index @ char data
			int index = (i * newWidth + j) * sizeof(RGBPIXEL);
			double widthRatio = (((double)(j)* (srcWidth - 2)) / (newWidth - 1)) + 1;
			double heightRatio = (((double)(i)* (srcHeight - 2)) / (newHeight - 1)) + 1;

			int x = ((int)(widthRatio / 2)) * 2;
			int inc_x = ((int)(widthRatio / 2 + 1)) * 2;
			if (inc_x >= srcWidth)
				inc_x = srcWidth - 1;
			double dx = (widthRatio - x) / 2; //difference of x

			int y = ((int)(heightRatio / 2)) * 2;
			int inc_y = ((int)(heightRatio / 2 + 1)) * 2;
			if (inc_y >= srcHeight)
				inc_y = srcHeight - 1;
			double dy = (heightRatio - y) / 2; //difference of y

			//The four pixel interpolation matrix
			RGBPIXEL f00 = newPixel[y][x];
			RGBPIXEL f10 = newPixel[y][inc_x];
			RGBPIXEL f01 = newPixel[inc_y][x];
			RGBPIXEL f11 = newPixel[inc_y][inc_x];

			//interpolated red pixel
			newData[index + 0] = f00.r * (1.f - dx) * (1.f - dy) + f10.r * (dx)* (1 - dy) +
				f01.r * (dy)* (1 - dx) + f11.r * dx * dy;

			//interpolated green pixel
			newData[index + 1] = f00.g * (1.f - dx) * (1.f - dy) + f10.g * (dx)* (1 - dy) +
				f01.g * (dy)* (1 - dx) + f11.g * dx * dy;

			//interpolated blue pixel
			newData[index + 2] = f00.b * (1.f - dx) * (1.f - dy) + f10.b * (dx)* (1 - dy) +
				f01.b * (dy)* (1 - dx) + f11.b * dx * dy;

			newRow[j] = { newData[index + 0], newData[index + 1], newData[index + 2] };
		}
		//insert row of new interpolated pixels
		newPixelData.push_back(newRow);
	}
	delete[] newData;
	bmp.setPixelData(newPixelData);
}
