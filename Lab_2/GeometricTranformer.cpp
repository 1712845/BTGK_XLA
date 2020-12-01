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


//int GeometricTransformer::Scale(
//	const Mat &srcImage,
//	Mat &dstImage,
//	float sx, float sy,
//	PixelInterpolate* interpolator) {
//
//	//sx phong to, sy thu nho
//
//	if (srcImage.data == NULL)
//		return 0;
//	int width = srcImage.cols, height = srcImage.rows;
//	int widthx = srcImage.cols * sx, heightx = srcImage.rows*sx;
//	int widthy = srcImage.cols/sy, heighty = srcImage.rows/sy;
//
//	int srcChannels = srcImage.channels();
//
//	if (srcChannels == 1) {
//		dstImage = cv::Mat(height, width, CV_8UC1);
//	}
//	else {
//		dstImage = cv::Mat(heightx, widthx, CV_8UC3);
//	}
//
//	int srcWidthStep = srcImage.step[0];
//	int dstWidthStep = dstImage.step[0];
//
//	uchar* pSrcData = srcImage.data;
//	uchar* pDstData = dstImage.data;
//
//	//sx
//	for (int y = 0; y < heightx; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep) {
//		uchar* pSrcRow = pSrcData;
//		uchar* pDstRow = pDstData;
//		for (int x = 0; x < width; x++, pSrcRow += srcChannels, pDstRow += dstChannels) {
//			for (int i = 0; i < srcChannels; i++) {
//				// saturate_cast<uchar> lam tron pixel ve [0..255] dung nhat vd 257->1 (sai) 
//				// => 257->255 (dung) 
//				pDstRow[i] = saturate_cast<uchar>(pSrcRow[i] * c);
//			}
//		}
//	}
//
//
//	return 1;
//}



///////////////

int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	if (beforeImage.data == NULL || beforeImage.channels() != afterImage.channels())
	{
		return 0;
	}
	// Thông số ảnh afterImage
	int dstRow = afterImage.rows;
	int dstCol = afterImage.cols;
	int dstWidthStep = afterImage.step1();

	// Thông số ảnh beforeImage
	int srcRow = beforeImage.rows;
	int srcCol = beforeImage.cols;
	int srcWidthStep = beforeImage.step1();

	int nChannel = beforeImage.channels();

	uchar* pSrcData = (uchar*)beforeImage.data;
	uchar* pDstData = (uchar*)afterImage.data;

	// Duyệt trên ảnh afterImage
	for (int y = 0; y < dstRow; y++)
	{
		for (int x = 0; x < dstCol; x++)
		{
			// Tìm tọa độ tương ứng tx, ty ở ảnh beforeImage qua ánh xạ ngược
			float tx = x, ty = y;
			transformer->TransformPoint(tx, ty);

			// Duyệt từng kênh màu, tìm màu nội suy
			for (int k = 0; k < nChannel; k++)
			{
				uchar color = 0;
				if (tx >= 0 && tx <= srcCol && ty >= 0 && ty <= srcRow)
				{
					color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannel);
				}

				// Gán màu nội suy cho điểm ảnh của afterImage
				int index = y * dstWidthStep + x * nChannel + k;
				pDstData[index] = color;
			}
		}
	}
	return 1;
}

int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol
	float w = float(srcCol);
	float h = float(srcRow);
	float diag = sqrt(w * w + h * h);

	float alpha0 = atan2(h, w);
	float new_w = max(fabs(diag * cos(-alpha0 + angle)),
		fabs(diag * cos(alpha0 + angle)));
	float new_h = max(fabs(diag * sin(-alpha0 + angle)),
		fabs(diag * sin(alpha0 + angle)));

	int dstRow = int(ceil(new_h));
	int dstCol = int(ceil(new_w));

	// Khởi tạo dstImage với kích thước thích hợp
	if (nChannel == 1)
		dstImage = Mat(dstRow, dstCol, CV_8UC1, Scalar(0));
	else if (nChannel == 3)
		dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Translate(-new_w / 2, -new_h / 2);
	affineTrans->Rotate(-angle);
	affineTrans->Translate(srcCol / 2.0f, srcRow / 2.0f);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol
	int dstRow = srcRow;
	int dstCol = srcCol;

	// Khởi tạo dstImage với kích thước thích hợp
	if (nChannel == 1)
		dstImage = Mat(dstRow, dstCol, CV_8UC1, Scalar(0));
	else if (nChannel == 3)
		dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Translate(-srcCol / 2.0f, -srcRow / 2.0f);
	affineTrans->Rotate(-angle);
	affineTrans->Translate(srcCol / 2.0f, srcRow / 2.0f);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

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

	// Tính toán dstRow, dstCol
	int dstRow = int(ceil(srcRow * sy));
	int dstCol = int(ceil(srcCol * sx));

	// Khởi tạo dstImage với kích thước thích hợp
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

int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
		return 0;

	int height = srcImage.rows;
	int width = srcImage.cols;
	int nChannels = srcImage.channels();

	//Khởi tạo dstImage bằng với chiều của srcImage
	dstImage = Mat(height, width, CV_8UC3, Scalar(0));

	//Lấy widthStep của srcImage và dstImage
	int dstWidthStep = dstImage.step1();
	int srcWidthStep = srcImage.step1();

	uchar* pSrcData = srcImage.data;
	uchar* pDstData = dstImage.data;

	//Trục ngang
	if (direction == 1) {
		//Lấy giá trị y ở giữa ảnh
		float axis_y = height / 2.0;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				//Giữ nguyên x
				float tx = x, ty;
				//Nếu y > axis_y thì y nằm ở trên trục, ngược lại y nằm dưới
				if (y >= axis_y)
					ty = y - axis_y;
				else
					ty = y + axis_y;

				// Duyệt từng kênh màu, tìm màu nội suy
				for (int k = 0; k < nChannels; k++)
				{
					uchar color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannels);
					int index = y * dstWidthStep + x * nChannels + k;
					pDstData[index] = color;
				}
			}
		}
	}
	//Trục đứng
	else {
		//Lấy giá trị x ở giữa ảnh
		float axis_x = width / 2.0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				//Giữ nguyên y
				float tx, ty = y;
				//Nếu x > axis_x thì y nằm ở bên phải trục, ngược lại y nằm bên trái trục
				if (x >= axis_x)
					tx = x - axis_x;
				else
					tx = x + axis_x;

				// Duyệt từng kênh màu, tìm màu nội suy
				for (int k = 0; k < nChannels; k++)
				{
					uchar color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannels);
					int index = y * dstWidthStep + x * nChannels + k;
					pDstData[index] = color;
				}
			}
		}
	}

	return 1;
}

GeometricTransformer::GeometricTransformer()
{
}

GeometricTransformer::~GeometricTransformer()
{
}

Mat matrixDot(const Mat& mat1, const Mat& mat2)
{
	Mat result = Mat(3, 3, CV_32FC1, Scalar(0));

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float sum = 0;
			for (int k = 0; k < 3; k++)
			{
				sum += mat1.at<float>(i, k) * mat2.at<float>(k, j);
			}
			result.at<float>(i, j) = sum;
		}
	}
	//std::cout << result << std::endl;
	return result;
}