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

PixelInterpolate::PixelInterpolate() {}
PixelInterpolate::~PixelInterpolate() {}

vector<uchar> NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	if (pSrc == NULL)
		return { 0 };
	vector<uchar> rbg;
	int x1 = (int)(floor(tx));
	int x2 = (int)(ceil(tx));
	int y1 = (int)(floor(ty));
	int y2 = (int)(ceil(ty));
	if ((pSrc + x1 + y1 * srcWidthStep) == NULL || (pSrc + x2 + y2 * srcWidthStep) == NULL)
		return { 0,0,0 };
	else
	{
		uchar* p = pSrc + (int)(round(tx)) + (int)(round(ty) * srcWidthStep);
		for (int i = 0; i < nChannels; i++)
			rbg[i] = p[i];
	}
	return rbg;
}
NearestNeighborInterpolate::NearestNeighborInterpolate() {}
NearestNeighborInterpolate::~NearestNeighborInterpolate() {}
int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator) {
	if (beforeImage.data == NULL || transformer == NULL || interpolator == NULL)
		return 0;
	Mat img = Mat(beforeImage);
	Mat temp(beforeImage.rows, beforeImage.cols, CV_32FC2);
	for (int y = 0; y < img.rows; y++) {
		uchar* p = img.ptr<uchar>(y);
		float* sp = temp.ptr<float>(y);
		for (int x = 0; x < img.cols; x++, p += img.channels(), sp += 2) {
			float tx = float(x);
			float ty = float(y);
			transformer->TransformPoint(tx, ty);
			sp[0] = tx;
			sp[1] = ty;
		}
	}
	return 1;
}


BilinearInterpolate::BilinearInterpolate() {
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



GeometricTransformer::GeometricTransformer() {}
GeometricTransformer::~GeometricTransformer() {}


int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL || sx < 0.001 || sy < 0.001)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Thông số ảnh destImage
	int dstRow = int(ceil(srcRow * sy));
	int dstCol = int(ceil(srcCol * sx));

	// Khởi tạo dstImage với kích thước đã tính
	dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Scale(1 / sx, 1 / sy);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL || newWidth <= 0 || newHeight <= 0)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính tỉ lệ scale
	float sx = float(newWidth) / srcCol;
	float sy = float(newHeight) / srcRow;

	// Thực hiện biến đổi thông qua phép Scale
	int ret = this->Scale(srcImage, dstImage, sx, sy, interpolator);
	return ret;
}


//////////////////////////////////-------------------------------------------------------------

int GeometricTransformer::RotateKeepImage(
	const cv::Mat & srcImage, cv::Mat & dstImage, float angle, PixelInterpolate * interpolator) {
	// Kiểm trả ảnh đầu vào
	if (!srcImage.data) {
		// Phát hiện lỗi: ảnh input ko tồn tại
		std::cout << "[EXCEPTION] Error with input image.\n";
		return 0; // Trả về 0
	}

	// Chiều rộng của ảnh source
	int widthSourceImage = srcImage.cols;

	// Chiều cao của ảnh source
	int heigthSourceImage = srcImage.rows;

	// Số channels của ảnh source
	int sourceChannels = srcImage.channels();

	size_t sourceWidthStep = srcImage.step[0];

	int dstWidth = (int)(widthSourceImage * cos(angle * PI / 180) + heigthSourceImage * sin(angle * PI / 180));
	int dstHeight = (int)(widthSourceImage * sin(angle * PI / 180) + heigthSourceImage * cos(angle * PI / 180));

	AffineTransform affineTransform;
	affineTransform.Translate(dstHeight / 2 - heigthSourceImage / 2, dstWidth / 2 - widthSourceImage / 2);
	affineTransform.Translate(-dstHeight / 2, -dstWidth / 2);
	affineTransform.Rotate(angle);
	affineTransform.Translate(dstHeight / 2, dstWidth / 2);

	cv::Mat inverseMatrix = affineTransform.getMatrixTransform().inv();

	affineTransform.setMatrixTransform(inverseMatrix);

	uchar* pSrc = srcImage.data;

	if (sourceChannels == 1) {
		dstImage = cv::Mat::zeros(dstHeight, dstWidth, CV_8UC1);
	}
	else if (sourceChannels == 3) {
		dstImage = cv::Mat::zeros(dstHeight, dstWidth, CV_8UC3);
	}
	else {
		std::cout << "[EXCEPTION]\n";

	}

	GeometricTransformer::Transform(srcImage, dstImage, &affineTransform, interpolator);
	inverseMatrix.release();
	affineTransform.~AffineTransform();
	return 1;
}


