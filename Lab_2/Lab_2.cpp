#include "GeometricTranformer.h"
int main(int argc, char** argv)
{
	AffineTransform x;
	//x.Translate(1, 2);
	///*x.Rotate(50);
	//x.Scale(12.5, 66);*/
	//AffineTransform* A = &x;
	//vector<uchar> B;
	//
	////Mat src = imread("E:\\Deadline\\lena.png", CV_LOAD_IMAGE_COLOR);
	//Mat src = imread("E:\\3.jpg", IMREAD_COLOR);

	//int width = src.cols, height = src.rows;
	//int srcChannels = src.channels();
	//int srcWidthStep = src.step[0];
	//uchar* pSrc = src.data;
	//BilinearInterpolate Test(A,src);
	//B = Test.Interpolate(1.5, 4.6, pSrc, srcWidthStep, srcChannels);
	//for (int i = 0; i < B.size(); i++) {
	//	cout << (int)B[i] << endl;
	//}

	//x.Scale(2, 5);
	Mat m = Mat::zeros(3, 3, CV_32F);
	m.at<float>(1, 1) = 12;
	m.at<float>(0, 2) = 5;
	m.at<float>(2, 1) = 6;

	cout << m << endl;;
	cout << m.inv();




}