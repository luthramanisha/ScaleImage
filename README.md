# Scale Image
Two algorithms for scaling image is implemented in this repository. One is nearest neighbor interpolation method, second is 
bilinear interpolation method without use of any image processing library.

##Program description
The program is a command line tool. It is intended to allow a user to scale
a bitmap image. It will always write the output to ’out.bmp’ in the current
directory. To do so it accepts program options in this format:

```
−s < scalefactor >< imagefilepath >
```
E.g. ’stripes.bmp’ required argument −s x : A scale factor to change the
size the output image by a factor.
E.g. −s 2.75 will increase the image size by a factor 2.75
The program is restricted to work with 24 bit RGB Bitmaps without color
palette.

###Comparison
The nearest neighbor (NN) interpolation algorithm chooses a value
of the nearest pixel for the unknown interpolated pixel. The logic is very simple and works well for images without any curves, for eg., in the task folder
: ’stripes.bmp’, ’box.bmp’. However, for the images with curves, for eg.,
’curve.bmp’ and ’circle.bmp’, the algorithm produces jaggy-effect in the image. The Jaggy artifact is due to aliasing, which causes distortions in the
image. In other words, this artifact causes pixels to be distinguishable and
could be clearly seen in the image, and causes a stair like effect in the image (mainly with curves). To get rid of the jagginess, or to ensure good
anti-aliasing filtering and smoother edges, I suggest implementing bilinearinterpolation algorithm for scaling.
The bilinear interpolation method, generates an anti-aliasing effect in
the image, which causes smoother edges and produces intermediate colors,
while maintaining the integrity of the image, as asked in the task. As the
name suggests, it is based on linear-interpolation equation. This is the
method to estimate an arbitrary point between two other points. The same
is done in both directions x and y. In the end, these two linear interpolation equations are combined to estimate the value of the pixel in between
four points or pixels in image. Thus, the average weight of four neighbourhood pixels determine the value of the unknown pixel, in contrast to nearest-neighbour algorithm. The comparison between the results of provided images
is shown in Figure 1, (i) the original image, (ii) Nearest neighbour Interpolation results and (iii) Bilinear Interpolation results.

###Implementation
The algorithm has been realized in the implementation `BitmapOperation ::
performBilinear` method. That is supported by utility methods and suboperation methods BitmapOperation :: vectorT oData, BitmapOperation ::
copyOldT oNewP ixels, BitmapOperation :: borderRows and BitmapOperation ::
borderCols methods. The functionality of the aforementioned sub-routines
is explained as follows:

