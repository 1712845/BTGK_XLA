#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/core/types_c.h>
#include<string>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <vector>

using namespace cv;
//using namespace std;

class ColorTransformer
{
public:
	/*
	Hàm nhận vào một ảnh, thay đổi độ sáng của ảnh này và lưu kết quả vào ảnh mới
	Tham so:
		sourceImage	: ảnh ban đầu
		destinationImage: ảnh kết quả
		b	: giá trị số nguyên dùng để thay đổi độ sáng của ảnh
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b);

	/*
	Hàm nhận vào một ảnh, thay đổi độ tương phản của ảnh này và lưu kết quả vào ảnh mới
	Tham so :
		sourceImage : ảnh ban đầu
		destinationImage : ảnh kết quả
		c	: giá trị số thực dùng để thay đổi độ tương phản của ảnh
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c);


	/*
	Hàm tính lược đồ màu tổng quát cho ảnh bất kỳ
	Tham so :
		sourceImage : ảnh ban đầu
		histMatrix : ma trận histogram (nChannels x 256), mỗi dòng là 1 mảng 256 phần tử histogram của từng kênh màu
	Hàm trả về:
		1: Nếu thành công thì trả về matrix kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tính được histogram hoặc ảnh input không tồn tại
	*/
	int CalcHistogram(const Mat& sourceImage, Mat& histMatrix);

	/*
	Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
	Tham so :
		image : ảnh dùng để tính histogram
		histImage : ảnh histogram
	Hàm trả về:
		1: Nếu thành công vẽ được histogram
		0: Nếu không vẽ được histogram
	*/
	int HistogramEqualization(const Mat& sourceImage, Mat& destinationImage);



	/*
Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
Tham so :
	histMatrix : ma trận histogram đã tính được
	histImage : ảnh histogram được vẽ
Hàm trả về:
	1: Nếu thành công vẽ được histogram
	0: Nếu không vẽ được histogram
*/
	int DrawHistogram(const Mat& histMatrix, Mat& histImage);


	/*
	Hàm so sánh hai ảnh
	Tham so :
		image1 : ảnh thứ nhất
		image2 : ảnh thứ hai
	Hàm trả về:
		độ đo sự tương đồng giữa hai ảnh
	*/
	float CompareImage(const Mat& image1, Mat& image2);
	
	
	ColorTransformer();
	~ColorTransformer();
void drawhist2(Mat src) {

	std::vector<Mat> bgr_planes;
	split(src, bgr_planes);
	int histSize = 256;
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };
	bool uniform = true, accumulate = false;
	Mat b_hist, g_hist, r_hist;
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}
	imshow("Source image", src);
	imshow("calcHist Demo", histImage);
	waitKey();
}
};
