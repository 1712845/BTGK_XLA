#include "pch.h"
#include "GeometricTranformer.h"

AffineTransform::AffineTransform() {
	// khởi tạo ma trận đơn vị (Mat::eye)
	_matrixTransform = cv::Mat::eye(3, 3, CV_32FC1);
	r_matrixTransform = cv::Mat::eye(3, 3, CV_32FC1);
}

AffineTransform::~AffineTransform() {

}
void AffineTransform::Translate(float dx, float dy) {
	Mat temp = cv::Mat::eye(3, 3, CV_32FC1);
	Mat r_temp = cv::Mat::eye(3, 3, CV_32FC1);

	if (temp.empty() || r_temp.empty()) {
		return;
	}
	swap(dx, dy); // Do tính toán = ma trận nên phải đảo lại
	_matrixTransform.at<float>(0, 2) = dx;
	_matrixTransform.at<float>(1, 2) = dy;

	r_matrixTransform.at<float>(0, 2) = -dx; // tăng 1 đoạn x thì affine nghịch phải giảm 1 đoạn x
	r_matrixTransform.at<float>(1, 2) = -dy; // tăng 1 đoạn y thì affine nghịch phải giảm 1 đoạn y
	
	_matrixTransform = temp*_matrixTransform ;
	r_matrixTransform = r_matrixTransform* r_temp; // đổi thứ tự, ví dụ affine thuận là scale rồi xoay, 
													//thì affine nghịch phải xoay rồi mới scale ngược.
	cout << _matrixTransform << endl;
	cout << r_matrixTransform << endl;
}

void AffineTransform::Rotate(float angle){
	angle = angle * PI / 180; // Đổi ra độ ra radian
	Mat temp = cv::Mat::eye(3, 3, CV_32FC1);
	Mat r_temp = cv::Mat::eye(3, 3, CV_32FC1); // ma trận tính toán cho affine ngược

	if (temp.empty() || r_temp.empty()) {
		return;
	}
	
	temp.at<float>(0, 0) = cos(angle);
	temp.at<float>(0, 1) = -sin(angle);
	temp.at<float>(1, 0) = sin(angle);
	temp.at<float>(1, 1) = cos(angle);

	r_temp.at<float>(0, 0) = cos(-angle); // ngược với affine
	r_temp.at<float>(0, 1) = -sin(-angle);
	r_temp.at<float>(1, 0) = sin(-angle);
	r_temp.at<float>(1, 1) = cos(-angle);

	_matrixTransform = temp*_matrixTransform ;
	r_matrixTransform = r_matrixTransform *r_temp ;
	cout << _matrixTransform << endl;
	cout << r_matrixTransform << endl;
}

void AffineTransform::Scale(float sx, float sy) {
	Mat temp = cv::Mat::eye(3, 3, CV_32F);
	Mat r_temp = cv::Mat::eye(3, 3, CV_32F); // ma trận tính toán cho affine ngược

	if (temp.empty() || r_temp.empty()) {
		return;
	}
	swap(sx, sy); // Do tính toán = ma trận nên phải đảo lại
	temp.at<float>(0, 0) = sx;
	temp.at<float>(1, 1) = sy;

	r_temp.at<float>(0, 0) = 1.0/sx; // ngược với affine
	r_temp.at<float>(1, 1) = 1.0/sy;

	_matrixTransform = temp*_matrixTransform ;
	r_matrixTransform = r_matrixTransform*r_temp;
	cout << _matrixTransform << endl;
	cout << r_matrixTransform << endl;
}

void AffineTransform::TransformPoint(float &x, float &y){
	Mat temp = cv::Mat::eye(3, 1, CV_32F);

	if (temp.empty()) {
		return;
	}

	temp.at<float>(0, 0) = x;
	temp.at<float>(1, 0) = y;
	temp.at<float>(2, 0) = 1;
	temp = _matrixTransform*temp;
	x = temp.at<float>(0, 0);
	y = temp.at<float>(1, 0);
	cout << x <<" "<< y<<endl;
}

void  AffineTransform::r_TransformPoint(float &x, float &y) {
	Mat r_temp = cv::Mat::eye(3, 1, CV_32F); // ma trận tính toán cho affine ngược

	if (r_temp.empty()) {
		return;
	}

	r_temp.at<float>(0, 0) = x;
	r_temp.at<float>(1, 0) = y;
	r_temp.at<float>(2, 0) = 1;
	r_temp =r_matrixTransform*r_temp;

	x = r_temp.at<float>(0, 0); 
	y = r_temp.at<float>(1, 0);
	cout << x << " " << y<<endl;
}

PixelInterpolate::PixelInterpolate() {

}
PixelInterpolate::~PixelInterpolate() {

}
BilinearInterpolate::BilinearInterpolate(AffineTransform* affine, Mat source) {
	this->matrix = affine;
	this->src = source;
}

BilinearInterpolate::~BilinearInterpolate() {

}
void BilinearInterpolate::setAffineMatrix(AffineTransform* m) {
	this->matrix = m;
}

vector<uchar> BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels)
{
	matrix->r_TransformPoint(tx, ty);
	float b = abs(tx - round(tx)); //ở đây đặt tx là y, và ty = x, do đó nên ngược với công thức lý thuyết
	float a = abs(ty - round(ty));
	int m = round(tx);
	int n = round(ty);
	int cols = src.cols;
	int rows = src.rows;
	vector<uchar> res;
	if (pSrc == NULL)return res;
	res.resize(nChannels);

	//nếu điểm affine nghịch nằm ngoài, cho nó màu đen
	if ((m < 0 or m >= rows) or (n < 0 or n >= cols)) {
		for (int i = 0; i < nChannels; i++) {
			res[i] = 0;
		}
		return res;
	}

	// di chuyển con trỏ đến vị trí f(n,m)
	pSrc += m * srcWidthStep;
	pSrc += n * (nChannels);

	for (int i = 0; i < nChannels; i++) {//công thức f'(x',y') = (1-a)(1-b)f(n,m)+a(1-b)f(n+1,m)+(1-a)bf(n,m+1)+abf(n+1,m+1)
		res[i] = (1 - a) * (1 - b) * pSrc[i]; // cái nào nằm ngoài cho nó bằng 0
		if (tx + 1 < rows) {
			uchar* pTemp = pSrc + srcWidthStep;
			res[i] += b * (1 - a) * pTemp[i];
		}
		if (ty + 1 < cols) {
			uchar* pTemp = pSrc + nChannels;
			res[i] += a * (1 - b) * pTemp[i];
		}
		if (tx + 1 < rows and ty + 1 < cols) {
			uchar* pTemp = pSrc + srcWidthStep + nChannels;
			res[i] += a * b * pTemp[i];
		}
	}
	return res;
}