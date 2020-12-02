#include "GeometricTranformer.h"

int main(int argc, char** argv)
{
	AffineTransform x;
	GeometricTransformer gt;
	PixelInterpolate *b= new  BilinearInterpolate() ;

	Mat src = imread("E:\\3.jpg", IMREAD_COLOR);


	Mat dst;


	gt.Scale(src, dst, 2, 3, b);

	imshow("win2", dst);

}