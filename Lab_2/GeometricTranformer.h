#include"pch.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <math.h> // Tinh sin cos Ronate
#include <iostream>
using namespace std;
using namespace cv;
const float PI = 3.141592653589793238462643383279502884;
// Bỏ lớp transform lên đầu để sử dụng hàm để xây dựng các lớp khác
/*
Lớp biểu diễn pháp biến đổi affine
*/
class AffineTransform
{
	Mat _matrixTransform; //ma trận 3x3 biểu diễn phép biến đổi 
	Mat r_matrixTransform; //ma trận 3x3 biểu diễn phép biến đổi ngược
public:
	void Translate(float dx, float dy);// xây dựng matrix transform cho phép tịnh tiến theo vector (dx,dy)
	void Rotate(float angle);//xây dựng matrix transform cho phép xoay 1 góc angle
	void Scale(float sx, float sy);//xây dựng matrix transform cho phép tỉ lệ theo hệ số 		
	void TransformPoint(float &x, float &y);//transform 1 điểm (x,y) theo matrix transform đã có

	void r_TransformPoint(float &x, float &y); // transform thêm vào cho affine ngược
	AffineTransform();
	~AffineTransform();
};
/*
 Lớp base dùng để nội suy màu của 1 pixel
*/
class PixelInterpolate
{
public:
	/*
	Hàm tính giá trị màu của ảnh kết quả từ nội suy màu trong ảnh gốc
	Tham số
		- (tx,ty): tọa độ thực của ảnh gốc sau khi thực hiện phép biến đổi affine
		- pSrc: con trỏ ảnh gốc
		- srcWidthStep: widthstep của ảnh gốc
		- nChannels: số kênh màu của ảnh gốc
	Trả về
		- Giá trị màu được nội suy
	*/

	// sửa lại thành  vector<uchar>
	virtual vector<uchar> Interpolate(
		float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) = 0;
	PixelInterpolate();
	~PixelInterpolate();
};

/*
Lớp nội suy màu theo phương pháp song tuyến tính
*/
class BilinearInterpolate : public PixelInterpolate
{
	Mat src;// Giới hạn chiều cao và chiều dài ảnh
	AffineTransform* matrix; // cần affine gốc để có ma trận nghịch đảo
public:
	// Làm việc với nhiều kênh màu nên return uchar --> vector uchar
	//tx = nrow, ty = ncol
	vector<uchar> Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels);
	BilinearInterpolate(AffineTransform* affine, Mat source);
	void setAffineMatrix(AffineTransform* m);
	~BilinearInterpolate();
};

/*
Lớp nội suy màu theo phương pháp láng giềng gần nhất
*/
class NearestNeighborInterpolate : public PixelInterpolate
{
public:
	vector<uchar> Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels);
	NearestNeighborInterpolate();
	~NearestNeighborInterpolate();
};




/*
Lớp thực hiện phép biến đổi hình học trên ảnh
*/

class GeometricTransformer
{
public:
	/*
	Hàm biến đổi ảnh theo 1 phép biến đổi affine đã có
	Tham số
	 - beforeImage: ảnh gốc trước khi transform
	 - afterImage: ảnh sau khi thực hiện phép biến đổi affine
	 - transformer: phép biến đổi affine
	 - interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Transform(
		const Mat &beforeImage,
		Mat &afterImage,
		AffineTransform* transformer,
		PixelInterpolate* interpolator);

	/*
	Hàm xoay bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateKeepImage(
		const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator);

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
	int RotateUnkeepImage(
		const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator);

	/*
	Hàm phóng to, thu nhỏ ảnh theo tỉ lệ cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- sx, sy: tỉ lệ phóng to, thu nhỏ ảnh
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Scale(
		const Mat &srcImage,
		Mat &dstImage,
		float sx, float sy,
		PixelInterpolate* interpolator);


	/*
	Hàm thay đổi kích thước ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thay đổi kích thước
	- newWidth, newHeight: kích thước mới
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Resize(
		const Mat &srcImage,
		Mat &dstImage,
		int newWidth, int newHeight,
		PixelInterpolate* interpolator);

	/*
	Hàm lấy đối xứng ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi lấy đối xứng
	- direction = 1 nếu lấy đối xứng theo trục ngang và direction = 0 nếu lấy đối xứng theo trục đứng
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Flip(const Mat &srcImage,
		Mat &dstImage,bool direction,
		PixelInterpolate* interpolator);

	GeometricTransformer();
	~GeometricTransformer();
};



