#include "GeometricTranformer.h"
int main(int argc, char** argv)
{
	AffineTransform x;
	x.Translate(1, 2);
	/*x.Rotate(50);
	x.Scale(12.5, 66);*/
	AffineTransform* A = &x;
	vector<uchar> B;
	
	//Mat src = imread("E:\\Deadline\\lena.png", CV_LOAD_IMAGE_COLOR);
	Mat src = imread("E:\\Deadline\\lena.png", IMREAD_COLOR);

	int width = src.cols, height = src.rows;
	int srcChannels = src.channels();
	int srcWidthStep = src.step[0];
	uchar* pSrc = src.data;
	BilinearInterpolate Test(A,src);
	B = Test.Interpolate(1.5, 4.6, pSrc, srcWidthStep, srcChannels);
	for (int i = 0; i < B.size(); i++) {
		cout << (int)B[i] << endl;
	}
	float x = 2;
	float y = 10;

	A.Rotate(30);
	A.TransformPoint(x, y);
	A.r_TransformPoint(x, y);

	/*A.Rotate(12.5);
	
	A.Scale(2,3);
	return 0;
}