/*
Hàm xoay không bảo toàn nội dung ảnh theo góc xoay cho trước
Tham số
- srcImage: ảnh input
- dstImage: ảnh sau khi thực hiện phép xoay
- angle: góc xoay (đơn vị: độ)
- interpolator: biến chỉ định phương pháp nội suy màu
Trả về:
 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
 - 1: Nếu biến đổi thành công
*/
int GeometricTransformer::RotateUnkeepImage(
	const cv::Mat & srcImage,
	cv::Mat & dstImage,
	float angle,
	PixelInterpolate * interpolator) {

	// Kiểm trả ảnh đầu vào
	if (!srcImage.data) {
		// Phát hiện lỗi: ảnh input ko tồn tại
		std::cout << "[EXCEPTION] Error with input image.\n";
		return 0; // Trả về 0
	}

	// Chiều rộng của ảnh source
	int widthSourceImage = srcImage.cols;

	// Chiều cao của ảnh source
	int heigthSourceImage = srcImage.rows;

	// Số channels của ảnh source
	int sourceChannels = srcImage.channels();

	size_t sourceWidthStep = srcImage.step[0];

	AffineTransform affineTransform;

	affineTransform.Translate(-heigthSourceImage / 2, -widthSourceImage / 2);
	affineTransform.Rotate(angle);
	affineTransform.Translate(heigthSourceImage / 2, widthSourceImage / 2);

	cv::Mat inverseMatrix = affineTransform.getMatrixTransform().inv();
	affineTransform.setMatrixTransform(inverseMatrix);

	if (sourceChannels == 1) {
		dstImage = cv::Mat::zeros(heigthSourceImage, widthSourceImage, CV_8UC1);
	}
	else if (sourceChannels == 3) {
		dstImage = cv::Mat::zeros(heigthSourceImage, widthSourceImage, CV_8UC3);
	}
	else {
		std::cout << "[EXCEPTION]\n";

	}

	GeometricTransformer::Transform(srcImage, dstImage, &affineTransform, interpolator);

	inverseMatrix.release();
	affineTransform.~AffineTransform();
	return 1;
}



int GeometricTransformer::Transform(
	const Mat &beforeImage,
	Mat &afterImage,
	AffineTransform* transformer,
	PixelInterpolate* interpolator) {
		// Kiểm trả ảnh đầu vào
		if (!beforeImage.data) {
			// Phát hiện lỗi: ảnh input ko tồn tại
			std::cout << "[EXCEPTION] Error with input image.\n";
			return 0; // Trả về 0
		}
		// Chiều rộng của ảnh source
		int widthBeforeImage = beforeImage.cols;
		// Chiều cao của ảnh source
		int heigthBeforeImage = beforeImage.rows;
		// Số channels của ảnh source
		int sourceChannels = beforeImage.channels();
		// Width step của ảnh source
		size_t sourceWidthStep = beforeImage.step[0];
		// Lấy ma trận affine
		cv::Mat matrixTransform = transformer->getMatrixTransform();
		float B[] =
		{
			0, 0, 1.0,
		};
		cv::Mat P = cv::Mat(3, 1, CV_32FC1, B);
		// Con trỏ ảnh gốc
		uchar* pSrc = beforeImage.data;
		// Chiều cao của ảnh destination
		int heightAfterImage = afterImage.rows;
		// Chiều rộng của ảnh destination
		int widthAfterImage = afterImage.cols;
		for (int i = 0; i < heightAfterImage; i++)
		{
			uchar* pData = afterImage.ptr<uchar>(i);
			for (int j = 0; j < widthAfterImage; j++)
			{
				// Đặt Px = x, Py = y với x, y là tọa độ đối với tâm (0,0) là gốc trên cùng bên trái
				P.ptr<float>(0)[0] = i * 1.0;
				P.ptr<float>(1)[0] = j * 1.0;
				// Tìm tọa độ thực trên ảnh gốc
				cv::Mat srcP = matrixTransform * P;
				// tx, ty là index thực của điểm ảnh trên ma trận ảnh gốc
				float tx = srcP.ptr<float>(0)[0];
				float ty = srcP.ptr<float>(1)[0];
				// Chỉ xét tx, ty nằm trong ảnh gốc
				if (tx >= 0 && ty >= 0 && tx < heigthBeforeImage && ty < widthBeforeImage)
				{
					for (int c = 0; c < sourceChannels; c++) {
						// Áp dụng Interpolate cho từng channel
						pData[j * sourceChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, sourceWidthStep, sourceChannels)[i];
					}
				}
			}
		}
		// Giải phóng ma trận P
		P.release();
		// Giải phóng ma trận biến đổi
		matrixTransform.release();
		return 1;
}